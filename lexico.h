/* lexico.h */
#ifndef LEXICO_H
#define LEXICO_H

#include "turbo.h" // Asegura que conozca el tipo SIMBOLO

/*      Definicion del SEGMENTO DE DATOS                */
extern ADDRESS
    ds[DATASIZE],           /* El segmento de datos         */
    *dx,                    /* El apuntador a las constantes */
    *px;                    /* El apuntador a las temporales */

extern DATO d;              /* El dato temporal             */
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
