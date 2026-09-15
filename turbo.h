/*
turbo.h
    Formato de dim: 
        2 bytes para el tipo de dato.            Ej. CHAR=0x01, INTEGER=0x02
        1 byte para las dimensiones del arreglo. Ej. 1..5, entonces 0x1; 1..5,1..4 entonces 2
        2 bytes para el numero de objetos en el arreglo. Ej. Si 1..5 entonces 5 elementos
*/
#ifndef TURBO_H
#define TURBO_H

#include <unistd.h>
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>

#define TIPOSIZE    128
#define CODESIZE    4096
#define DATASIZE    4096
#define STACKSIZE   1024
#define REGSIZE     128

enum instrucciones {
    ADD,SUB,MUL,MOD,DIV,PSH,ASG,REA,WRT,IFC,CAC,
    GTC,LTC,EQC,LEC,GEC,NEC,ORC,ANC,NOC,MEU,JMP,
    INC,DEC,RTN,CLL,FRE,ALC,CPY,SAV,UNS,RST,EVA,
    STK,FAR
};

enum tokens {
    ID = 256, NUM,CAR,CONST,VAR,_ARRAY,OF,BEGIN,END,READ,READLN,
    _TRUE,_FALSE,_PI,WRITE,WRITELN,IF,THEN,ELSE,WHILE,FOR,DO,
    GE,GT,PREDEF,LT,LE,EQ,NE,AS,OR,AND,NOT,REPEAT,UNTIL,
    CASE,TO,DOWNTO,PROCEDURE,FUNCTION,GOTOXY,PP,MDL
};

enum objetos {
    _UNDEF       =0x00,
    _CONSTANTE   =0x10,
    _VARIABLE    =0x20,
    _DIRECCION   =0x30,
    _LOCAL       =0x40,
    _PROC        =0x50,
    _FUNC        =0x60,
    _REFE        =0x70,
    _LABEL       =0x80
};

enum tipos {
    _CHAR     =0x01,
    _INTEGER  =0x02,
    _REAL     =0x03,
    _STRING   =0x04,
    _BOOLEAN  =0x05,
    _ARREGLO  =0x08
};

#define INT_WIDTH            ( sizeof(uint16_t) )
#define poplocal(n)         ((si) = (n))
#define reset_px            ( (px) = (dx) )
#define CONST_SEG           ( 0 )
#define TIPO(t)             ( ((t)&0x7) )
#define IVAL(t)             ( *((t)+1)<<8|*(t) )
#define ISCONST(x)          ( ((x)>>4)==1 )
#define ISLOCAL(x)          ( !(x)?0:(ri)-(x) )
#define ISVAR(t)            ( ((t)&_VARIABLE)==_VARIABLE )

typedef unsigned char ADDRESS;

typedef struct {
    ADDRESS t,s;
    uint16_t o;     /* Permite 64K Segmentos de 64K bytes */
} DATO;

typedef union valor {
    uint16_t i;
    char c;
    double d;
    char *s;
} VALOR;

typedef struct activacion {
    unsigned codigo;
    ADDRESS *datos;
    struct activacion *ptr;
} ACTIVACION;

/*      Estructura de datos para el uso de identificadores      */
typedef struct {
    char        *nombre;
    unsigned    nivel,
                tbloque,
                offset;
    ADDRESS     tipo,
                dims[16],
                args[16];
} SIMBOLO;

/*
    Definicion de prototipos
*/
char *fota( double n ), *wsave( int ri, int ci, int rf, int cf ),
    *readfile( char *s ), *strlwr(char *str), *itoa(int n);

int _compila( char *filename ), tipo( int *qt );

void _ejecuta(), _listado(), show(char *s), init_runtime(),
    text(char *s), mostrar_ayuda(const char *prog_name),
    error(char *s), msglines(), _grabar(), _cargar(),
    showTypeStack(), debugDim(ADDRESS *d),
    dumpDS(), dumpDSK(), dumpRS( int n ), dumpSS(), dumpCS();

ADDRESS *_ifc( ADDRESS *c ), *_cac( ADDRESS *c );

extern int a_memoria( ADDRESS *ptr, ADDRESS *org, ADDRESS t );

#endif /* TURBO_H */