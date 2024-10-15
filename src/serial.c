#include "serial.h"

serial_t *serial_new(const char *name, uint8_t quiet) {
    serial_t *s = malloc(sizeof(serial_t));
    strncpy(s->name, name, 256);
    (void) quiet; // ignore quiet param
    s->fd = open(name, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (s->fd == -1) {
	    perror("serial_new: Unable to open port - ");
        free(s);
        return NULL;
    }
    ssize_t ok = tcgetattr(s->fd, &s->options);
    if ( ok < 0) {
        close(s->fd);
        free(s);
        return NULL;
    }

    s->_CFLAG = s->options.c_cflag;
    s->_IFLAG = s->options.c_iflag;
    s->_ISPEED = s->options.c_ispeed;
    s->_LFLAG = s->options.c_lflag;
    s->_OFLAG = s->options.c_oflag;
    s->_OSPEED = s->options.c_ospeed;

    return s;
}

ssize_t serial_write(serial_t *s, uint8_t *data, uint64_t datalen) {
    ssize_t n = write(s->fd, data, datalen);
    if ( n < datalen )
        printf("write of %ld bytes failed\n", datalen);
    return n;
}

void serial_read_char_time(serial_t *s, uint64_t timems) {}

void serial_read_const_time(serial_t *s, uint64_t timems) {}

ssize_t serial_read(serial_t *s, uint8_t *buf, uint64_t toread, uint8_t noblocking) {
    if ( noblocking )
        fcntl(s->fd, F_SETFL, FNDELAY);
    ssize_t n = read(s->fd, buf, toread);
    fcntl(s->fd, F_SETFL, 0);
    return n;
}

void serial_destroy(serial_t *s) {
    close(s->fd);
    free(s);
}
