/* sintactico.h */
#ifndef SINTACTICO_H
#define SINTACTICO_H

#include "turbo.h"
#include "lexico.h"

extern VALOR v;
/*      Definicion del SEGMENTO DE DATOS                */
extern ADDRESS
    ds[DATASIZE],           /* El segmento de datos         */
    *dx,                    /* El apuntador a las constantes */
    *px;                    /* El apuntador a las temporales */
extern DATO d;              /* El dato temporal             */
extern ADDRESS
    cs[CODESIZE],           /* El segmento de codigo        */
    *pc,                    /* El contador de programa      */
    *ip;                    /* El apuntador al codigo       */
/*      Definicion de Stack para comprobar tipos        */
extern ADDRESS
    ti,                     /* Indice de Tipos              */
    ts[TIPOSIZE];           /*  Estructura de tipos             */


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