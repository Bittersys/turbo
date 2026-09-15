/*
turbo.c

Notas:
    El tamaño de palabra para el segmento de codigo debe ser
    de 16 bits.

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

#include "turbo.h"

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

int     control, parserok, linea;
jmp_buf ambiente;
char    *progname,
        *bfr,*pun;
long    ini,fin,iz,szs[16];
unsigned bsize,SysError;

extern void analisis(),ctes_predef(),
    _lst(), _cll( ADDRESS *codigo );            

int main( int argc, char ** argv ) {

    if (argc < 2) {
        mostrar_ayuda(argv[0]);
        return EXIT_FAILURE;
    }

    if ( !strcmp(argv[1], "--run") ) {
        if (argc < 3) {
            fprintf(stderr, "Error: Falta especificar el archivo .bin después de --run\n");
            return EXIT_FAILURE;
        }
        
        progname = argv[2];
        init_runtime();        
        _cargar();
        _listado();
        _ejecuta();
        return EXIT_SUCCESS;
    }


    if ( _compila( argv[1] ) != linea  ) {        
        _listado();    
        _ejecuta();
        return EXIT_SUCCESS;
    }
}

int _compila( char *filename ) {
    pun      =
    bfr      = readfile(filename);
    bsize    = strlen(bfr);
    progname = filename;
    SysError = 0;
    control = setjmp(ambiente);
    if ( SysError ) goto fin;

    init_runtime();
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

void text( char *s ) {
    printf("%s",s);
}

void error( char *s ) {

    printf("Error en linea %d: %s\n",linea,s);
    SysError = 1;
    longjmp(ambiente,control);
}

void msglines() {
    text("Compilando      lineas.");
    text(itoa(linea));
}

void _grabar() {
    int file;
    struct {
        unsigned data,code,inic;
    } obj;

    obj.data = dx-ds;
    obj.code = ip-cs;
    obj.inic = pc-cs;

    file = open(strcat(strtok(progname,"."),".bin"),
                O_WRONLY | O_CREAT | O_TRUNC, 0644);
        write(file,(unsigned char *)&obj,sizeof(obj));
        write(file,(unsigned char *)ds,obj.data);
        write(file,(unsigned char *)cs,obj.code);
    close(file);
}

void _cargar() {
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

char *itoa(int n) {
    char *buffer = (char *)malloc(12 * sizeof(char)); 

    if (buffer == NULL) {
        return NULL; // Fallo al asignar memoria
    }

    // sprintf llena el buffer. No usamos su valor de retorno aquí.
    sprintf(buffer, "%d", n); 

    return buffer; // Devolvemos el puntero a la memoria asignada
}

void show( char *s ) {
    while ( *s  ) printf("%c",*s++);
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
    for ( int i=0; i < 32; i ++ )
	printf("%3d ",ptr[i]);
    printf("\n");
}

void dumpSS() {
    
    printf("SS dump:\n");
    for ( int i=0; i < sp-ss; i ++ ) 
        printf("TYPE:%#X SEG:%3d OFF%3d, ",ss[i].t,ss[i].s,ss[i].o);
    printf("\n");
}

void dumpCS() {
    ADDRESS *codigo = cs;

    while ( codigo < ip ) {
        printf("%#X ",*codigo);
        codigo ++;
    }
}

void mostrar_ayuda(const char *prog_name) {
    printf("Uso del Compilador Turbo:\n");
    printf("  %s <archivo.pas>       Compila un archivo fuente de Pascal.\n", prog_name);
    printf("  %s --run <archivo.bin> Carga y ejecuta un bytecode binario directamente.\n", prog_name);
}

void init_runtime() {
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
}