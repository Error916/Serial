#ifndef PRINTER_H_
#define PRINTER_H_

#include <stdio.h>
#include <stdlib.h>

#include "serial.h"
#include "Citizen_CL_S521.h"

typedef enum {
    CITIZEN_CL_S521,
    NUM_PRINTER_MODELS
} printer_model;

typedef struct {
    serial_t *port;
    const char* (*checkprinter)(serial_t *s);
    const char* (*gotoXY)(uint32_t x, uint32_t y, 
        uint32_t sx, uint32_t sy, uint32_t sdx, uint32_t sdy, uint32_t sdir);
    const char* (*printtext)(const char *text);
    const char* (*printcode39)(const char *text);
    const char* (*printcode128)(const char *text);
    const char* (*printimage)(const char *text, uint32_t size);
    const char* (*setcode39size)(void);
    const char* (*setcode128size)(void);
    const char* (*setfontsize)(void);
    const char* (*setfont)(uint8_t value, char dir);
    const char* (*setsize)(uint32_t width, uint32_t height);
    const char* (*setunderline)(void);
    const char* (*setemphasize)(void);
    const char* (*setdstrike)(void);
    const char* (*setreverse)(void);
    const char* (*initpriter)(void);
    const char* (*initticket)(uint32_t width, uint32_t height,
            uint32_t marker, uint32_t markerpos, uint32_t cutter);
    const char* (*startprintarea)(uint32_t x, uint32_t y,
            uint32_t dx, uint32_t dy, char dir, const char *stream);
    const char* (*setprintarea)(uint32_t x, uint32_t y,
            uint32_t dx, uint32_t dy, char dir, const char *stream);
    const char* (*printticket)(uint32_t marker, uint32_t cutter);
    const char* (*shutdown)(void);
} printer_t;

printer_t *printer_new(printer_model model, const char* portname);
void printer_destroy(printer_t *p);

const char *printer_checkprinter(serial_t *s);
const char *printer_gotoXY(uint32_t x, uint32_t y, 
        uint32_t sx, uint32_t sy, uint32_t sdx, uint32_t sdy, uint32_t sdir);
const char *printer_printtext(const char *text);
const char *printer_printcode39(const char *text);
const char *printer_printcode128(const char *text);
const char *printer_printimage(const char *text, uint32_t size);
const char *printer_setcode39size(void);
const char *printer_setcode128size(void);
const char *printer_setfontsize(void);
const char *printer_setfont(uint8_t value, char dir);
const char *printer_setsize(uint32_t width, uint32_t height);
const char *printer_setunderline(void);
const char *printer_setemphasize(void);
const char *printer_setdstrike(void);
const char *printer_setreverse(void);
const char *printer_initpriter(void);
const char *printer_initticket(uint32_t width, uint32_t height,
        uint32_t marker, uint32_t markerpos, uint32_t cutter);
const char *printer_startprintarea(uint32_t x, uint32_t y,
        uint32_t dx, uint32_t dy, char dir, const char *stream);
const char *printer_setprintarea(uint32_t x, uint32_t y,
        uint32_t dx, uint32_t dy, char dir, const char *stream);
const char *printer_printticket(uint32_t marker, uint32_t cutter);
const char *printer_shutdown(void);

#endif
