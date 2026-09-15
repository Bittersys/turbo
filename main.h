/*
main.h
    Formato de dim: 
        2 bytes para el tipo de dato.            Ej. CHAR=0x01, INTEGER=0x02
        1 byte para las dimensiones del arreglo. Ej. 1..5, entonces 0x1; 1..5,1..4 entonces 2
        2 bytes para el numero de objetos en el arreglo. Ej. Si 1..5 entonces 5 elementos
*/
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
    ADDRESS *s;
} VALOR;

typedef struct activacion {
    unsigned codigo;
    ADDRESS *datos;
    struct activacion *ptr;
} ACTIVACION;

/*      Definicion del SEGMENTO DE CODIGO               */
ADDRESS
    cs[CODESIZE],           /* El segmento de codigo        */
    *pc,                    /* El contador de programa      */
    *ip;                    /* El apuntador al codigo       */

/*      Definicion del SEGMENTO DE DATOS                */
ADDRESS
    ds[DATASIZE],           /* El segmento de datos         */
    *dx,                    /* El apuntador a las constantes */
    *px;                    /* El apuntador a las temporales */

/*      Definicion del SEGMENTO DE STACK                */
DATO d,                     /* El dato temporal             */
    *sp,                    /* El apuntador al stack        */
    ss[STACKSIZE];          /* El segmento de stack         */

/*      Definicion de REGISTROS DE ACTIVACION           */
ACTIVACION
    *rs[REGSIZE];           /* Arreglo de registros de activacion */

unsigned
    ri;                     /* Indice de Activacion Actual  */

/*      Definicion de Stack para comprobar tipos        */
ADDRESS
    ti,                     /* Indice de Tipos              */
    ts[TIPOSIZE];           /*  Estructura de tipos             */

VALOR 
    v;

/*      Estructura de datos para el uso de identificadores      */
struct {
    char        *nombre;
    unsigned    nivel,
                tbloque,
                offset;
    ADDRESS     tipo,
                dims[16],
                args[16];
} simbolos[128] = {
    "_MAIN_PROC",0,0,0,_PROC,0,0
};

long iz,szs[16];

/*
    Definicion de prototipos
*/
char *fota( double n ), *wsave( int ri, int ci, int rf, int cf ),
    *readfile( char *s ), *strlwr(char *str), *itoa(int n, int buf, int base);

int _compila( char *filename ), objsize( DATO o, int a ), tipo( int *qt ),
    rangolist(), scanner(), follow( int expect, int ifyes, int ifno ),
    next(), reservada( char *s ), busca ( char *s ), 
    a_memoria( ADDRESS *ptr, ADDRESS *org, ADDRESS t );

unsigned copia( DATO hasta, DATO desde, int arr ),
    lleva( DATO hasta, DATO desde, int arr ),
    trae( DATO hasta, DATO desde, int arr );

void _ejecuta(), _listado(), qtipo ( int t ), _lst(), _log(),
    _wrt(),_rea(), _psh( ADDRESS t, ADDRESS s, uint16_t o ),
    _alc( unsigned codigo, unsigned n ), _fre(), _add(), _sub(),
    _mul(), _div(), _mod(), _anc(), _orc(), _noc(), _ltc(), _lec(),
    _gtc(), _gec(), _nec(), _eqc(), _sav(), _asg(), _inc(), _dec(),
    _cpy( ADDRESS n ), _rst( ADDRESS *n ), _eva( ADDRESS *c ), 
    _uns( ADDRESS n ), _cll( ADDRESS *codigo ), 
    ctes_predef(), analisis(), constantes(), declaraciones(),
    tipo_direccion( int i, int f, int s ),
    proc_func( int actual ), procedures( int actual ),
    functions( int actual ), arglist(), programa(), sentencia(),
    sentenlist(), ifcode(), whilecode(), forcode(), repeatcode(),
    casecode(), proccode(), funccode(), retcode(int s, int r),
    writecode(), readcode(), bloque(), expresion(), sublogica(), 
    elemento(), equivale(), operacion(), termino(), final(),
    comparacion(), relacional(), logica_and(), logica_or(),
    adiciona(), multiplica(), factor(), negacion(), idcode(),
    match( int t ), inserta_simbolo( char *n, int t ),
    ensambla( ADDRESS seg, uint16_t off ), chktipo( ADDRESS o ),
    cursor( int x, int y ), show( char *s ),
    text( char *s, char x, char y, char a ),
    wrest( int ri, int ci, int rf, int cf, char *ptr ),
    wpain( int ri, int ci, int rf, int cf, int atr ),
    wmark( int ri, int ci, int rf, int cf ),
    error( char *s ), msglines(), _grabar(), cargar(),
    showTypeStack(), debugDim(ADDRESS *d),
    dumpDS(), dumpDSK(), dumpRS( int n ), dumpSS(), dumpCS();

ADDRESS *_ifc( ADDRESS *c ), *_cac( ADDRESS *c ), idlist(), chkarg( ADDRESS arg );

VALOR de_memoria( unsigned s, unsigned o, ADDRESS t );
