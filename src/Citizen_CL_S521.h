#ifndef CITIZEN_CL_S521_H_
#define CITIZEN_CL_S521_H_
#include <stdint.h>

#include "serial.h"

const char *Citizen_CL_S521_checkprinter(serial_t *s);
const char *Citizen_CL_S521_gotoXY(uint32_t x, uint32_t y, 
        uint32_t sx, uint32_t sy, uint32_t sdx, uint32_t sdy, uint32_t sdir);
const char *Citizen_CL_S521_printtext(const char *text);
const char *Citizen_CL_S521_printcode39(const char *text);
const char *Citizen_CL_S521_printcode128(const char *text);
const char *Citizen_CL_S521_printimage(const char *text, uint32_t size);
const char *Citizen_CL_S521_setcode39size(void);
const char *Citizen_CL_S521_setcode128size(void);
const char *Citizen_CL_S521_setfontsize(void);
const char *Citizen_CL_S521_setfont(uint8_t value, char dir);
const char *Citizen_CL_S521_setsize(uint32_t width, uint32_t height);
const char *Citizen_CL_S521_setunderline(void);
const char *Citizen_CL_S521_setemphasize(void);
const char *Citizen_CL_S521_setdstrike(void);
const char *Citizen_CL_S521_setreverse(void);
const char *Citizen_CL_S521_initpriter(void);
const char *Citizen_CL_S521_initticket(uint32_t width, uint32_t height,
        uint32_t marker, uint32_t markerpos, uint32_t cutter);
const char *Citizen_CL_S521_startprintarea(uint32_t x, uint32_t y,
        uint32_t dx, uint32_t dy, char dir, const char *stream);
const char *Citizen_CL_S521_setprintarea(uint32_t x, uint32_t y,
        uint32_t dx, uint32_t dy, char dir, const char *stream);
const char *Citizen_CL_S521_printticket(uint32_t marker, uint32_t cutter);
const char *Citizen_CL_S521_shutdown(void);

#endif
