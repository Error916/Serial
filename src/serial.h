#ifndef SERIAL_H_
#define SERIAL_H_

#include <stdio.h>   /* Standard input/output definitions */
#include <stdlib.h>  /* Malloc and free */
#include <stdint.h>  /* Standard int definitions */
#include <string.h>  /* String function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */

#ifdef __unix__
#include <unistd.h>  /* UNIX standard function definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/ioctl.h>

typedef struct {
    int fd;
    struct termios options;
    int _cflag;
    int _iflag;
    int _ispeed;
    int _lflag;
    int _oflag;
    int _ospeed;
    cc_t _veof;
    cc_t _veol;
    cc_t _verase;
    cc_t _vintr;
    cc_t _vkill;
    cc_t _vquit;
    cc_t _vsusp;
    cc_t _vstart;
    cc_t _vstop;
    cc_t _vmin;
    cc_t _vtime;
    int c_cflag;
    int c_iflag;
    int c_ispeed;
    int c_lflag;
    int c_oflag;
    int c_ospeed;
    cc_t c_veof;
    cc_t c_veol;
    cc_t c_verase;
    cc_t c_vintr;
    cc_t c_vkill;
    cc_t c_vquit;
    cc_t c_vsusp;
    cc_t c_vstart;
    cc_t c_vstop;
    cc_t c_vmin;
    cc_t c_vtime;
    int iossiospeed_baud;
    int rconst;
    int rtot;
    char name[256]; // should be the path lenght limit
    char alias[256];
} serial_t;
#endif

#ifdef _WIN32
#include <windows.h>

typedef struct {
    HANDLE port;
    COMMTIMEOUTS _timeouts;
    DCB _state;
    COMMTIMEOUTS timeouts;
    DCB state;
    char name[256]; // should be the path lenght limit
} serial_t;

typedef long long int ssize_t;
#endif

// https://www.msweet.org/serial/serial.html#2_1
// https://metacpan.org/dist/Device-SerialPort/source/SerialPort.pm
// https://www.pololu.com/docs/0J73/15.6

serial_t* serial_new(const char* name);
void serial_destroy(serial_t* s);

int serial_write_settings(serial_t* s);

ssize_t serial_write(serial_t* s, uint8_t* data, uint64_t datalen);
ssize_t serial_read(serial_t* s, uint8_t* buf, uint64_t toread);

int serial_can_baud(serial_t* s);
int serial_can_databits(serial_t* s);
int serial_can_stopbits(serial_t* s);
int serial_can_dtrdsr(serial_t* s);
int serial_can_handshake(serial_t* s);
int serial_can_parity_check(serial_t* s);
int serial_can_parity_config(serial_t* s);
int serial_can_parity_enable(serial_t* s);
int serial_can_rlsd(serial_t* s);
int serial_can_16bitmode(serial_t* s);
int serial_is_rs232(serial_t* s);
int serial_is_modem(serial_t* s);
int serial_can_rtscts(serial_t* s);
int serial_can_xonxoff(serial_t* s);
int serial_can_xon_char(serial_t* s);
int serial_can_spec_char(serial_t* s);
int serial_can_interval_timeout(serial_t* s);
int serial_can_total_timeout(serial_t* s);
int serial_binary(serial_t* s);
int serial_reset_error(serial_t* s);
int serial_can_ioctl(serial_t* s);
#endif
