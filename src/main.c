#include <stdio.h>

#include "printer.h"

int main(int argc, char **argv) {

    printer_t *p = printer_new(CITIZEN_CL_S521, "/dev/ttyS0");

    printf("%s\n", p->printtext("ciaooooooo"));

    printer_destroy(p);

    return 0;
}
