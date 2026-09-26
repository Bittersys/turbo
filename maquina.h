/*
    maquina.h
*/
#ifndef MAQUINA_H
#define MAQUINA_H

#include "turbo.h"

extern void error( char *s );

/*      Definicion del SEGMENTO DE DATOS                */
extern ADDRESS
    ds[DATASIZE],           /* El segmento de datos         */
    *dx,                    /* El apuntador a las constantes */
    *px;                    /* El apuntador a las temporales */

/*      Definicion del SEGMENTO DE STACK                */
extern DATO 
    d,                      /* El dato temporal             */
    *sp,                    /* El apuntador al stack        */
    ss[STACKSIZE];          /* El segmento de stack         */

/*      Definicion del SEGMENTO DE CODIGO               */
extern ADDRESS
    cs[CODESIZE],           /* El segmento de codigo        */
    *pc,                    /* El contador de programa      */
    *ip;                    /* El apuntador al codigo       */

extern long iz,szs[16];

extern VALOR v;

extern ACTIVACION *rs[REGSIZE]; /* Arreglo de registros de activacion */
extern unsigned ri;             /* Indice de Activacion Actual  */

void _log(), _wrt(), _rea(), _psh( ADDRESS t, ADDRESS s, uint16_t o ),
    _alc( unsigned codigo, unsigned n ), _fre(), _add(), _sub(), _meu(),
    _mul(), _div(), _mod(), _anc(), _orc(), _noc(), _ltc(), _lec(), _cst(),
    _gtc(), _gec(), _nec(), _eqc(), _sav(), _asg(), _inc(), _dec(),
    _cpy( ADDRESS n ), _rst( ADDRESS *n ), _eva( ADDRESS *c ), 
    _uns( ADDRESS n ), _cll( ADDRESS *codigo ), 
    _stk( ADDRESS arg, ADDRESS cod );

unsigned copia( DATO hasta, DATO desde, int arr ),
    lleva( DATO hasta, DATO desde, int arr ),
    trae( DATO hasta, DATO desde, int arr );

int objsize( DATO o, int a ), a_memoria( ADDRESS *ptr, ADDRESS *org, ADDRESS t );
VALOR de_memoria( unsigned s, unsigned o, ADDRESS t );

#endif /* MAQUINA_H */
