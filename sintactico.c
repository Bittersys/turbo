#include "sintactico.h"

/* Variables globales privadas: estrictamente exclusivas de sintactico.c */
static int pi;     /* Control de anidamiento en procs/funcs */
static int pre;    /* Token preanalizado actual (Lookahead) */
static int offset, idim;
static ADDRESS args[16], refs[16], dim[32];

/* Variables globales publicas: compartidas con lexico.c */
int  indecl,inargs,inproc,inasgn,inrang,rtn;     /*debug=0; */
int  nivel,array;

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
    proc_func();       
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
    uint16_t t=0,u;

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

uint16_t rangolist() {
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

void proc_func() {
    pi ++;
    while ( pre == PROCEDURE || pre == FUNCTION )
        if ( pre == PROCEDURE ) procedures();
        else if ( pre == FUNCTION ) functions();
}

void procedures() {
    int ax,xs;

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

        proc_func();          /* Agrego recursividad */
        simbolos[ax].offset = ip-cs;
        bloque();
        poplocal(xs);
        nivel --;
        *ip++ = RTN;
        match(';');
    }
}

void functions() {
    int ax,t,u,xs;

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
        proc_func();          /* Agrego recursividad */
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
        if ( simbolos[m].nivel+1 == (unsigned) nivel ) {
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
        default:
            error("Token inesperado");
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
        case  NOC:  /* Ejecutar pruebas de validacion */
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

void ensambla( ADDRESS seg, uint16_t off ) {

    *ip ++ = PSH;
    *ip ++ = d.t;
    *ip ++ = seg;
    memcpy(ip,&off,INT_WIDTH); ip += 2;
    ts[ti++] = d.t;
}

void _log() {
    printf("Linea %d, Token %d\n", linea, pre);
} 
