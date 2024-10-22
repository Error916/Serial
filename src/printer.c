#include "printer.h"
#include "serial.h"

printer_t *printer_new(printer_model model, const char *portname) { 
    printer_t *printer = malloc(sizeof(printer_t));

    printer->port = serial_new(portname); 
    if (printer->port == NULL) {
        free(printer);
        return NULL;
    }
    
    switch (model) {
        case CITIZEN_CL_S521:
            printer->checkprinter = Citizen_CL_S521_checkprinter;
            printer->gotoXY = Citizen_CL_S521_gotoXY;
            printer->printtext = Citizen_CL_S521_printtext;
            printer->printcode39 = Citizen_CL_S521_printcode39;
            printer->printcode128 = Citizen_CL_S521_printcode128;
            printer->printimage = Citizen_CL_S521_printimage;
            printer->setcode39size = Citizen_CL_S521_setcode39size;
            printer->setcode128size = Citizen_CL_S521_setcode128size;
            printer->setfontsize = Citizen_CL_S521_setfontsize;
            printer->setfont = Citizen_CL_S521_setfont;
            printer->setsize = Citizen_CL_S521_setsize;
            printer->setunderline = Citizen_CL_S521_setunderline;
            printer->setemphasize = Citizen_CL_S521_setemphasize;
            printer->setdstrike = Citizen_CL_S521_setdstrike;
            printer->setreverse = Citizen_CL_S521_setreverse;
            printer->initpriter = Citizen_CL_S521_initpriter;
            printer->initticket = Citizen_CL_S521_initticket;
            printer->startprintarea = Citizen_CL_S521_startprintarea;
            printer->setprintarea = Citizen_CL_S521_setprintarea;
            printer->printticket = Citizen_CL_S521_printticket;
            printer->shutdown = Citizen_CL_S521_shutdown;
            break;
        default:
            // TODO: Print Error
            printer->checkprinter = printer_checkprinter;
            printer->gotoXY = printer_gotoXY;
            printer->printtext = printer_printtext;
            printer->printcode39 = printer_printcode39;
            printer->printcode128 = printer_printcode128;
            printer->printimage = printer_printimage;
            printer->setcode39size = printer_setcode39size;
            printer->setcode128size = printer_setcode128size;
            printer->setfontsize = printer_setfontsize;
            printer->setfont = printer_setfont;
            printer->setsize = printer_setsize;
            printer->setunderline = printer_setunderline;
            printer->setemphasize = printer_setemphasize;
            printer->setdstrike = printer_setdstrike;
            printer->setreverse = printer_setreverse;
            printer->initpriter = printer_initpriter;
            printer->initticket = printer_initticket;
            printer->startprintarea = printer_startprintarea;
            printer->setprintarea = printer_setprintarea;
            printer->printticket = printer_printticket;
            printer->shutdown = printer_shutdown;
    }

    return printer;
}

void printer_destroy(printer_t *p) {
    serial_destroy(p->port);
    free(p);
}

const char *printer_checkprinter(serial_t *s) { return NULL; }
const char *printer_gotoXY(uint32_t x, uint32_t y, 
        uint32_t sx, uint32_t sy, uint32_t sdx, uint32_t sdy, uint32_t sdir) { return NULL; }
const char *printer_printtext(const char *text) { return NULL; }
const char *printer_printcode39(const char *text) { return NULL; }
const char *printer_printcode128(const char *text) { return NULL; }
const char *printer_printimage(const char *text, uint32_t size) { return NULL; }
const char *printer_setcode39size(void) { return NULL; }
const char *printer_setcode128size(void) { return NULL; }
const char *printer_setfontsize(void) { return NULL; }
const char *printer_setfont(uint8_t value, char dir) { return NULL; }
const char *printer_setsize(uint32_t width, uint32_t height) { return NULL; }
const char *printer_setunderline(void) { return NULL; }
const char *printer_setemphasize(void) { return NULL; }
const char *printer_setdstrike(void) { return NULL; }
const char *printer_setreverse(void) { return NULL; }
const char *printer_initpriter(void) { return NULL; }
const char *printer_initticket(uint32_t width, uint32_t height,
        uint32_t marker, uint32_t markerpos, uint32_t cutter) { return NULL; }
const char *printer_startprintarea(uint32_t x, uint32_t y,
        uint32_t dx, uint32_t dy, char dir, const char *stream) { return NULL; }
const char *printer_setprintarea(uint32_t x, uint32_t y,
        uint32_t dx, uint32_t dy, char dir, const char *stream) { return NULL; }
const char *printer_printticket(uint32_t marker, uint32_t cutter) { return NULL; }
const char *printer_shutdown(void) { return NULL; }
