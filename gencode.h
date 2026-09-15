/*
    gencode.h
*/
#ifndef GENCODE_H
#define GENCODE_H

#include "turbo.h"

extern char    *progname;
extern ADDRESS
    cs[CODESIZE],           /* El segmento de codigo        */
    *pc,                    /* El contador de programa      */
    *ip;                    /* El apuntador al codigo       */

void _lst();

#endif /* GENCODE_H */