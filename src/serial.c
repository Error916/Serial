#include "serial.h"

#ifdef __unix__

serial_t* serial_new(const char* name) {
    serial_t* s = malloc(sizeof(serial_t));
    strncpy(s->name, name, 256);
    s->fd = open(name, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (s->fd == -1) {
        perror("serial_new: Unable to open port - ");
        free(s);
        return NULL;
    }
    ssize_t ok = tcgetattr(s->fd, &s->options);
    if (ok < 0) {
        perror("serial_new: Unable to get termios attributes - ");
        close(s->fd);
        free(s);
        return NULL;
    }

    // saving the original values
    s->_cflag = s->options.c_cflag;
    s->_iflag = s->options.c_iflag;
    s->_ispeed = s->options.c_ispeed;
    s->_lflag = s->options.c_lflag;
    s->_oflag = s->options.c_oflag;
    s->_ospeed = s->options.c_ospeed;
    s->_veof = s->options.c_cc[VEOF];
    s->_veol = s->options.c_cc[VEOL];
    s->_verase = s->options.c_cc[VERASE];
    s->_vintr = s->options.c_cc[VINTR];
    s->_vkill = s->options.c_cc[VKILL];
    s->_vquit = s->options.c_cc[VQUIT];
    s->_vsusp = s->options.c_cc[VSUSP];
    s->_vstart = s->options.c_cc[VSTART];
    s->_vstop = s->options.c_cc[VSTOP];
    s->_vmin = s->options.c_cc[VMIN];
    s->_vtime = s->options.c_cc[VTIME];

    // copy the original values into "current" value
    s->c_cflag = s->_cflag;
    s->c_iflag = s->_iflag;
    s->c_ispeed = s->_ispeed;
    s->c_lflag = s->_lflag;
    s->c_oflag = s->_oflag;
    s->c_ospeed = s->_ospeed;
    s->c_veof = s->_veof;
    s->c_veol = s->_veol;
    s->c_verase = s->_verase;
    s->c_vintr = s->_vintr;
    s->c_vkill = s->_vkill;
    s->c_vquit = s->_vquit;
    s->c_vsusp = s->_vsusp;
    s->c_vstart = s->_vstart;
    s->c_vstop = s->_vstop;
    s->c_vmin = s->_vmin;
    s->c_vtime = s->_vtime;

    // default to "raw" mode
    s->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | IGNPAR | INPCK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    s->c_oflag &= ~OPOST;
    s->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

    // sane port
    s->c_iflag |= IGNBRK;
    s->c_cflag |= (CLOCAL | CREAD);

    // 9600 baud
    s->c_ispeed = B9600;
    s->c_ospeed = B9600;

    // 8data bits
    s->c_cflag &= ~CSIZE;
    s->c_cflag |= CS8;

    // disable parity
    s->c_cflag &= ~(PARENB | PARODD);

    // 1 stop bit
    s->c_cflag &= ~CSTOPB;

    s->iossiospeed_baud = -1;

    serial_write_settings(s);

    strncpy(s->alias, s->name, 256);

    // private data
    s->rconst = 0;
    s->rtot = 0;

    if (!serial_can_ioctl(s)) {
        fprintf(stderr, "serial_new: Port not usable to iotcl\n");
        close(s->fd);
        free(s);
        return NULL;
    }

    return s;
}

void serial_destroy(serial_t* s) {
    close(s->fd);
    free(s);
}

int serial_write_settings(serial_t* s) {
    s->options.c_cflag = s->c_cflag;
    s->options.c_iflag = s->c_iflag;
    cfsetispeed(&s->options, s->c_ispeed);
    s->options.c_lflag = s->c_lflag;
    s->options.c_oflag = s->c_oflag;
    cfsetospeed(&s->options, s->c_ospeed);
    s->options.c_cc[VEOF] = s->c_veof;
    s->options.c_cc[VEOL] = s->c_veol;
    s->options.c_cc[VERASE] = s->c_verase;
    s->options.c_cc[VINTR] = s->c_vintr;
    s->options.c_cc[VKILL] = s->c_vkill;
    s->options.c_cc[VQUIT] = s->c_vquit;
    s->options.c_cc[VSUSP] = s->c_vsusp;
    s->options.c_cc[VSTART] = s->c_vstart;
    s->options.c_cc[VSTOP] = s->c_vstop;
    s->options.c_cc[VMIN] = s->c_vmin;
    s->options.c_cc[VTIME] = s->c_vtime;

    return tcsetattr(s->fd, TCSANOW, &s->options);
}

ssize_t serial_write(serial_t* s, uint8_t* data, uint64_t datalen) {
    if (data == NULL) return 0;
    if (datalen <= 0) return 0;
    return write(s->fd, data, datalen);
}

ssize_t serial_read(serial_t* s, uint8_t* buf, uint64_t toread) {
    if (toread == 0) return 0;
    size_t done = 0;
    size_t count_in = 0;
    uint8_t string_in[255] = { 0 };
    size_t bufsize = 255; // VMIN max

    while (done < toread) {
        size_t size = toread - done;
        if (size > bufsize) size = bufsize;
        fd_set rin, ein;
        FD_ZERO(&rin);
        FD_SET(s->fd, &rin);
        ein = rin;
        int nfds = s->fd + 1;
        struct timeval timer = { 0 };
        timer.tv_usec = s->rconst + (toread * s->rtot);
        int ready = select(nfds, &rin, NULL, &ein, &timer);
        printf("%d\n", ready);
        if (ready > 0)
            count_in = read(s->fd, string_in, size);
        if (count_in) {
            strcat(buf, string_in);
            done += count_in;
        }
        else if (done) break;
        else return 0;
    }
    return done;
}

// true/false capabilities (read only)
int serial_can_baud(serial_t* s) { (void)s; return 1; }
int serial_can_databits(serial_t* s) { (void)s; return 1; }
int serial_can_stopbits(serial_t* s) { (void)s; return 1; }
int serial_can_dtrdsr(serial_t* s) { (void)s; return 1; }
int serial_can_handshake(serial_t* s) { (void)s; return 1; }
int serial_can_parity_check(serial_t* s) { (void)s; return 1; }
int serial_can_parity_config(serial_t* s) { (void)s; return 1; }
int serial_can_parity_enable(serial_t* s) { (void)s; return 1; }
int serial_can_rlsd(serial_t* s) { (void)s; return 0; } // currently
int serial_can_16bitmode(serial_t* s) { (void)s; return 0; } // Win32 - specific
int serial_is_rs232(serial_t* s) { (void)s; return 1; }
int serial_is_modem(serial_t* s) { (void)s; return 0; } // Win32 - specific
int serial_can_rtscts(serial_t* s) { (void)s; return 1; } // this is a flow option
int serial_can_xonxoff(serial_t* s) { (void)s; return 1; } // this is a flow option
int serial_can_xon_char(serial_t* s) { (void)s; return 1; } // use stty
int serial_can_spec_char(serial_t* s) { (void)s; return 0; } // use stty
int serial_can_interval_timeout(serial_t* s) { (void)s; return 0; } // currently
int serial_can_total_timeout(serial_t* s) { (void)s; return 1; } // currently
int serial_binary(serial_t* s) { (void)s; return 1; }
int serial_reset_error(serial_t* s) { (void)s; return 0; } // for compatibility

int serial_can_ioctl(serial_t* s) {
    int status;
    ioctl(s->fd, TIOCMGET, &status);
    if ((status & TIOCMBIS) && (status & TIOCMBIC) && (status & TIOCM_RTS) && (status & TIOCM_DTR))
        return 1;
    return 0;
}

#endif

#ifdef _WIN32
serial_t* serial_new(const char* name) {
    serial_t* s = malloc(sizeof(serial_t));
    strncpy_s(s->name, 256, name, 256);
    s->port = CreateFileA(name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH, NULL);
    if (s->port == INVALID_HANDLE_VALUE) {
        free(s);
        return NULL;
    }

    // Flush away any bytes previously read or written.
    if (!FlushFileBuffers(s->port)) {
        CloseHandle(s->port);
        free(s);
        return NULL;
    }

    // get original values
    if (!GetCommTimeouts(s->port, &s->_timeouts)) {
        CloseHandle(s->port);
        free(s);
        return NULL;
    }
    if (!GetCommState(s->port, &s->_state)) {
        CloseHandle(s->port);
        free(s);
        return NULL;
    }

    // copy the original values into "current" value
    s->timeouts.ReadIntervalTimeout = s->_timeouts.ReadIntervalTimeout;
    s->timeouts.ReadTotalTimeoutConstant = s->_timeouts.ReadTotalTimeoutConstant;
    s->timeouts.ReadTotalTimeoutMultiplier = s->_timeouts.ReadTotalTimeoutMultiplier;
    s->timeouts.WriteTotalTimeoutConstant = s->_timeouts.WriteTotalTimeoutConstant;
    s->timeouts.WriteTotalTimeoutMultiplier = s->_timeouts.WriteTotalTimeoutMultiplier;
    s->state.DCBlength = s->_state.DCBlength;
    s->state.BaudRate = s->_state.BaudRate;
    s->state.ByteSize = s->_state.ByteSize;
    s->state.Parity = s->_state.Parity;
    s->state.StopBits = s->_state.StopBits;

    // we make the change we need
    s->timeouts.ReadIntervalTimeout = 0;
    s->timeouts.ReadTotalTimeoutConstant = 1000;
    s->timeouts.ReadTotalTimeoutMultiplier = 0;
    s->timeouts.WriteTotalTimeoutConstant = 100;
    s->timeouts.WriteTotalTimeoutMultiplier = 0;
    s->state.DCBlength = sizeof(DCB);
    s->state.BaudRate = 9600;
    s->state.ByteSize = 8;
    s->state.Parity = NOPARITY;
    s->state.StopBits = ONESTOPBIT;

    if (!serial_write_settings(s)) {
        CloseHandle(s->port);
        free(s);
        return NULL;
    }

    return s;
}

void serial_destroy(serial_t* s) {
    CloseHandle(s->port);
    free(s);
}

int serial_write_settings(serial_t* s) {
    if (!SetCommTimeouts(s->port, &s->timeouts)) return 0;
    if (!SetCommState(s->port, &s->state)) return 0;

    return 1;
}

ssize_t serial_write(serial_t* s, uint8_t* data, uint64_t datalen) {
    if (data == NULL) return -1;
    if (datalen <= 0) return -1;
    DWORD written;
    if (!WriteFile(s->port, data, datalen, &written, NULL)) return -1;
    return written;
}

ssize_t serial_read(serial_t* s, uint8_t* buf, uint64_t toread) {
    DWORD received;

    if (!ReadFile(s->port, buf, toread, &received, NULL)) return -1;

    return received;
}

int serial_can_baud(serial_t* s) { (void)s; return 0; }
int serial_can_databits(serial_t* s) { (void)s; return 0; }
int serial_can_stopbits(serial_t* s) { (void)s; return 0; }
int serial_can_dtrdsr(serial_t* s) { (void)s; return 0; }
int serial_can_handshake(serial_t* s) { (void)s; return 0; }
int serial_can_parity_check(serial_t* s) { (void)s; return 0; }
int serial_can_parity_config(serial_t* s) { (void)s; return 0; }
int serial_can_parity_enable(serial_t* s) { (void)s; return 0; }
int serial_can_rlsd(serial_t* s) { (void)s; return 0; }
int serial_can_16bitmode(serial_t* s) { (void)s; return 0; }
int serial_is_rs232(serial_t* s) { (void)s; return 0; }
int serial_is_modem(serial_t* s) { (void)s; return 0; }
int serial_can_rtscts(serial_t* s) { (void)s; return 0; }
int serial_can_xonxoff(serial_t* s) { (void)s; return 0; }
int serial_can_xon_char(serial_t* s) { (void)s; return 0; }
int serial_can_spec_char(serial_t* s) { (void)s; return 0; }
int serial_can_interval_timeout(serial_t* s) { (void)s; return 0; }
int serial_can_total_timeout(serial_t* s) { (void)s; return 0; }
int serial_binary(serial_t* s) { (void)s; return 0; }
int serial_reset_error(serial_t* s) { (void)s; return 0; }
int serial_can_ioctl(serial_t* s) { (void)s; return 0; }
#endif
