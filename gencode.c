#include "gencode.h"

static FILE *out;
static void qtipo(int t);

void _lst() {

    int i;
    char *name;
    ADDRESS *codigo = cs;
    
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

static void qtipo ( int t ) {
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