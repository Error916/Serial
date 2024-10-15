#include "serial.h"

#ifdef __unix__

serial_t* serial_new(const char* name, uint8_t quiet) {
    serial_t* s = malloc(sizeof(serial_t));
    strncpy(s->name, name, 256);
    (void)quiet; // ignore quiet param
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
    s->_vsto = s->options.c_cc[VSTO];
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

    if (can_ioctl(s)) {
        perror("serial_new: Port not usable to iotcl - ");
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
    s->options.c_cflag      = s->c_cflag;
    s->options.c_iflag      = s->c_iflag;
    s->options.c_ispeed     = s->c_ispeed;
    s->options.c_lflag      = s->c_lflag;
    s->options.c_oflag      = s->c_oflag;
    s->options.c_ospeed     = s->c_ospeed;
    s->options.c_cc[VEOF]   = s->c_veof;
    s->options.c_cc[VEOL]   = s->c_veol;
    s->options.c_cc[VERASE] = s->c_verase;
    s->options.c_cc[VINTR]  = s->c_vintr;
    s->options.c_cc[VKILL]  = s->c_vkill;
    s->options.c_cc[VQUIT]  = s->c_vquit;
    s->options.c_cc[VSUSP]  = s->c_vsusp;
    s->options.c_cc[VSTART] = s->c_vstart;
    s->options.c_cc[VSTO]   = s->c_vsto;
    s->options.c_cc[VMIN]   = s->c_vmin;
    s->options.c_cc[VTIME]  = s->c_vtime;

    return tcsetattr(s->fd, TCSANOW, &s->options);
}

ssize_t serial_write(serial_t* s, uint8_t* data, uint64_t datalen) {
    ssize_t n = write(s->fd, data, datalen);
    if (n < datalen)
        printf("write of %ld bytes failed\n", datalen);
    return n;
}

ssize_t serial_read(serial_t* s, uint8_t* buf, uint64_t toread) {
    if (toread == 0) return 0;
    size_t done = 0;
    size_t count_in = 0;
    uint8_t string_in[255] = { 0 };
    uint8_t *in2 = buf;
    size_t bufsize = 255; // VMIN max

    while (done < toread) {
        size_t size = toread - done;
        if (size > bufsize) size = bufsize;
        size_t rin = 0;
        rin |= (1 << s->fd); // vec(rin, fd, 1) = 1 TODO capire meglio
        struct timeval timer = { 0 };
        timer.tv_usec= s->RCONST + (toread * s->RTOT);
        int ready = select(s->fd, &rin, NULL, &rin, &timer);
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
int serial_can_baud(serial_t* s)                { (void)s; return 1; }
int serial_can_databits(serial_t* s)            { (void)s; return 1; }
int serial_can_stopbits(serial_t* s)            { (void)s; return 1; }
int serial_can_dtrdsr(serial_t* s)              { (void)s; return 1; }
int serial_can_handshake(serial_t* s)           { (void)s; return 1; }
int serial_can_parity_check(serial_t* s)        { (void)s; return 1; }
int serial_can_parity_config(serial_t* s)       { (void)s; return 1; }
int serial_can_parity_enable(serial_t* s)       { (void)s; return 1; }
int serial_can_rlsd(serial_t* s)                { (void)s; return 0; } // currently
int serial_can_16bitmode(serial_t* s)           { (void)s; return 0; } // Win32 - specific
int serial_is_rs232(serial_t* s)                { (void)s; return 1; }
int serial_is_modem(serial_t* s)                { (void)s; return 0; } // Win32 - specific
int serial_can_rtscts(serial_t* s)              { (void)s; return 1; } // this is a flow option
int serial_can_xonxoff(serial_t* s)             { (void)s; return 1; } // this is a flow option
int serial_can_xon_char(serial_t* s)            { (void)s; return 1; } // use stty
int serial_can_spec_char(serial_t* s)           { (void)s; return 0; } // use stty
int serial_can_interval_timeout(serial_t* s)    { (void)s; return 0; } // currently
int serial_can_total_timeout(serial_t* s)       { (void)s; return 1; } // currently
int serial_binary(serial_t* s)                  { (void)s; return 1; }
int serial_reset_error(serial_t* s)             { (void)s; return 0; } // for compatibility

int serial_can_ioctl(serial_t *s) {
    int status;
    ioctl(s->fd, TIOCMGET, &status);
    if ((status & TIOCMBIS) && (status & TIOCMBIC) && (status & TIOCM_RTS) &&
        (((status & TIOCSDTR) && (status & TIOCCDTR)) || (status & TIOCM_DTR)))
        return 1;
    return 0;
}

#endif

#ifdef _WIN32
serial_t* serial_new(const char* name, uint8_t quiet) { return NULL; }
void serial_destroy(serial_t* s) {}

int serial_write_settings(serial_t* s) { return 0; }

ssize_t serial_write(serial_t* s, uint8_t* data, uint64_t datalen) { return 0; }
ssize_t serial_read(serial_t* s, uint8_t* buf, uint64_t toread, uint8_t noblocking) { return 0; }

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
