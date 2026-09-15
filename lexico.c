#include "lexico.h"

static char *p, _bfr[256];

int ps,si,str_lon;
SIMBOLO simbolos[128] = {
    {"_MAIN_PROC",0,0,0,_PROC,{0},{0}}
};

int scanner() {

    int t;
    while ( bfr-pun < bsize ) {
        t = next();
        if ( t == ' ' || t == '\t' || t == '\r' )
             ;
        else if ( t == '\n' ) {
             linea++;
             /*msglines();*/
        }
        else if ( isdigit(t) ) {
             int f=0;

             p = _bfr;
        regresa:
             while ( isdigit(t) ) {
                 *p ++ = t;
                 t = next();
             }
             if ( t == '.' && !f && !array ) {
                 f = 1;
                 *p ++ = t;
                 t = next();
                 goto regresa;
             }
             bfr --;
             *p = 0;

             if ( f ) {
                 d.t = _CONSTANTE|_REAL;
                 v.d = atof(_bfr);

                 /*sscanf(bfr,"%lf",&v.d);*/

                 a_memoria(dx,(ADDRESS*)&v.d,d.t);
                 if ( !indecl ) ensambla(CONST_SEG,dx-ds);
                 dx += sizeof(double);
             } else {
                 d.t = _CONSTANTE|_INTEGER;
                 v.i = atoi(_bfr);
                 a_memoria(dx,(ADDRESS*)&v.i,d.t);
                 if ( !indecl ) ensambla(CONST_SEG,dx-ds);
                 dx += INT_WIDTH;
             }
             return(NUM);
        }
        else if ( isalpha(t) ) {
            p = _bfr;
            while ( isalnum(t) || t == '_' ) {
                *p ++ = t;
                t = next();
            }
            bfr --;
            *p = 0;            
            p = strlwr(_bfr);
            if ( (ps=reservada(p)) ) return(ps);
            else if ( (ps=busca(p)) == -1 ) {
                ps = si;
                if ( indecl || inproc || inargs ) inserta_simbolo(p,_UNDEF);
                else error("Undefined variable.");
                return(_UNDEF);
            }
            d.t = simbolos[ps].tipo;
            switch ( d.t&0xF0 ) {
                case  _PROC:
                    d.t = simbolos[ps].tipo&0x0F;
                    return(_PROC);
                case  _FUNC:
                    d.t = simbolos[ps].tipo&0x0F;
                    return(_FUNC);
                case  _CONSTANTE:
                    v = de_memoria(simbolos[ps].tbloque,simbolos[ps].offset,d.t);
                    return(CONST);
                case  _VARIABLE:
                case  _REFE:
                    if ( ((d.t&0x0F)&_ARREGLO) == _ARREGLO )
                        return(_ARRAY);
                    return(ID);
            }
        }
        else if ( t == '{' ) {
             char *s,tmp[256];

             s = &tmp[1];
             while ( 1 ) {
                 t = next();
                 if ( t == '\n' ) {
                     linea++;
                     /*msglines();*/
                 } else if ( t == '}' )
                     break;
                 *s++ = t;
                 if ( (s-tmp) > 256 ) error("Comment too long or missing delimiter");
             }
             *s = 0;
        }
        else if ( t == '\'' ) {
             char *s;

             s = &_bfr[1];
             str_lon = 0;
             while ( 1 ) {
                 t = next();
                 if ( t == '\'' || t == '\n' )
                     break;
                 *s++ = t;
                 str_lon ++;
                 if ( str_lon > 256 ) error("String too long");
             }
             *s = 0;

             if ( t != '\'' )
                 error("Incorrect delimiter");

             if ( str_lon <= 1 ) {
                 v.c = _bfr[1];
                 d.t = _CONSTANTE|_CHAR;
                 a_memoria(dx,(ADDRESS*)&v.c,d.t);
                 if ( !indecl ) ensambla(CONST_SEG,dx-ds);
                 dx ++;
                 return(CAR);
             } else {
                 _bfr[0] = str_lon;                 
                 v.s = _bfr;
                 d.t = _CONSTANTE|_STRING;
                 a_memoria(dx, (ADDRESS *)v.s, d.t);
                 if ( !indecl ) ensambla(CONST_SEG,dx-ds);
                 dx += str_lon+2;
                 return(_STRING);
             }
        }
        else {
             switch ( t ) {
                 case '.': return(follow('.',PP,'.'));
                 case ':': return(follow('=',AS,':'));
                 case '<': return(follow('=',LE,follow('>',NE,LT)));
                 case '>': return(follow('=',GE,GT));
                 case '=': return(EQ);
                 default : return(t);
             }
        }
    }
    return(0);
}

int follow( int expect, int ifyes, int ifno ) {
    int c = next();

    if ( c == expect )
        return( ifyes );
    bfr --;
    return( ifno );
}

int next() {

    if ( bfr-pun <= bsize ) return(*bfr ++);
    else return(0);
}

/*      Reserved Words Management           */

struct {
    char *p;
    int t;
} reservadas[] = {
    {"const", CONST},
    {"var", VAR},
    {"char", _CHAR},
    {"integer", _INTEGER},
    {"real", _REAL},
    {"string", _STRING},
    {"boolean", _BOOLEAN},
    {"true", _TRUE},
    {"false", _FALSE},
    {"pi", _PI},
    {"if", IF},
    {"then", THEN},
    {"else", ELSE},
    {"while", WHILE},
    {"for", FOR},
    {"to", TO},
    {"downto", DOWNTO},
    {"repeat", REPEAT},
    {"until", UNTIL},
    {"case", CASE},
    {"do", DO},
    {"and", AND},
    {"or", OR},
    {"not", NOT},
    {"mod", MDL},
    {"procedure", PROCEDURE},
    {"function", FUNCTION},
    {"array", _ARRAY},
    {"of", OF},
    {"begin", BEGIN},
    {"end", END},
    {"read", READ},
    {"readln", READLN},
    {"write", WRITE},
    {"writeln", WRITELN},
    {0,     0}
};

int reservada( char *s ) {
    int i;

    for ( i=0; reservadas[i].t; i++ ) {
        if ( !strcmp(reservadas[i].p,s) ) {
            return(reservadas[i].t);
        }
    }
    return(0);
}

/*      Symbol Table Management             */

int busca ( char *s ) {
    int i;

    if ( (inargs || indecl || inproc) && !inrang ) {
        for ( i=si-1; i >= 0; i-- ) {
            if ( !strcmp(simbolos[i].nombre,s) && simbolos[i].nivel == (unsigned)nivel )
                return(i);
        }
    } else {
        for ( i=si-1; i >= 0; i-- ) {
            if ( !strcmp(simbolos[i].nombre,s) && simbolos[i].nivel <= (unsigned)nivel )
                return(i);
        }
    }
    return(-1);
}

void inserta_simbolo( char *n, int t ) {

    simbolos[si].nombre = malloc( strlen(n)+1 );
    strcpy(simbolos[si].nombre,n);
    simbolos[si].nivel = nivel;
    simbolos[si].tipo = t;
    si ++;
}

void ctes_predef() {

    v.c = '\n';
    d.t = _CONSTANTE|_CHAR;
    a_memoria(dx++,(ADDRESS*)&v,d.t);

    v.c = 1;
    d.t = _CONSTANTE|_BOOLEAN;
    a_memoria(dx++,(ADDRESS*)&v,d.t);

    v.c = 0;
    d.t = _CONSTANTE|_BOOLEAN;
    a_memoria(dx++,(ADDRESS*)&v,d.t);

    v.d = 3.14159265359;
    d.t = _CONSTANTE|_REAL;
    a_memoria(dx,(ADDRESS*)&v,d.t); dx += sizeof(double);

}
