/* lexico.h */
#ifndef LEXICO_H
#define LEXICO_H

#include "turbo.h" // Ensures it knows the SIMBOLO type

/*      DATA SEGMENT Definition                */
extern ADDRESS
    ds[DATASIZE],           /* The data segment         */
    *dx,                    /* The pointer to constants */
    *px;                    /* The pointer to temporaries */

extern DATO d;              /* The temporary data             */
extern int linea, nivel, si, indecl, inargs, inproc, inasgn, inrang, array, rtn; 
extern char *bfr, *pun;
extern unsigned bsize;
extern VALOR de_memoria( unsigned s, unsigned o, ADDRESS t );
extern void ensambla( ADDRESS seg, uint16_t off ), error( char *s );

extern VALOR v;

int scanner(), follow( int expect, int ifyes, int ifno ),
    next(), reservada( char *s ), busca ( char *s );

void  ctes_predef(), inserta_simbolo( char *n, int t );

#endif /* LEXICO_H */
