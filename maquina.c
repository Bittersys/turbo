#include "maquina.h"

void _cll( ADDRESS *codigo ) {
    
    while ( *codigo != RTN && codigo < ip ) {
        switch( *codigo ) {
            case  ADD:  _add();
                        codigo ++;
                        break;
            case  ALC:  _alc(IVAL(codigo+1),IVAL(codigo+3));
                        codigo += 5;
                        break;
            case  ANC:  _anc();
                        codigo ++;
                        break;
            case  ASG:  _asg();
                        codigo ++;
                        break;
            case  CAC:  codigo = _cac(codigo+1);
                        break;
            case  CLL:  _cll(cs+IVAL(codigo+1));                        
                        codigo += 3;
                        break;
            case  CPY:  _cpy(codigo[1]);
                        codigo += 2;
                        break;
            case  DEC:  _dec();
                        codigo ++;
                        break;
            case  DIV:  _div();
                        codigo ++;
                        break;
            case  EQC:  _eqc();
                        codigo ++;
                        break;
            case  EVA:  _eva(codigo+1);
                        codigo += codigo[3]*4+6;
                        break;
            case  SUB:  _sub();
                        codigo ++;
                        break;
            case  MUL:  _mul();
                        codigo ++;
                        break;
            case  MOD:  _mod();
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
            case  NEC:  _nec();
                        codigo ++;
                        break;
            case  FRE:  _fre();
                        codigo ++;
                        break;
            case  PSH:  _psh(codigo[1],codigo[2],IVAL(codigo+3));
                        codigo += 5;
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
            case  REA:  _rea();
                        codigo ++;
                        break;
            case  FAR:  szs[iz++] = IVAL(codigo+4);
                        codigo += 6;
                        break;
            case  IFC:  codigo = _ifc(codigo+1);
                        break;
            case  INC:  _inc();
                        codigo ++;
                        break;
            case  JMP:  codigo = cs + IVAL(codigo+1);
                        break;
            case  STK:  _stk(codigo[1],codigo[2]);
                        codigo += 3;
                        break;
            case  WRT:  _wrt();
                        codigo ++;
                        break;
        }
    }
}

void _wrt() {
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
    fgets(&tmp[1],255,stdin);

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
    ADDRESS t1,t2;
    char tmp[256];

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
        if ( TIPO(t2) == _CHAR ) tmp[(int)tmp[0]] = v.c;
        else strcat(tmp+1,v.s+1);
        px += a_memoria(px,(ADDRESS *)tmp,_STRING);
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

    d = * --sp; v = de_memoria(d.s,d.o,d.t);
    d = * --sp; w = de_memoria(d.s,d.o,d.t);
    if ( v.i == 0 ) error("Error de ejecucion.Division por cero");

    w.i %= v.i;
    d.t = _CONSTANTE|TIPO(d.t);
    _psh(d.t,ri,px-ds);
    px += a_memoria(px,(ADDRESS*)&w,d.t);

}

void _anc() {
    VALOR w;

    d = * --sp; v = de_memoria(d.s,d.o,d.t);
    d = * --sp; w = de_memoria(d.s,d.o,d.t);
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
    _psh(_CONSTANTE|(0x0f&d.t),x.s,x.o);
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
    ADDRESS j,k;

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

void _stk( ADDRESS arg, ADDRESS cod ) {
    DATO x;

    if ( cod ) x = * --sp;
    _uns(arg);
    if ( cod ) *sp ++ = x;

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
    ADDRESS      s2=hasta.s;
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
    ADDRESS      s1=desde.s/*,s2=hasta.s*/;
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


/* Coloca un dato de la direccion de memoria a en el dato d */
VALOR de_memoria( unsigned s, unsigned o, ADDRESS t ) {
    VALOR v;
    ADDRESS *ptr=rs[ri-s]->datos;
    
    switch ( t & 0x07 ) {
        case  _CHAR:
        case  _BOOLEAN:memcpy(&v.c,ptr+o,sizeof(char));     break;
        case  _INTEGER:memcpy(&v.i,ptr+o,INT_WIDTH);        break;
        case  _REAL:   memcpy(&v.d,ptr+o,sizeof(double));   break;
        case  _STRING: v.s = (char *)(ptr + o + 1);         break;
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
