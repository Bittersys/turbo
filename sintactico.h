/* sintactico.h */
#ifndef SINTACTICO_H
#define SINTACTICO_H

#include "turbo.h"
#include "lexico.h"

extern VALOR v;
/*      DATA SEGMENT Definition                */
extern ADDRESS
    ds[DATASIZE],           /* The data segment         */
    *dx,                    /* The pointer to constants */
    *px;                    /* The pointer to temporaries */
extern DATO d;              /* The temporary data             */
extern ADDRESS
    cs[CODESIZE],           /* The code segment        */
    *pc,                    /* The program counter      */
    *ip;                    /* The pointer to the code       */
/*      Stack Definition for type checking        */
extern ADDRESS
    ti,                     /* Type Index              */
    ts[TIPOSIZE];           /*  Type structure             */


extern SIMBOLO simbolos[128];

extern int ps,si,linea,str_lon;
extern int scanner();
extern void error( char *s );

void analisis(), constantes(), declaraciones(),
    tipo_direccion( int i, int f, int s ),
    proc_func(), procedures(),
    functions(), arglist(), programa(), sentencia(),
    sentenlist(), ifcode(), whilecode(), forcode(), repeatcode(),
    casecode(), proccode(), funccode(), retcode(int s, int r),
    writecode(), readcode(), bloque(), expresion(), sublogica(), 
    elemento(), equivale(), operacion(), termino(), final(),
    comparacion(), relacional(), logica_and(), logica_or(), 
    adiciona(), multiplica(), factor(), negacion(), idcode(),match( int t ),
    ensambla( ADDRESS seg, uint16_t off ), chktipo( ADDRESS o );

ADDRESS idlist(), chkarg( ADDRESS arg );
uint16_t rangolist();

#endif /* SINTACTICO_H */
