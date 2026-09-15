/*
Notas:
    Esta version corrige los saltos IF con el tamaño correcto
    de bytes. Por default usa 2 para int, aqui se corrige con el
    tipo de datos unit16_t.

    Siguiente paso revisar todas las instrucciones y sus saltos
    para que avance solo 2 bytes.

    Actualmente el error es que se avanzan 2 bytes al puntero pero 
    se copian datos tipo unsigned con tamaño de bytes.

    _ARRAY =    Palabra reservada
    ARREGLO =   Tipo de datos
    chkarg retorna void y se pretende usar el valor de retorno
*/

#include "main.h"

int     control, parserok, linea;
jmp_buf ambiente;
FILE    *out;
char    *progname,
        *bfr,*pun;
long    ini,fin;
unsigned bsize,SysError;

int main( int argc, char ** argv ) {

    printf("Compilando...\n");
    if ( _compila( argv[1] ) != linea  ) {
        
        printf("Generando código...\n");
        _listado();    

        printf("Ejecutando...\n");
        _ejecuta();
    }
}

int _compila( char *filename ) {
    int      pausa;
    char    *p;

    pun      =
    bfr      = readfile(filename);
    bsize    = strlen(bfr);
    progname = filename;
    pausa    = 0;
    SysError = 0;
    control = setjmp(ambiente);
    if ( SysError ) goto fin;

    ip = cs;            /* Inicia apuntador de Instrucciones */
    dx = ds;            /* Inicia apuntador de Datos Constantes*/
    sp = ss;            /* Inicia apuntador de Stack          */
                        /*                                    */
    ri = 0;             /* Inicia Indice de Activaciones      */
    rs[ri] = (ACTIVACION *) malloc(sizeof(*rs[0]));
    rs[ri]->codigo= -1;
    rs[ri]->datos = ds;
    rs[ri]->ptr = NULL;

    ti = 0;             /* Inicia Indice de Chequeo de tipos */
    ctes_predef();
    analisis();

    fin:;
        _grabar();

    if ( SysError ) return(linea);
    else return(0);
}

void _ejecuta() {

    SysError = 0;
    control = setjmp(ambiente);    
    if ( SysError ) goto fin;
    px = dx;        /* Asigna direccion inicial de la pila */
    iz = 0;
    _cll(pc);
    fin:;
}

void _listado() {

    SysError = 0;
    control = setjmp(ambiente);
    if ( SysError ) goto fin;

    px = dx;        /* Asigna direccion inicial de la pila */
    _lst();
    fin:;
}

void qtipo ( int t ) {
    char p[64];

    memset(p,0,64);
    switch( t&0xf0 ) {
        case  _PROC:     fprintf(out,"Procedure          ");
                         return;
        case  _FUNC:     strcpy(p,"Function   \t"); break;
        case  _REFE:     strcpy(p,"Referencia \t"); break;
        case  _CONSTANTE: strcpy(p,"_CONSTANTE\t"); break;
        case  _VARIABLE: strcpy(p,"_VARIABLE  \t"); break;
        default:         strcpy(p,""); break;
    }
    strcat(p,(t&_ARREGLO)==_ARREGLO?"_ARREGLO ":"         ");
    switch( t&0x07 ) {
        case  _INTEGER: strcat(p,"Integer\t"); break;
        case  _REAL:    strcat(p,"Real   \t"); break;
        case  _CHAR:    strcat(p,"Char   \t"); break;
        case  _STRING:  strcat(p,"String \t"); break;
        case  _BOOLEAN: strcat(p,"Boolean\t"); break;
    }
    fprintf(out,"%s",p);
}

void _lst() {

    int i;
    char *name;
    ADDRESS *codigo = cs,
            *segmento,tipo;

    name = strcat(strtok(progname,"."),".mas");
    out = fopen(name,"w");
    fprintf(out,"\n%s\n",name);
    while ( codigo < ip ) {
        if ( codigo == pc ) fprintf(out,"INICIO >\n");
        fprintf(out,"\t%.4X-%.4lX ",0,codigo-cs);
        switch( *codigo ) {
            case  ADD:  fprintf(out,"[1]SUM\n");
                        codigo ++;
                        break;
            case  SUB:  fprintf(out,"[1]SUB\n");
                        codigo ++;
                        break;
            case  MUL:  fprintf(out,"[1]MUL\n");
                        codigo ++;
                        break;
            case  MOD:  fprintf(out,"[1]MOD\n");
                        codigo ++;
                        break;
            case  DIV:  fprintf(out,"[1]DIV\n");
                        codigo ++;
                        break;
            case  ASG:  fprintf(out,"[1]ASG\n");
                        codigo ++;
                        break;
            case  LTC:  fprintf(out,"[1]LTC\n");
                        codigo ++;
                        break;
            case  LEC:  fprintf(out,"[1]LEC\n");
                        codigo ++;
                        break;
            case  GTC:  fprintf(out,"[1]GTC\n");
                        codigo ++;
                        break;
            case  GEC:  fprintf(out,"[1]GEC\n");
                        codigo ++;
                        break;
            case  EQC:  fprintf(out,"[1]EQC\n");
                        codigo ++;
                        break;
            case  NEC:  fprintf(out,"[1]NEC\n");
                        codigo ++;
                        break;
            case  INC:  fprintf(out,"[1]INC\n");
                        codigo ++;
                        break;
            case  DEC:  fprintf(out,"[1]DEC\n");
                        codigo ++;
                        break;
            case  NOC:  fprintf(out,"[1]NOC\n");
                        codigo ++;
                        break;
            case  ORC:  fprintf(out,"[1]ORC\n");
                        codigo ++;
                        break;
            case  ANC:  fprintf(out,"ANC\n");
                        codigo ++;
                        break;
            case  WRT:  fprintf(out,"WRT\n");
                        codigo ++;
                        break;
            case  REA:  fprintf(out,"REA\n");
                        codigo ++;
                        break;
            case  IFC:  fprintf(out,"IFC %.4X\n",IVAL(codigo+1));
                        codigo += 3;
                        break;
            case  CAC:  fprintf(out,"CAC %.4X\n"
                                    "%.4X\n",codigo[1],IVAL(codigo+2));
                        codigo += 4;
                        break;
            case  JMP:  fprintf(out,"JMP %.4X\n",IVAL(codigo+1));
                        codigo += 3;
                        break;
            case  RTN:  fprintf(out,"RTN\n");
                        codigo++;
                        break;
            case  CLL:  fprintf(out,"CLL %.4X:%.4X\n",0,codigo[1]);
                        codigo += 2;
                        break;
            case  ALC:  fprintf(out,"ALC %.4X:%.4d\n",IVAL(codigo+1),IVAL(codigo+3));
                        codigo += 5;
                        break;
            case  CPY:  fprintf(out,"CPY %.4d\n",codigo[1]);
                        codigo += 2;
                        break;
            case  UNS:  fprintf(out,"UNS %.4d\n",codigo[1]);
                        codigo += 2;
                        break;
            case  RST:  fprintf(out,"RST %4.4d %4.4d",codigo[1],codigo[2]);
                        codigo +=3;
                        while ( *codigo != 0xFF )
                            fprintf(out,",%4.4d",*codigo++);
                        fprintf(out,"\n");
                        codigo++;
                        break;
            case  FRE:  fprintf(out,"FRE\n");
                        codigo ++;
                        break;
            case  SAV:  fprintf(out,"SAV\n");
                        codigo ++;
                        break;
            case  EVA:  fprintf(out,"EVA %4.4d %4.4d",IVAL(codigo+1),IVAL(codigo+4));
                        for ( i=0; i < codigo[3] ; i ++ )
                            fprintf(out,",[%4.4d:%4.4d]",IVAL(codigo+i*4+6),
                            IVAL(codigo+i*4+8));
                            fprintf(out,"\n");
                        codigo += codigo[3]*4+6;
                        break;
            case  FAR:  fprintf(out,"FAR %4.4d %4.4d\n"
                                    "%4.4d\n",IVAL(codigo+1),codigo[3],IVAL(codigo+4));
                        codigo += 6;
                        break;
            case  PSH:  fprintf(out,"PSH "); qtipo(codigo[1]);
                        fprintf(out,"%4.4d:%4.4d\n",codigo[2],IVAL(codigo+3));
                        codigo += 5;
                        break;
            case  STK:  fprintf(out,"STK %4.4d:%4.4d\n",codigo[1],codigo[2]);
                        codigo += 3;
                        break;
            default:    fprintf(out,"ERR %4.4d\n",*codigo);  codigo ++;  break;
        }
    }
    fprintf(out,"%.4X-%.4lX FIN\n",0,codigo-cs);
    fclose(out);
}

void _wrt() {
    int dc,sg;
    char *s;
    d = * --sp;
    v = de_memoria(d.s,d.o,d.t);

    switch(d.t&0x7) {
        case  _INTEGER: printf("%7d",v.i);                break;
        case  _REAL:    printf("%s",fota(v.d));            break;
        case  _CHAR:    printf("%c",v.c);                  break;
        case  _STRING:  printf("%s",v.s);                  break;
        case  _BOOLEAN: printf("%s",v.c==1?"TRUE":"FALSE"); break;
    }
}

void _rea() {
    DATO x;
    char tmp[256];
    VALOR n;

    d = * --sp;
    fgets(&tmp[1],256,stdin);

    x.t = TIPO(d.t);
    switch ( x.t ) {
        case  _CHAR:    *px = tmp[1];
                        break;
        case  _REAL:    n.d = atof(tmp+1);
                        memcpy(px,(char *)&n.d,sizeof(double));
                        break;
        case  _INTEGER: n.i = atoi(tmp+1);
                        memcpy(px,(char *)&n.i,INT_WIDTH);
                        break;
        case  _STRING:  *px = strlen(tmp+1)+1;
                        memcpy(px+1,tmp+1,strlen(tmp+1)+1);
                        break;
        default:        error("Ejecucion.Dato no aceptado por read");
                        break;
    }
    x.s = ri; x.o = px-ds;
    copia(d,x,0);
}

void _psh( ADDRESS t, ADDRESS s, uint16_t o ) {
    d.t = t;
    d.s = ISCONST(t)?ri:s;        /* Controlar */
    d.o = o;
    *sp ++ = d;
}

void _alc( unsigned codigo, unsigned n ) {
    ACTIVACION *t;

    t = (ACTIVACION *) malloc(sizeof(*t));
    t->codigo = codigo;
    t->datos = malloc(n);
    memset(t->datos,'X',n);

    if ( rs[ri]->codigo != codigo ) {
        t->ptr = NULL;
        ri ++;
    } else
        t->ptr = rs[ri];

    rs[ri] = t;
}

void _fre() {
    ACTIVACION *t;

    t = rs[ri];
    if ( t->ptr != NULL ) rs[ri] = t->ptr;
    else
        ri --;

    iz = 0;
    free(t->datos);
    free(t);
}

void _add() {
    VALOR w;
    ADDRESS t1,t2,tmp[256];

    d = * --sp; t2 = d.t; v = de_memoria(d.s,d.o,d.t);
    d = * --sp; t1 = d.t; w = de_memoria(d.s,d.o,d.t);
    if ( TIPO(t1) == _REAL && TIPO(t2) == _REAL )
        w.d += v.d;
    if ( TIPO(t1) == _INTEGER && TIPO(t2) == _INTEGER )
        w.i += v.i;
    if ( TIPO(t1) == _REAL && TIPO(t2) == _INTEGER )
        w.d += v.i;
    if ( TIPO(t1) == _INTEGER && TIPO(t2) == _REAL )
        { v.d += w.i; w.d = v.d; d.t = _REAL; }

    if (( TIPO(t1) == _CHAR || TIPO(t1) == _STRING ) &&
        ( TIPO(t2) == _CHAR || TIPO(t2) == _STRING )) {
        _psh(_STRING,ri,px-ds);
        memset(tmp,0,256);
        tmp[0] = TIPO(t1)==_CHAR?1:w.s[0];
        tmp[0] += TIPO(t2)==_CHAR?1:v.s[0];
        if ( TIPO(t1) == _CHAR ) tmp[1] = w.c;
        else strcpy(tmp+1,w.s+1);
        if ( TIPO(t2) == _CHAR ) tmp[tmp[0]] = v.c;
        else strcat(tmp+1,v.s+1);
        px += a_memoria(px,tmp,_STRING);
        return;
    }
    d.t = _CONSTANTE|TIPO(d.t);
    _psh(d.t,ri,px-ds);
    px += a_memoria(px,(ADDRESS*)&w,d.t);

}

void _sub() {
    VALOR w;
    ADDRESS t1,t2;

    d = * --sp; t2 = d.t; v = de_memoria(d.s,d.o,d.t);
    d = * --sp; t1 = d.t; w = de_memoria(d.s,d.o,d.t);
    if ( TIPO(t1) == _REAL && TIPO(t2) == _REAL )
        w.d -= v.d;
    if ( TIPO(t1) == _INTEGER && TIPO(t2) == _INTEGER )
        w.i -= v.i;
    if ( TIPO(t1) == _REAL && TIPO(t2) == _INTEGER )
        w.d -= v.i;
    if ( TIPO(t1) == _INTEGER && TIPO(t2) == _REAL )
        { v.d = w.i - v.d; w.d = v.d; d.t = _REAL; }

    d.t = _CONSTANTE|TIPO(d.t);
    _psh(d.t,ri,px-ds);
    px += a_memoria(px,(ADDRESS*)&w,d.t);

}

void _mul() {
    VALOR w;
    ADDRESS t1,t2;

    d = * --sp; t2 = d.t; v = de_memoria(d.s,d.o,d.t);
    d = * --sp; t1 = d.t; w = de_memoria(d.s,d.o,d.t);
    if ( TIPO(t1) == _REAL && TIPO(t2) == _REAL )
        w.d *= v.d;
    if ( TIPO(t1) == _INTEGER && TIPO(t2) == _INTEGER )
        w.i *= v.i;
    if ( TIPO(t1) == _REAL && TIPO(t2) == _INTEGER )
        w.d *= v.i;
    if ( TIPO(t1) == _INTEGER && TIPO(t2) == _REAL )
        { v.d *= w.i; w.d = v.d; d.t = _REAL; }

    d.t = _CONSTANTE|TIPO(d.t);
    _psh(d.t,ri,px-ds);
    px += a_memoria(px,(ADDRESS*)&w,d.t);

}

void _div() {
    VALOR w;
    ADDRESS t1,t2;

    d = * --sp; t2 = d.t; v = de_memoria(d.s,d.o,d.t);
    d = * --sp; t1 = d.t; w = de_memoria(d.s,d.o,d.t);
    if (( v.i == 0 && TIPO(t2) == _INTEGER ) ||
        ( v.d == 0.0 && TIPO(t2) == _REAL ))
        error("Error de ejecucion.Division por cero");
    if ( TIPO(t1) == _REAL && TIPO(t2) == _REAL )
        w.d /= v.d;
    if ( TIPO(t1) == _INTEGER && TIPO(t2) == _INTEGER )
        w.i /= v.i;
    if ( TIPO(t1) == _REAL && TIPO(t2) == _INTEGER )
        w.d /= v.i;
    if ( TIPO(t1) == _INTEGER && TIPO(t2) == _REAL )
        { v.d = w.i/v.d; w.d = v.d; d.t = _REAL; }

    d.t = _CONSTANTE|TIPO(d.t);
    _psh(d.t,ri,px-ds);
    px += a_memoria(px,(ADDRESS*)&w,d.t);

}

void _mod() {
    VALOR w;
    ADDRESS t1,t2;

    d = * --sp; t2 = d.t; v = de_memoria(d.s,d.o,d.t);
    d = * --sp; t1 = d.t; w = de_memoria(d.s,d.o,d.t);
    if ( v.i == 0 ) error("Error de ejecucion.Division por cero");

    w.i %= v.i;
    d.t = _CONSTANTE|TIPO(d.t);
    _psh(d.t,ri,px-ds);
    px += a_memoria(px,(ADDRESS*)&w,d.t);

}

void _anc() {
    VALOR w;
    ADDRESS t1,t2;

    d = * --sp; t2 = d.t; v = de_memoria(d.s,d.o,d.t);
    d = * --sp; t1 = d.t; w = de_memoria(d.s,d.o,d.t);
    w.c &= v.c;
    d.t = _CONSTANTE|TIPO(d.t);
    _psh(d.t,ri,px-ds);
    px += a_memoria(px,(ADDRESS*)&w,d.t);

}

void _orc() {
    VALOR w;

    d = * --sp; v = de_memoria(d.s,d.o,d.t);
    d = * --sp; w = de_memoria(d.s,d.o,d.t);
    w.c |= v.c;
    d.t = _CONSTANTE|TIPO(d.t);
    _psh(d.t,ri,px-ds);
    px += a_memoria(px,(ADDRESS*)&w,d.t);

}

void _noc() {
    ADDRESS *des;

    d = sp[-1];
    v = de_memoria(d.s,d.o,d.t);
    v.c = !v.c;

    des = rs[ri-d.s]->datos+d.o;
    a_memoria(des,(ADDRESS*)&v,d.t);

}

void _ltc() {
    VALOR w;
    ADDRESS t1,t2;

    d = * --sp; t2 = d.t; v = de_memoria(d.s,d.o,d.t);
    d = * --sp; t1 = d.t; w = de_memoria(d.s,d.o,d.t);
    if (TIPO(t1)==_REAL && TIPO(t2)==_REAL)        w.i = w.d < v.d;
    if (TIPO(t1)==_INTEGER && TIPO(t2)==_INTEGER)  w.i = w.i < v.i;
    if (TIPO(t1)==_REAL && TIPO(t2)==_INTEGER)     w.i = w.d < v.i;
    if (TIPO(t1)==_INTEGER && TIPO(t2)==_REAL)     w.i = w.i < v.d;
    if (TIPO(t1)==_CHAR && TIPO(t2)==_CHAR)       w.i = w.c < v.c;
    if (TIPO(t1)==_STRING && TIPO(t2)==_STRING)   w.i = strcmp(w.s,v.s) < 0;
    d.t = _CONSTANTE|_INTEGER;
    _psh(d.t,ri,px-ds);
    px += a_memoria(px,(ADDRESS*)&w,d.t);
}

void _lec() {
    VALOR w;
    ADDRESS t1,t2;

    d = * --sp; t2 = d.t; v = de_memoria(d.s,d.o,d.t);
    d = * --sp; t1 = d.t; w = de_memoria(d.s,d.o,d.t);
    if (TIPO(t1)==_REAL && TIPO(t2)==_REAL)        w.i = w.d <= v.d;
    if (TIPO(t1)==_INTEGER && TIPO(t2)==_INTEGER)  w.i = w.i <= v.i;
    if (TIPO(t1)==_REAL && TIPO(t2)==_INTEGER)     w.i = w.d <= v.i;
    if (TIPO(t1)==_INTEGER && TIPO(t2)==_REAL)     w.i = w.i <= v.d;
    if (TIPO(t1)==_CHAR && TIPO(t2)==_CHAR)       w.i = w.c <= v.c;
    if (TIPO(t1)==_STRING && TIPO(t2)==_STRING)   w.i = strcmp(w.s,v.s) <= 0;
    d.t = _CONSTANTE|_INTEGER;
    _psh(d.t,ri,px-ds);
    px += a_memoria(px,(ADDRESS*)&w,d.t);
}

void _gtc() {
    VALOR w;
    ADDRESS t1,t2;

    d = * --sp; t2 = d.t; v = de_memoria(d.s,d.o,d.t);
    d = * --sp; t1 = d.t; w = de_memoria(d.s,d.o,d.t);
    if (TIPO(t1)==_REAL && TIPO(t2)==_REAL)        w.i = w.d > v.d;
    if (TIPO(t1)==_INTEGER && TIPO(t2)==_INTEGER)  w.i = w.i > v.i;
    if (TIPO(t1)==_REAL && TIPO(t2)==_INTEGER)     w.i = w.d > v.i;
    if (TIPO(t1)==_INTEGER && TIPO(t2)==_REAL)     w.i = w.i > v.d;
    if (TIPO(t1)==_CHAR && TIPO(t2)==_CHAR)       w.i = w.c > v.c;
    if (TIPO(t1)==_STRING && TIPO(t2)==_STRING)   w.i = strcmp(w.s,v.s) > 0;
    d.t = _CONSTANTE|_INTEGER;
    _psh(d.t,ri,px-ds);
    px += a_memoria(px,(ADDRESS*)&w,d.t);
}

void _gec() {
    VALOR w;
    ADDRESS t1,t2;

    d = * --sp; t2 = d.t; v = de_memoria(d.s,d.o,d.t);
    d = * --sp; t1 = d.t; w = de_memoria(d.s,d.o,d.t);
    if (TIPO(t1)==_REAL && TIPO(t2)==_REAL)        w.i = w.d >= v.d;
    if (TIPO(t1)==_INTEGER && TIPO(t2)==_INTEGER)  w.i = w.i >= v.i;
    if (TIPO(t1)==_REAL && TIPO(t2)==_INTEGER)     w.i = w.d >= v.i;
    if (TIPO(t1)==_INTEGER && TIPO(t2)==_REAL)     w.i = w.i >= v.d;
    if (TIPO(t1)==_CHAR && TIPO(t2)==_CHAR)       w.i = w.c >= v.c;
    if (TIPO(t1)==_STRING && TIPO(t2)==_STRING)   w.i = strcmp(w.s,v.s) >= 0;
    d.t = _CONSTANTE|_INTEGER;
    _psh(d.t,ri,px-ds);
    px += a_memoria(px,(ADDRESS*)&w,d.t);
}

void _nec() {
    VALOR w;
    ADDRESS t1,t2;

    d = * --sp; t2 = d.t; v = de_memoria(d.s,d.o,d.t);
    d = * --sp; t1 = d.t; w = de_memoria(d.s,d.o,d.t);
    if (TIPO(t1)==_REAL && TIPO(t2)==_REAL)        w.i = w.d != v.d;
    if (TIPO(t1)==_INTEGER && TIPO(t2)==_INTEGER)  w.i = w.i != v.i;
    if (TIPO(t1)==_REAL && TIPO(t2)==_INTEGER)     w.i = w.d != v.i;
    if (TIPO(t1)==_INTEGER && TIPO(t2)==_REAL)     w.i = w.i != v.d;
    if (TIPO(t1)==_CHAR && TIPO(t2)==_CHAR)       w.i = w.c != v.c;
    if (TIPO(t1)==_STRING && TIPO(t2)==_STRING)   w.i = strcmp(w.s,v.s) != 0;
    d.t = _CONSTANTE|_INTEGER;
    _psh(d.t,ri,px-ds);
    px += a_memoria(px,(ADDRESS*)&w,d.t);
}

void _eqc() {
    VALOR w;
    ADDRESS t1,t2;

    d = * --sp; t2 = d.t; v = de_memoria(d.s,d.o,d.t);
    d = * --sp; t1 = d.t; w = de_memoria(d.s,d.o,d.t);
    if (TIPO(t1)==_REAL && TIPO(t2)==_REAL)        w.i = w.d == v.d;
    if (TIPO(t1)==_INTEGER && TIPO(t2)==_INTEGER)  w.i = w.i == v.i;
    if (TIPO(t1)==_REAL && TIPO(t2)==_INTEGER)     w.i = w.d == v.i;
    if (TIPO(t1)==_INTEGER && TIPO(t2)==_REAL)     w.i = w.i == v.d;
    if (TIPO(t1)==_CHAR && TIPO(t2)==_CHAR)       w.i = w.c == v.c;
    if (TIPO(t1)==_STRING && TIPO(t2)==_STRING)   w.i = strcmp(w.s,v.s) == 0;
    d.t = _CONSTANTE|_INTEGER;
    _psh(d.t,ri,px-ds);
    px += a_memoria(px,(ADDRESS*)&w,d.t);
}

void _sav() {    /* Coloca en lugar seguro el retorno de una funcion */
    DATO x;

    x.o = px-ds;
    x.s = ri;
    d = * --sp;
    px += copia(x,d,0);
    if ( rs[ri]->ptr == NULL ) -- ri ;
    _psh(_CONSTANTE|0x0f&d.t,x.s,x.o);
    if ( rs[ri]->ptr == NULL ) ++ ri ;
}

void _asg() {
    DATO x;
    
    d = * --sp;
    x = * --sp;
    copia(x,d,0);
    reset_px;
}

void _inc() {
    ADDRESS *des;

    d = * --sp;
    v = de_memoria(d.s,d.o,d.t);
    v.i ++;
    des = rs[ri-d.s]->datos+d.o;
    a_memoria(des,(ADDRESS*)&v,d.t);
}

void _dec() {
    ADDRESS *des;

    d = * --sp;
    v = de_memoria(d.s,d.o,d.t);
    v.i --;
    des = rs[ri-d.s]->datos+d.o;
    a_memoria(des,(ADDRESS*)&v,d.t);
}

void _cpy( ADDRESS n ) {
    DATO x;
    int i;

    d.s = 0; d.o = 0;
    for ( i=0; i < n; i ++ ) {
        x = sp[i-n];
        if ( rs[ri]->ptr != NULL && !x.s ) d.o += lleva(d,x,i-n);
        else {
            x.s = ISCONST(x.t)?ri:x.s+1;
            d.o += copia(d,x,i-n);
        }
    }
}

void _rst( ADDRESS *n ) {
    DATO x;
    int i;
    ADDRESS j,k,f=0;

    j = *n++;
    k = *n++;
    d.s = 0; d.o = 0; i=0;
    while ( *n != 0xFF ) {
        for ( ; i < *n; i ++ ) {
            d.o += objsize(sp[-j-k+i],-j-k+i);
        }
        x = sp[-j-k+i];
        d.t = x.t;
        if ( rs[ri]->ptr != NULL && !x.s ) trae(x,d,-j-k+i);
        else if ( ISVAR(x.t) ) {
            x.s = x.s+1;
            copia(x,d,-j-k+i);
        } n ++;
    }
    reset_px;
}

/*  Evalua la direccion de un elemento de arreglo */

void _eva( ADDRESS *c ) {
    int i;
    int off=0,li,ls,k=0,n=c[2];
    unsigned s = IVAL(c),
             mayor = IVAL(c+3);

    for ( i=0; i < n; i ++ ) {
        d = sp[i-n];
        v = de_memoria(d.s,d.o,d.t);
        li = IVAL(c+i*4+5);
        ls = IVAL(c+i*4+7);
        if ( !(v.i >= li && v.i <= ls) )
            error("Ejecucion. Subscriptor fuera de rango");
        mayor /= ls;
        k += mayor * (v.i-1);
    }
    off += s * k;
    sp[-n-1].o += off;
    _uns(n);
}

ADDRESS *_ifc( ADDRESS *c ) {
    int bool=0;

    d = * --sp;
    v = de_memoria(d.s,d.o,d.t);
    reset_px;           /* Corrige Heap */

    if ( TIPO(d.t) == _INTEGER ) bool = v.i == 1;
    else bool = v.c == 1;

    if ( bool ) return(c+2);
    else return(cs+IVAL(c));

}

ADDRESS *_cac( ADDRESS *c ) {
    int i = *c+1,k,bool;

    d = sp[-i];
    v = de_memoria(d.s,d.o,d.t);
    if ( TIPO(d.t) == _INTEGER ) k = v.i;
    else k = v.c;

    for ( i = *c; i ; i -- ) {
        d = sp[-i];
        v = de_memoria(d.s,d.o,d.t);

        if ( TIPO(d.t) == _INTEGER ) bool = v.i == k;
        else bool = v.c == k;
        if ( bool ) {
            _uns(*c);
            return(c+3);
        }
    } _uns(*c);
    return(cs+IVAL(c+1));
}

void _uns( ADDRESS n ) {
    sp -= n;
}

unsigned copia( DATO hasta, DATO desde, int arr ) {
    unsigned size, o1=desde.o,o2=hasta.o;
    ADDRESS      s1=desde.s,s2=hasta.s;
    ADDRESS      *org = rs[ri-s1]->datos+o1,
                 *des = rs[ri-s2]->datos+o2;


    switch ( desde.t & 0x07 ) {
        case  _CHAR:
        case  _BOOLEAN:size = sizeof(char);  break;
        case  _INTEGER:size = INT_WIDTH;   break;
        case  _REAL:   size = sizeof(double);break;
        case  _STRING: size = org[0]+2;       break;
    }

    if ( (desde.t&_ARREGLO)==_ARREGLO && (desde.t&_STRING)==_STRING )
        size *= szs[iz+arr];

    memcpy(des,org,size);
    return(size);
}

unsigned lleva( DATO hasta, DATO desde, int arr ) {
    unsigned size, o1=desde.o,o2=hasta.o;
    ADDRESS      s1=desde.s,s2=hasta.s;
    ADDRESS      *org = rs[ri]->ptr->datos+o1,
                 *des = rs[ri-s2]->datos+o2;

    switch ( desde.t & 0x0f ) {
        case  _CHAR:
        case  _BOOLEAN:size = sizeof(char);  break;
        case  _INTEGER:size = INT_WIDTH;   break;
        case  _REAL:   size = sizeof(double);break;
        case  _STRING: size = org[0]+2;       break;
    }

    if ( (desde.t&_ARREGLO)==_ARREGLO && (desde.t&_STRING)==_STRING )
        size *= szs[iz+arr];

    memcpy(des,org,size);
    return(size);
}

unsigned trae( DATO hasta, DATO desde, int arr ) {
    unsigned size, o1=desde.o,o2=hasta.o;
    ADDRESS      s1=desde.s,s2=hasta.s;
    ADDRESS      *org = rs[ri-s1]->datos+o1,
                 *des = rs[ri]->ptr->datos+o2;

    switch ( desde.t & 0x0f ) {
        case  _CHAR:
        case  _BOOLEAN:size = sizeof(char);     break;
        case  _INTEGER:size = INT_WIDTH;        break;
        case  _REAL:   size = sizeof(double);   break;
        case  _STRING: size = org[0]+2;         break;
    }

    if ( (desde.t&_ARREGLO)==_ARREGLO && (desde.t&_STRING)==_STRING )
        size *= szs[iz+arr];

    memcpy(des,org,size);
    return(size);
}

void _stk( ADDRESS arg, ADDRESS cod ) {
    DATO x,y;

    if ( cod ) x = * --sp;
    _uns(arg);
    if ( cod ) *sp ++ = x;

}

void _cll( ADDRESS *codigo ) {
    int mfp;

    while ( *codigo != RTN && codigo < ip )
        switch( *codigo ) {
            case  ASG:  _asg();
                        codigo ++;
                        break;
            case  ADD:  _add();
                        codigo ++;
                        break;
            case  SUB:  _sub();
                        codigo ++;
                        break;
            case  MUL:  _mul();
                        codigo ++;
                        break;
            case  DIV:  _div();
                        codigo ++;
                        break;
            case  MOD:  _mod();
                        codigo ++;
                        break;
            case  ANC:  _anc();
                        codigo ++;
                        break;
            case  ORC:  _orc();
                        codigo ++;
                        break;
            case  NOC:  _noc();
                        codigo ++;
                        break;
            case  LTC:  _ltc();
                        codigo ++;
                        break;
            case  LEC:  _lec();
                        codigo ++;
                        break;
            case  GTC:  _gtc();
                        codigo ++;
                        break;
            case  GEC:  _gec();
                        codigo ++;
                        break;
            case  EQC:  _eqc();
                        codigo ++;
                        break;
            case  NEC:  _nec();
                        codigo ++;
                        break;
            case  ALC:  _alc(IVAL(codigo+1),IVAL(codigo+3));
                        codigo += 5;
                        break;
            case  FRE:  _fre();
                        codigo ++;
                        break;
            case  PSH:  _psh(codigo[1],codigo[2],IVAL(codigo+3));
                        codigo += 5;
                        break;
            case  CPY:  _cpy(codigo[1]);
                        codigo += 2;
                        break;
            case  UNS:  _uns(codigo[1]);
                        codigo += 2;
                        break;
            case  RST:  _rst(codigo+1);
                        codigo += 3;
                        while ( *codigo++ != 0xFF );
                        break;
            case  SAV:  _sav();
                        codigo ++;
                        break;
            case  CLL:  _cll(cs+IVAL(codigo+1));                        
                        codigo += 3;
                        break;
            case  WRT:  _wrt();
                        codigo ++;
                        break;
            case  REA:  _rea();
                        codigo ++;
                        break;
            case  EVA:  _eva(codigo+1);
                        codigo += codigo[3]*4+6;
                        break;
            case  FAR:  szs[iz++] = IVAL(codigo+4);
                        codigo += 6;
                        break;
            case  IFC:  codigo = _ifc(codigo+1);
                        break;
            case  CAC:  codigo = _cac(codigo+1);
                        break;
            case  JMP:  codigo = cs + IVAL(codigo+1);
                        break;
            case  INC:  _inc();
                        codigo ++;
                        break;
            case  DEC:  _dec();
                        codigo ++;
                        break;
            case  STK:  _stk(codigo[1],codigo[2]);
                        codigo += 3;
                        break;
        }
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

/* Tener cuidado con el tamaño de los objetos */
int objsize( DATO o, int a ) {
    int size;
    ADDRESS *org;

    switch ( o.t & 0x07 ) {
        case  _CHAR:
        case  _BOOLEAN:size = sizeof(char);     break;
        case  _INTEGER:size = INT_WIDTH;        break;
        case  _REAL:   size = sizeof(double);   break;
        default:
             org = rs[ri-o.s]->datos+o.o;
             switch ( o.t ) {
                 case  _STRING: size = org[0]+2;       break;
             } break;
    }

    if ( (o.t&_ARREGLO)==_ARREGLO && (o.t&_STRING)==_STRING) size *= szs[iz+a];

    return(size);
}

/***************************************
 * 
 * Inicia Analisis sintactico
 * 
 */

int si;
int pi;     /*  Control de anidamiento en procs/funcs */

char *p,_bfr[256];
int  indecl,inargs,inproc,inasgn,inrang,rtn;     /*debug=0; */
int  ps,pre,nivel,offset,array,idim;
ADDRESS str_lon,args[16],refs[16],dim[32];

void analisis() {
    indecl=
    inargs=
    inproc=
    inasgn=
    inrang=
    nivel =
    offset=
    array =0;
    si    =
    pi    =
    linea = 1;

    pre = scanner();    
    declaraciones();    
    simbolos[0].tbloque = offset;
    proc_func(0);       
    programa();
}

void constantes() {
    unsigned n,o;

    if ( pre == CONST ) {
        indecl = 1;
        match(pre);
        while ( n=si-1, pre == _UNDEF ) {
            match(pre);
            o = dx-ds;
            match(EQ);
            if ( (d.t&0xF0) == _CONSTANTE ) match(pre);
            else error("El valor derecho no es una constante");
            simbolos[n].tipo = d.t;
            simbolos[n].nivel = 0;
            simbolos[n].offset = o;
            match(';');
        }
        indecl = 0;
    }
}

void declaraciones() {
    int ax,bx,cx;

    constantes();
    if ( pre == VAR ) {
        indecl = 1;
        match(VAR);
        while ( pre == _UNDEF ) {
            ax = si - 1;
            idim = 5;
            idlist();
            if ( pre == ':' ) {
                match(':');
                memset(dim,0,32);
                cx = tipo(&bx);
                tipo_direccion(ax,bx,cx);
                match(';');
            } else error("Dos Puntos Esperados");
        }
        indecl = 0;
    }
}

ADDRESS idlist() {
    ADDRESS i=0;

    do {
        match(_UNDEF); i++;
        if ( pre == ',' )
            match(',');
        else break;
    } while ( pre == _UNDEF );
    return(i);
}

int tipo( int *qt ) {
    uint16_t t,u;

    switch( pre ) {
        case  _CHAR:
        case  _BOOLEAN:
             t = sizeof(char);
             memcpy(dim,&t,INT_WIDTH);
             *qt = pre;
             match(pre);
             break;
        case  _INTEGER:
             t = INT_WIDTH;
             memcpy(dim,&t,INT_WIDTH);
             *qt = pre;
             match(pre);
             break;
        case  _REAL:
             t = sizeof(double);
             memcpy(dim,&t,INT_WIDTH);
             *qt = pre;
             match(pre);
             break;
        case  _STRING:
             *qt = pre;
             match(pre);
             match('[');
             t = str_lon = v.i+2;
             memcpy(dim,&t,INT_WIDTH);
             match(NUM);
             match(']');
             break;
        case _ARRAY:
             array = 1;
             match(pre);
             match('[');
             u = rangolist();
             match(']');
             array = 0;
             match(OF);
             t = u * tipo(qt);
             /*printf("Tipo:%d tamaño: %d size: %d\n",*qt,u,t);*/
             if ( *qt == _CHAR ) *qt = _STRING;
             *qt |= _ARREGLO;
             break;
        default:     error("Tipo Desconocido"); break;
    }
    return(t);
}

int rangolist() {
    int i=0,j,s,m,n;

    inrang = 1;
    while ( 1 ) {
        /*printf("Copiando en memoria[%d]: %d\n",idim,v.i);*/
        if ( pre == NUM || pre == CONST )
            memcpy(dim+idim,&v.i,INT_WIDTH);
        else error("Valor de rango no permitido");
        idim += INT_WIDTH;
        match(pre);
        match(PP);
        /*printf("Copiando en memoria[%d]: %d\n",idim,v.i);*/
        if ( pre == NUM || pre == CONST )
            memcpy(dim+idim,&v.i,INT_WIDTH);
        else error("Valor de rango no permitido");
        idim += INT_WIDTH;
        match(pre);
        i ++;
        if ( pre == ',' ) match(pre);
        else break;
    }
    inrang = 0;
    /*debugDim(dim);*/
    for ( s=1,j=0; j < i; j ++ ) {
        memcpy(&m,dim+(j+dim[2])*4+7,INT_WIDTH);
        memcpy(&n,dim+(j+dim[2])*4+5,INT_WIDTH);
        /*printf("Limites [%d,%d]\n",n,m);*/
        s *= m-n+1;
    }
    memcpy(dim+3,&s,INT_WIDTH);

    dim[2] = (idim-5)/4;
    /*printf("Rangolist en %d a %d\n",1,s);
    debugDim(dim);*/

    return(s);

}

void tipo_direccion( int i, int f, int s ) {

    while ( i < si ) {
        simbolos[i].tipo = _VARIABLE | f;
        /*printf("Calculando tipo de direccion para %#X\n",f&0x0F);*/
        switch ( f&0x0F ) {
            case  _ARREGLO|_BOOLEAN:
            case  _ARREGLO|_INTEGER:
            case  _ARREGLO|_STRING:
            case  _ARREGLO|_REAL:
                memcpy(simbolos[i].dims,dim,dim[2]*4+5);
                break;
            case  _STRING:
                 *dx = str_lon;
                 break;
        }
        simbolos[i].tbloque = s;
        simbolos[i].offset = offset;
        offset += simbolos[i++].tbloque;
    }
}

void proc_func( int actual ) {

    while ( pre == PROCEDURE || pre == FUNCTION )
        if ( pre == PROCEDURE ) procedures(actual);
        else if ( pre == FUNCTION ) functions(actual);
}

void procedures( int actual ) {
    int ax,f=0,xs;

    while ( pre == PROCEDURE ) {    /* Instala el codigo procedure */
        ax = si;
        inproc = 1;
        match(PROCEDURE);
        inproc = 0;

        nivel ++;
        xs = ps+1;

        if ( pre != _UNDEF )
            error("El nombre del procemiento ya es utilizado");
        match(_UNDEF);

        offset = 0;
        arglist();

        match(';');
        simbolos[ax].tipo = _PROC|_PROC;
        memcpy(simbolos[ax].args,args,16);
        declaraciones();
        simbolos[ax].tbloque = offset;

        proc_func(pi++);          /* Agrego recursividad */
        simbolos[ax].offset = ip-cs;
        bloque();
        poplocal(xs);
        nivel --;
        *ip++ = RTN;
        match(';');
    }
}

void functions( int actual ) {
    int ax,f=0,t,u,xs;

    while ( pre == FUNCTION ) {     /* Instala el codigo procedure */
        ax = si;

        inproc = 1;
        match(FUNCTION);
        inproc = 0;

        nivel ++;
        xs = ps+1;
        if ( pre != _UNDEF )
            error("El nombre de la funcion ya es utilizado");
        match(_UNDEF);

        offset = 0;
        arglist();
        if ( pre != ':' )
            error("La funcion debe tener un tipo de retorno");
        match(':');
        u = tipo(&t);
        match(';');
        simbolos[ax].tipo = _FUNC|t;
        memcpy(simbolos[ax].args,args,16);
        declaraciones();
        simbolos[ax].tbloque = offset+u;
        proc_func(pi++);          /* Agrego recursividad */
        simbolos[ax].offset = ip-cs;
        rtn = 0;
        bloque();
        if ( !rtn ) error("No existe expresion de retorno para funcion");
        poplocal(xs);
        nivel --;
        match(';');
    }
}

void arglist() {
    int ref=0,i=1,n,ax,bx,cx;

    memset(args,0,16);
    if ( pre == '(' ) {
        inargs = 1;
        match('(');
        while ( 1 ) {
            if ( pre == VAR ) {
                ref = 1;
                match(VAR);
            }
            ax = si - 1;
            /* 
                Reserva los primeros 5 bytes
                [0][1][2][3][4]
                1 byte el numero de dimensiones en posicion [2]
            */
            idim = 5;

            n = idlist();
            args[0] += n;
            if ( pre == ':' ) {
                match(':');
                memset(dim,0,16);
                cx = tipo(&bx);
                bx |= (ref==1?_REFE:_UNDEF);
                while ( n-- ) args[i++] = bx;
                tipo_direccion(ax,bx,cx);
                ref = 0;
                if ( pre == ';' ) match(';');
                else break;
            } else error("Dos Puntos Esperados");
        } inargs = 0;
        match(')');
    }
}

void programa() {
    uint16_t  m;

    pc = ip;
    if ( pre != BEGIN )
        error("BEGIN esperado.");

    *ip ++ = ALC;
    m = pc-cs;
    memcpy(ip,&m,INT_WIDTH);                      ip += 2;
    memcpy(ip,&simbolos[0].tbloque,INT_WIDTH);    ip += 2;

    match(BEGIN);
    sentenlist();
    *ip ++ = FRE;
    if ( pre != END )
        error("END esperado.");
    match(END);
    match('.');
}

void sentencia() {
    switch( pre ) {
        case  IF:       ifcode();    break;
        case  FOR:      forcode();   break;
        case  WHILE:    whilecode(); break;
        case  REPEAT:   repeatcode();break;
        case  CASE:     casecode();  break;
        case  _PROC:    proccode();  break;
        case  _FUNC:    funccode();  break;
        case  BEGIN:    bloque();    break;
        case  WRITE:
        case  WRITELN:  writecode(); break;
        case  READ:
        case  READLN:   readcode();  break;
        case  ID:
        case  _ARRAY:    idcode();    break;
        default:                     break;
    }
}

void sentenlist() {

    while ( 1 ) {
        sentencia();
        if ( pre != ';' ) break;
        else match(pre);
    }
}

void ifcode() {
    uint16_t _off;
    ADDRESS  *nxt,*els;

    match(IF);
    expresion();        /*  Expresion Logica            */
    chkarg(_BOOLEAN);
    match(THEN);
    *ip++ = IFC;        /*  Codigo IF                   */
    nxt = ip;           /*  Salto siguiente instruccion */
    ip += 2;
    bloque();
    if ( pre == ELSE ) {
        *ip++ = JMP;    /*  Si condicion, entonces no codigo */
        els = ip;       /*  ELSE y debe saltar al final del bloque */
        ip += 2;
        _off = ip-cs;   memcpy(nxt,&_off,sizeof(uint16_t));
        match(ELSE);
        bloque();
        nxt = els;
    }
    _off = ip-cs;       memcpy(nxt,&_off,sizeof(uint16_t));
}

void whilecode() {
    ADDRESS *nxt;
    uint16_t _off;

    match(WHILE);
    _off = ip-cs;       /*  Regreso al ciclo  */

    expresion();        /*  Expresion Logica  */
    chkarg(_BOOLEAN);
    match(DO);

    *ip++ = IFC;
    nxt = ip;
    ip += 2;

    bloque();

    *ip++ = JMP;        /*  Crea instruccion de salto incondicional */
    memcpy(ip,&_off,INT_WIDTH);
    ip += 2;

    _off = ip-cs;       /*  Crea posicion a donde saltar si falso */
    memcpy(nxt,&_off,INT_WIDTH);

}

void forcode() {
    int t,x;
    uint16_t _off;
    ADDRESS *nxt;

    match(FOR);
    t = ps;             /*  Guarda la direccion de la variable lider */
    idcode();
    _off = ip-cs;       /*  Punto de retorno al ciclo */

    if ( pre == TO ) {
        x = 1;
        match(TO);
    } else {
        x = 0;
        match(DOWNTO);
    }

    expresion(); d.t = simbolos[t].tipo;
    ensambla(nivel-simbolos[t].nivel,simbolos[t].offset);

    *ip++ = x?GEC:LEC;
    *ip++ = IFC;
    nxt = ip;           /*  Posicion de Salto si falso */
    ip += 2;

    match(DO);
    bloque();

    d.t = simbolos[t].tipo; /* Incrementa la variable en 1 */
    ensambla(nivel-simbolos[t].nivel,simbolos[t].offset);
    *ip++ = x?INC:DEC;

    *ip++ = JMP;        /*  Crea instruccion de salto incondicional */
    memcpy(ip,&_off,INT_WIDTH); ip += 2;

    _off = ip-cs;       /*  Crea posicion a donde saltar si falso */
    memcpy(nxt,&_off,INT_WIDTH);

}

void repeatcode() {
    uint16_t _off;
    ADDRESS *nxt,*els;

    match(REPEAT);
    _off = ip-cs;       /*  Regreso al ciclo  */
    sentenlist();
    match(UNTIL);
    expresion();        /*  Expresion Logica  */
    chkarg(_BOOLEAN);
    *ip++ = IFC;        /*  Codigo IF         */
    memcpy(ip,&_off,INT_WIDTH); ip += 2;

}

void casecode() {
    int t,i,j=0;
    unsigned _off;
    ADDRESS *nxt,*jmp[256];

    match(CASE);
    expresion();
    t = chkarg(_LABEL);
    /*chkarg(_LABEL);*/
    match(OF);
    do {
        i = 0;
        while ( 1 ) {
            expresion();
            chkarg(_LABEL);
            if ( chkarg(_LABEL) != t )
                error("Incongruencia de tipos selector/seleccion");
            i++;
            if ( pre == ',' ) match(pre);
            else break;
        }
        match(':');

        *ip++ = CAC;
        *ip++ = i&0xFF;
        nxt = ip;
        ip += 2;
        bloque();
        *ip++ = JMP;
        jmp[j++] = ip;
        ip += 2;
        _off = ip-cs;
        memcpy(nxt,&_off,sizeof(int));

        if ( pre == ';' ) match(';');
        else break;
    } while ( pre != END && pre != ELSE );
    if ( pre == ELSE ) {
        match(ELSE);
        bloque();
        *ip++ = JMP;
        jmp[j++] = ip;
        ip += 2;
        if ( pre == ';' ) match(';');
    } match(END);

    *ip++ = UNS; *ip++ = 1; _off = ip-cs;
    for ( i=0; i < j; i ++ ) memcpy(jmp[i],&_off,sizeof(int));

}

void proccode() {
    int n=0,m=ps,r=0,f=0;

    match(_PROC);
    memset(refs,0xFF,16);
    memcpy(args,simbolos[m].args,16);

    /*  checa tipo y numero de argumentos   */

    if ( pre == '(' ) {
        inasgn = 1;
        match('(');
        while ( 1 ) {
            expresion(); n++;
            chkarg(args[n]);
            if ( (args[n]&0xf0) == _REFE ) {
                r = 1;
                refs[f++] = n-1;
            }
            if ( pre == ',' ) match(',');
            else break;
        }
        match(')');
        inasgn = 0;
    }
    if ( n != args[0] ) error("Número incorrecto de argumentos");
    retcode(m,r);
}

void funccode() {
    int n=0,m=ps,r=0,f=0;

    /* Pretende retornar valor */

    match(_FUNC);
    if ( pre == AS ) {
        if ( simbolos[m].nivel+1 == nivel ) {
            rtn = 1;
            match(AS);
            inasgn = 1;
            expresion();
            inasgn = 0;
            chkarg(simbolos[m].tipo&0xf);
            *ip ++ = SAV;
            *ip ++ = RTN;
        } else error("Retorno de funcion desconocida");
        return;
    }

    if ( inasgn ) {
        memset(refs,0xFF,16);
        memcpy(args,simbolos[m].args,16);

        /*  checa tipo y numero de argumentos   */

        ts[ti++] = simbolos[m].tipo;
        if ( pre == '(' ) {
            inasgn = 1;
            match('(');
            while ( pre != ')' ) {
                expresion(); n++;
                chkarg(args[n]);
                if ( (args[n]&0xf0) == _REFE ) {
                    r = 1;
                    refs[f++] = n-1;
                }
                if ( pre == ',' ) match(',');
                else break;
            }
            match(')');
            inasgn = 0;
        }
        if ( n != args[0] ) error("Número incorrecto de argumentos");
        retcode(m,r);
    } else error("Se requiere un Lvalor para asignar funcion");
}

void retcode(int s, int r) {
    int i=0;

    *ip++ = ALC;
    memcpy(ip,&simbolos[s].offset,sizeof(int));  ip += 2;
    memcpy(ip,&simbolos[s].tbloque,sizeof(int)); ip += 2;

    *ip++ = CPY;
    *ip++ = simbolos[s].args[0];

    *ip++ = CLL;
    memcpy(ip,&simbolos[s].offset,sizeof(int));  ip += 2;

    if (r) {
        *ip++ = RST;
        *ip++ = simbolos[s].args[0];
        *ip++ = ((simbolos[s].tipo&0xf0)==0x60)?1:0;
        do {
            *ip++ = refs[i];
            if ( refs[i] == 0xFF ) break;
            else i++;
        } while ( 1 );
    }
    *ip++ = STK;
    *ip++ = simbolos[s].args[0];
    *ip++ = ((simbolos[s].tipo&0xf0)==0x60)?1:0;
    *ip++ = FRE;
}

void writecode() {
    int t=pre;

    inasgn = 1;
    match(pre);
    if ( pre == '(' ) {
        match('(');
        do {
            expresion();
            *ip ++ = WRT;
            if ( pre == ',' )
                match(',');
        } while ( pre != ')' );
        match(')');
    }
    inasgn = 0;
    if ( t == WRITELN ) {
        d.t = _CONSTANTE|_CHAR;
        ensambla(CONST_SEG,0);
        *ip ++ = WRT;
    }
}

void readcode() {
    int t=pre;

    match(pre);
    match('(');
    do {
        idcode();
        *ip ++ = REA;
        if ( pre == ',' )
            match(',');
    } while ( pre != ')' );
    match(')');
    if ( t == READLN ) {
        d.t = _CONSTANTE|_CHAR;
        ensambla(CONST_SEG,0);
        *ip ++ = WRT;
    }
}

void bloque() {

    if ( pre == BEGIN ) {
        match(BEGIN);
        sentenlist();
        match(END);
    } else sentencia();

}

void expresion() {

    sublogica();
    logica_or();

}

void sublogica() {

    elemento();
    logica_and();

}

void elemento() {

    equivale();
    comparacion();

}

void equivale() {

    operacion();
    relacional();
}

void operacion() {

    termino();
    adiciona();
}

void termino() {

    factor();
    multiplica();
}

void final() {

    factor();
    negacion();
}

void comparacion() {

    while ( 1 ) {
        switch ( pre ) {
            case  EQ:
                 match(pre);
                 equivale();
                 chktipo(EQC);
                 *ip++ = EQC;
                 continue;
            case  NE:
                 match(pre);
                 equivale();
                 chktipo(NEC);
                 *ip++ = NEC;
                 continue;
            default:
                 return;
        }
    }
}

void relacional() {

    while ( 1 ) {
        switch ( pre ) {
            case  GT:
                 match(pre);    
                 operacion();   
                 chktipo(GTC);  
                 *ip++ = GTC;
                 continue;
            case  GE:
                 match(pre);
                 operacion();
                 chktipo(GEC);
                 *ip++ = GEC;
                 continue;
            case  LT:
                 match(pre);
                 operacion();
                 chktipo(LTC);
                 *ip++ = LTC;
                 continue;
            case  LE:
                 match(pre);
                 operacion();
                 chktipo(LEC);
                 *ip++ = LEC;
                 continue;
            default:
                 return;
        }
    }
}

void logica_and() {

    while ( 1 ) {
        switch ( pre ) {
            case  AND:
                 match(pre);
                 elemento();
                 chktipo(ANC);
                 *ip++ = ANC;
                 continue;
            default:
                 return;
        }
    }
}

void logica_or() {

    while ( 1 ) {
        switch ( pre ) {
            case  OR:
                 match(pre);
                 sublogica();
                 chktipo(ORC);
                 *ip++ = ORC;
                 continue;
            default:
                 return;
        }
    }
}

void adiciona() {

    while ( 1 ) {
        switch ( pre ) {
            case  '+':
                 match(pre);
                 termino();
                 chktipo(ADD);
                 *ip ++ = ADD;
                 continue;
            case  '-':
                 match(pre);
                 termino();
                 chktipo(SUB);
                 *ip ++ = SUB;
                 continue;
            default:
                 return;
        }
    }
}

void multiplica() {

    while ( 1 ) {
        switch ( pre ) {
            case  '*':
                 match(pre);
                 factor();
                 chktipo(MUL);
                 *ip ++ = MUL;
                 continue;
            case  '%':
            case  MDL:
                 match(pre);
                 factor();
                 chktipo(MOD);
                 *ip ++ = MOD;
                 continue;
            case  '/':
                 match(pre);
                 factor();
                 chktipo(DIV);
                 *ip ++ = DIV;
                 continue;
            default:
                 return;
        }
    }
}

void factor() {

    switch ( pre ) {
        case  '-':
             match('-');
             ip[1] = _INTEGER;
             ip += 4;
             expresion();
             chktipo(MEU);
             *ip++ = MEU;
             break;
        case  NOT:
             match(NOT);
             factor();
             chkarg(NOC);
             *ip++ = NOC;
             break;
        case  _TRUE:
             match(_TRUE);
             d.t = _CONSTANTE|_BOOLEAN;
             ensambla(CONST_SEG,1);
             break;
        case  _FALSE:
             match(_FALSE);
             d.t = _CONSTANTE|_BOOLEAN;
             ensambla(CONST_SEG,2);
             break;
        case  _PI:
             match(_PI);
             d.t = _CONSTANTE|_REAL;
             ensambla(CONST_SEG,3);
             break;
        case  '(':
             match('(');
             expresion();
             match(')');
             break;
        case  ID:
        case _ARRAY:
             idcode();
             break;
        case  _FUNC:
             funccode();
             break;
        case  CONST:
             ensambla(CONST_SEG,simbolos[ps].offset);
             match(CONST);
             break;
        case CAR:
        case NUM:
        case _STRING:
             match(pre);
             break;
    }
}

void negacion() {

    switch ( pre ) {
        case  '-':
             match('-');
             ip[1] = _INTEGER;
             ip += 4;
             expresion();
             chktipo(MEU);
             *ip++ = MEU;
             break;
        case  NOT:
             match(NOT);
             factor();
             chkarg(NOC);
             *ip++ = NOC;
             break;
    }
}

void idcode() {
    int i,j,k,all=1;
    ADDRESS t;

    j = ps;
    k = pre;
    match(pre);
    ensambla(nivel-simbolos[j].nivel,simbolos[j].offset);
    if ( k == _ARRAY ) {
        t = simbolos[j].dims[2];
        if ( pre == '[' ) {         /* Es elemento de un arreglo */
            i = 0;
            all = 0;
            /**
            printf("Corrigiendo tipo %#X por %#X\n",ip[-4],_ARREGLO);
            ip[-4] = _ARREGLO;     
            */
            match(pre);
            while ( 1 ) {
                expresion();
                chkarg(_ARREGLO); i++;
                if ( pre == ',' ) match(pre);
                else if ( pre != ']' )
                    error("Falta delimitador ']'");
                else break;
            }
            if ( i != t ) error("Los indices del arreglo no corresponden");
            else match(pre);

            *ip ++ = EVA;           /* Codigo para evaluar arreglos */
        } else *ip ++ = FAR;

        memcpy(ip,simbolos[j].dims,INT_WIDTH);
        ip += 2;
        *ip ++ = t;
        memcpy(ip,simbolos[j].dims+3,INT_WIDTH);
        ip += 2;

        if ( !all ) {
            for ( i=0; i < t; i++ ) {
                memcpy(ip,simbolos[j].dims+i*4+5,INT_WIDTH);
                ip += INT_WIDTH;
                memcpy(ip,simbolos[j].dims+i*4+7,INT_WIDTH);
                ip += INT_WIDTH;
            }
        }
    }
    if ( pre == AS && !inasgn ) {   /* Pretende realizar asignacion */
        inasgn = 1;
        match(AS);
        expresion();
        inasgn = 0;
        chktipo(ASG);
        *ip ++ = ASG;
    }
}

void match( int t ) {

    if ( pre == t )
        pre = scanner();
    else error("Error de Sintaxis.");

}

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
                else error("Variable sin definir.");
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
                 if ( (s-tmp) > 256 ) error("Comentario muy largo o falta delimitador");
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
                 if ( str_lon > 256 ) error("String muy largo");
             }
             *s = 0;

             if ( t != '\'' )
                 error("Delimitador Incorrecto");

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
                 a_memoria(dx,v.s,d.t);
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

/*      Manejo de Palabras Reservadas           */

struct {
    char *p;
    int t;
} reservadas[] = {
    "const", CONST,
    "var", VAR,
    "char", _CHAR,
    "integer", _INTEGER,
    "real", _REAL,
    "string", _STRING,
    "boolean", _BOOLEAN,
    "true", _TRUE,
    "false", _FALSE,
    "pi", _PI,
    "if", IF,
    "then", THEN,
    "else", ELSE,
    "while", WHILE,
    "for", FOR,
    "to", TO,
    "downto", DOWNTO,
    "repeat", REPEAT,
    "until", UNTIL,
    "case", CASE,
    "do", DO,
    "and", AND,
    "or", OR,
    "not", NOT,
    "mod", MDL,
    "procedure", PROCEDURE,
    "function", FUNCTION,
    "array", _ARRAY,
    "of", OF,
    "begin", BEGIN,
    "end", END,
    "read", READ,
    "readln", READLN,
    "write", WRITE,
    "writeln", WRITELN,
    0,     0
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

/*      Manejo de Tabla de Símbolos             */

int busca ( char *s ) {
    int i,existe=0;

    if ( (inargs || indecl || inproc) && !inrang ) {
        for ( i=si-1; i >= 0; i-- )
            if ( !strcmp(simbolos[i].nombre,s) && simbolos[i].nivel == nivel )
                return(i);
    } else {
        for ( i=si-1; i >= 0; i-- ) {
            if ( !strcmp(simbolos[i].nombre,s) && simbolos[i].nivel <= nivel )
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

/* Coloca un dato de la direccion de memoria a en el dato d */
VALOR de_memoria( unsigned s, unsigned o, ADDRESS t ) {
    //int s1 = s;
    VALOR v;
    ADDRESS *ptr=rs[ri-s]->datos;
    
    switch ( t & 0x07 ) {
        case  _CHAR:
        case  _BOOLEAN:memcpy(&v.c,ptr+o,sizeof(char));     break;
        case  _INTEGER:memcpy(&v.i,ptr+o,INT_WIDTH);        break;
        case  _REAL:   memcpy(&v.d,ptr+o,sizeof(double));   break;
        case  _STRING: v.s = ptr+o+1;                       break;
    }
    return(v);
}

/* Coloca el dato d en la posicion actual del apuntador dx y lo avanza */
int a_memoria( ADDRESS *ptr, ADDRESS *org, ADDRESS t ) {
    int size;

    switch ( t & 0x07 ) {
        case  _CHAR:
        case  _BOOLEAN:size = sizeof(char);     break;
        case  _INTEGER:size = INT_WIDTH;         break;
        case  _REAL:   size = sizeof(double);   break;
        case  _STRING: size = org[0]+2;         break;
    }
    memcpy(ptr,org,size);
    return(size);
}

void ensambla( ADDRESS seg, uint16_t off ) {

    *ip ++ = PSH;
    *ip ++ = d.t;
    *ip ++ = seg;
    memcpy(ip,&off,INT_WIDTH); ip += 2;
    ts[ti++] = d.t;
}

void chktipo( ADDRESS o ) {
    ADDRESS i,d;
    d = ts[--ti]&0x7;
    i = ts[--ti]&0x7;

    switch ( o ) {
        case  ASG:
             switch ( i ) {
                 case  _REAL:
                      if ( d == _REAL || d == _INTEGER ) ts[ti++] = _REAL;
                      else error("Asignacion de un no_REAL o conversion imposible");
                      break;
                 case  _INTEGER:
                      if ( d == _INTEGER ) ts[ti++] = _INTEGER;
                      else error("Asignacion de un no_INTEGER");
                      break;
                 case  _STRING:
                      if ( d == _CHAR || d == _STRING ) ts[ti++] = _STRING;
                      else error("Asignacion de un no_STRING");
                      break;
                 case  _CHAR:
                      if ( d == _CHAR ) ts[ti++] = _CHAR;
                      else error("Asignacion de un no caracter");
                      break;
                 case  _BOOLEAN:
                      if ( d == _BOOLEAN ) ts[ti++] = _BOOLEAN;
                      else error("Asignacion no booleana");
                      break;
             } break;
        case  ADD:
             switch ( i ) {
                 case  _REAL:
                      if ( d == _REAL || d == _INTEGER ) ts[ti++] = _REAL;
                      else error("Adicion entre tipos incompatibles");
                      break;
                 case  _INTEGER:
                      if ( d == _INTEGER ) ts[ti++] = _INTEGER;
                      else if ( d == _REAL ) ts[ti++] = _REAL;
                      else error("Adicion entre tipos incompatibles");
                      break;
                 case  _CHAR:
                      if ( d == _CHAR || d == _STRING ) ts[ti++] = _STRING;
                      else error("Adicion entre tipos incompatibles");
                      break;
                 case  _STRING:
                      if ( d == _CHAR || d == _STRING ) ts[ti++] = _STRING;
                      else error("Adicion entre tipos incompatibles");
                      break;
             } break;
        case  SUB:
        case  MUL:
        case  DIV:
             switch ( i ) {
                 case  _REAL:
                      if ( d == _REAL || d == _INTEGER ) ts[ti++] = _REAL;
                      else error("Operacion entre tipos incompatibles");
                      break;
                 case  _INTEGER:
                      if ( d == _INTEGER ) ts[ti++] = _INTEGER;
                      else if ( d == _REAL ) ts[ti++] = _REAL;
                      else error("Operacion entre tipos incompatibles");
                      break;
                 default: error("Operacion entre tipos incompatibles"); break;
             } break;
        case  MOD:
             switch ( i ) {
                 case  _INTEGER:
                      if ( d == _INTEGER ) ts[ti++] = _INTEGER;
                      else error("Residuo entre tipos incompatibles");
                      break;
                 default:
                      error("Operacion de Residuo no permitida");
                      break;
             } break;
        case  MEU:
             if ( d == _REAL ) ts[ti++] = _REAL;
             else if ( d == _INTEGER ) ts[ti++] = _INTEGER;
             else error("Operador unario aplicado a tipo incompatible");
             break;
        case  NOT:
             if ( d == _BOOLEAN ) ts[ti++] = _BOOLEAN;
             else error("Operador Logico aplicado a tipo incompatible");
             break;
        case  ORC:
        case  ANC:
             if ( i == _BOOLEAN && d == _BOOLEAN )      ts[ti++] = _BOOLEAN;
             else if ( i == _INTEGER && d == _INTEGER ) ts[ti++] = _INTEGER;
             else error("Expression logica entre tipos incompatibles");
             break;
        case GTC:
        case GEC:
        case LTC:
        case LEC:
        case NEC:
        case EQC:
            if ( i == _BOOLEAN && d == _BOOLEAN )       ts[ti++] = _BOOLEAN;
            else if ( (i == _INTEGER || i == _REAL) &&
                       (d == _INTEGER || d == _REAL) )  ts[ti++] = _BOOLEAN;
            else if ( i == _CHAR && d == _CHAR )        ts[ti++] = _BOOLEAN;
            else if ( i == _STRING && d == _STRING )    ts[ti++] = _BOOLEAN;
            else error("Expression logica entre tipos incompatibles");
            break;

    }
}

ADDRESS chkarg( ADDRESS arg ) {
    ADDRESS i;
    
    i = ts[--ti]&0x7;

    switch ( arg ) {
        case  _REAL:
             if ( i == _INTEGER || i == _REAL );
             else error("El argumento debe de ser Integer o Real");
             break;
        case  _STRING:
             if ( i == _STRING || i == _CHAR );
             else error("El argumento debe de ser String o Char");
             break;
        case  _CHAR:
             if ( i == _CHAR );
             else error("El argumento debe de ser Char");
             break;
        case  _BOOLEAN:
            if ( i == _BOOLEAN );
            else error("El argumento debe de ser Lógico");
             break;
        case  _ARREGLO:
             if ( i == _INTEGER || i == (_ARREGLO|_INTEGER) );
             else error("El indice del arreglo debe ser entero");
             break;
        case  NOC:
             if ( i == _BOOLEAN ) ts[ti++] = _BOOLEAN;
             else error("El argumento debe de ser Lógico");
             break;
        case  _LABEL:
             if ( i == _INTEGER || i == _CHAR ) return(i);
             else error("El tipo del selector es incorrecto");
             break;
    }
    return((ADDRESS) 0);
}

char *fota( double n ) {
    int dc,sg;
    char *p;

    p = fcvt(n,11,&dc,&sg);
    memmove(p+dc+1,p+dc,strlen(p)-dc);
    p[dc] = '.';
    if ( sg ) {
        memmove(p+1,p,strlen(p)+1);
        *p = '-';
    }
    memmove(p+10-dc,p,strlen(p)+1);
    memset(p,' ',10-dc);
    p[strlen(p)-1] = 0;
    return(p);
}

void cursor( int x, int y ) {
}

/*
    printf("Ejecucion: %ld\n",fin-ini);
    printf("Code %.4ld\nData %.4ld\nStack %.4ld\nHeap %.4ld\n",ip-cs,dx-ds,sp-ss,px-ds);
*/

#define gotoxy(x,y,c)       ( *(video+2*((x)*80+(y))) = (c) )
#define atribs(x,y,c)       ( *(video+2*((x)*80+(y))+1) = (c) )
#define reversx(x,y,c)      ( *(video+2*((x)*80+(y))+1) ^= (c) )

char *video;

void text( char *s, char x, char y, char a ) {
    printf("%s",s);
}

char * wsave( int ri, int ci, int rf, int cf ) {

    char *ptr,*buf;
    return(buf);
}

void wrest( int ri, int ci, int rf, int cf, char *ptr ) {
}

void wpain( int ri, int ci, int rf, int cf, int atr ) {
}

void wmark( int ri, int ci, int rf, int cf ) {
}

void error( char *s ) {

    printf("Error en linea %d: %s\n",linea,s);
    SysError = 1;
    longjmp(ambiente,control);
}

void msglines() {
    text("Compilando      lineas.",12,25,0);
    text(itoa(linea,0,10),12,36,0);
}

void _grabar() {
    int file;
    char *fichero;
    struct {
        unsigned data,code,inic;
    } obj;

    obj.data = dx-ds;
    obj.code = ip-cs;
    obj.inic = pc-cs;

    file = open(strcat(strtok(progname,"."),".bin"),
                O_CREAT|O_RDWR);
    write(file,(unsigned char *)&obj,sizeof(obj));
    write(file,(unsigned char *)ds,obj.data);
    write(file,(unsigned char *)cs,obj.code);
    close(file);
}

void cargar() {
    int file;
    struct {
        unsigned data,code,inic;
    } obj;

    file = open(strcat(strtok(progname,"."),".bin"),O_RDONLY);
    read(file,(unsigned char *)&obj,sizeof(obj));
    read(file,(unsigned char *)ds,obj.data);
    read(file,(unsigned char *)cs,obj.code);
    close(file);

    dx = ds + obj.data;
    ip = cs + obj.code;
    pc = cs + obj.inic;
}


/* 
    Leer un archivo en texto y devolver un apuntador
    de caracteres al buffer que lo contiene
*/
char * readfile( char *s ) {
    FILE *archivo = fopen(s, "rb");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return(NULL);
    }

    // Ir al final del archivo para medir su tamaño
    fseek(archivo, 0, SEEK_END);
    long tamano = ftell(archivo);
    rewind(archivo); // Regresar al inicio del archivo

    // Reservar memoria para el búfer (+1 para el carácter nulo '\0')
    char *buffer = (char *)malloc(tamano + 1);
    if (buffer == NULL) {
        printf("Error al asignar memoria.\n");
        fclose(archivo);
        return(NULL);
    }

    // Leer el contenido del archivo en el búfer
    size_t leido = fread(buffer, 1, tamano, archivo);
    buffer[leido] = '\0'; // Asegurar el fin de cadena para texto

    // Liberar recursos
    fclose(archivo);

    return(buffer);
}

char *strlwr(char *str) {
    unsigned char *p = (unsigned char *)str;
    while (*p) {
        *p = tolower(*p);
        p++;
    }
    return str;
}

char *itoa(int n, int buf, int base) {
    char *buffer = (char *)malloc(12 * sizeof(char)); 

    if (buffer == NULL) {
        return NULL; // Fallo al asignar memoria
    }

    // sprintf llena el buffer. No usamos su valor de retorno aquí.
    sprintf(buffer, "%d", n); 

    return buffer; // Devolvemos el puntero a la memoria asignada
}

void _log() {
    printf("Linea %d, Token %d\n", linea, pre);
} 

void show( char *s ) {
    while ( *s != NULL ) printf("%c",*s++);
}

void showTypeStack() {
    int i;

    printf("El apuntador indica: %d\n", ti);
    for ( i=ti; i >= 0; i -- )
        printf("--%#X--\n", ts[i]);
}

void debugDim( ADDRESS *d ) {
    int i;
    for ( i=0; i < 32; i ++ )
        printf("[%#X] ",d[i]);
    printf("\n");
}

/* 3 primeros bytes usados por ctes_predef  ('\n', True, False) */
void dumpDS() {
    int i;
    for ( i=0; i < 36; i ++ )
        printf("%3d ",ds[i]);
    printf("\n");
}

void dumpDSK() {
    int i;
    for ( i=0; i < 36; i ++ )
        printf("%#X ",ds[i]);
    printf("\n");
}

void dumpRS(int n) {
    ADDRESS *ptr=rs[n]->datos;
    
    printf("RS[%d] dump:\n",n);
    for ( int i=0; i < 32; i ++ ) printf("%3d ",ptr[i]); printf("\n");
}

void dumpSS() {
    
    printf("SS dump:\n");
    for ( int i=0; i < sp-ss; i ++ ) 
        printf("TYPE:%#X SEG:%3d OFF%3d, ",ss[i].t,ss[i].s,ss[i].o); printf("\n");
}

void dumpCS() {
    ADDRESS *codigo = cs;

    while ( codigo < ip ) {
        printf("%#X ",*codigo);
        codigo ++;
    }
}