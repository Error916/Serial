#include <stdio.h>

#include "serial.h"

int main(int argc, char **argv) {

    serial_t *s = serial_new("/dev/ttyS0", 0);
    if (s == NULL)
        printf("error creating to serial\n");

    if ( serial_write(s, "ciao", 5) < 5)
        printf("error writing to serial\n");

    s->rconst = 100 * 1000;

    char str[10];
    int read = serial_read(s, str, 10);
    printf("read %d, %s\n", read, str);

    serial_destroy(s);

    return 0;
}
