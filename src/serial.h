#ifndef SERIAL_H_
#define SERIAL_H_

#include <stdio.h>   /* Standard input/output definitions */
#include <stdlib.h>  /* Malloc and free */
#include <stdint.h>  /* Standard int definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

typedef struct {
    int fd;
    struct termios options;
    int _CFLAG;
    int _IFLAG;
    int _ISPEED;
    int _LFLAG;
    int _OFLAG;
    int _OSPEED;
    char name[256]; // should be the path lenght limit
} serial_t;

// https://www.msweet.org/serial/serial.html#2_1
// https://metacpan.org/dist/Device-SerialPort/source/SerialPort.pm
// https://stackoverflow.com/questions/8666378/detect-windows-or-linux-in-c-c
ssize_t serial_write(serial_t *s, uint8_t *data, uint64_t datalen);
void serial_read_char_time(serial_t *s, uint64_t timems);
void serial_read_const_time(serial_t *s, uint64_t timems);
ssize_t serial_read(serial_t *s, uint8_t *buf, uint64_t toread, uint8_t noblocking);

serial_t *serial_new(const char *name, uint8_t quiet);
void serial_destroy(serial_t *s);

#endif
