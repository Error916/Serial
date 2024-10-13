#include "serial.h"

void serial_write(serial_t *s, char *str) {}

void serial_read_char_time(serial_t *s, uint64_t timems) {}

void serial_read_const_time(serial_t *s, uint64_t timems) {}

uint8_t *serial_read(serial_t *s, uint64_t toread, uint64_t *readed) { return NULL; }
