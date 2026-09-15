/*
turbo.c

Notes:
    The word size for the code segment must be
    16 bits.

    This version fixes the IF jumps with the correct
    byte size. By default it uses 2 for int, here it is fixed
    with the uint16_t data type.

    Next step is to review all instructions and their jumps
    so that it advances only 2 bytes.

    Currently the error is that the pointer advances 2 bytes but 
    unsigned data is copied with byte size.

    _ARRAY =    Reserved word
    ARREGLO =   Data type
    chkarg returns void and the return value is intended to be used
*/

#include "turbo.h"

/*      CODE SEGMENT Definition               */
ADDRESS
    cs[CODESIZE],           /* The code segment        */
    *pc,                    /* The program counter      */
    *ip;                    /* The pointer to the code       */

/*      DATA SEGMENT Definition                */
ADDRESS
    ds[DATASIZE],           /* The data segment         */
    *dx,                    /* The pointer to constants */
    *px;                    /* The pointer to temporaries */

/*      STACK SEGMENT Definition                */
DATO d,                     /* The temporary data             */
    *sp,                    /* The pointer to the stack        */
    ss[STACKSIZE];          /* The stack segment         */

/*      ACTIVATION RECORDS Definition           */
ACTIVACION
    *rs[REGSIZE];           /* Array of activation records */

unsigned
    ri;                     /* Current Activation Index  */

/*      Stack Definition for type checking        */
ADDRESS
    ti,                     /* Type Index              */
    ts[TIPOSIZE];           /* Type structure             */

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
            fprintf(stderr, "Error: Missing .bin file specification after --run\n");
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
    px = dx;        /* Assigns initial address of the stack */
    iz = 0;
    _cll(pc);
    fin:;
}

void _listado() {

    SysError = 0;
    control = setjmp(ambiente);
    if ( SysError ) goto fin;

    px = dx;        /* Assigns initial address of the stack */
    _lst();
    fin:;
}

void text( char *s ) {
    printf("%s",s);
}

void error( char *s ) {

    printf("Error on line %d: %s\n",linea,s);
    SysError = 1;
    longjmp(ambiente,control);
}

void msglines() {
    text("Compiling      lines.");
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
    Read a text file and return a character pointer
    to the buffer containing it
*/
char * readfile( char *s ) {
    FILE *archivo = fopen(s, "rb");
    if (archivo == NULL) {
        printf("Could not open the file.\n");
        return(NULL);
    }

    // Go to the end of the file to measure its size
    fseek(archivo, 0, SEEK_END);
    long tamano = ftell(archivo);
    rewind(archivo); // Return to the beginning of the file

    // Allocate memory for the buffer (+1 for the null character '\0')
    char *buffer = (char *)malloc(tamano + 1);
    if (buffer == NULL) {
        printf("Error allocating memory.\n");
        fclose(archivo);
        return(NULL);
    }

    // Read file content into the buffer
    size_t leido = fread(buffer, 1, tamano, archivo);
    buffer[leido] = '\0'; // Ensure string termination for text

    // Free resources
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
        return NULL; // Failed to allocate memory
    }

    // sprintf fills the buffer. We do not use its return value here.
    sprintf(buffer, "%d", n); 

    return buffer; // Return the pointer to the allocated memory
}

void show( char *s ) {
    while ( *s  ) printf("%c",*s++);
}

void showTypeStack() {
    int i;

    printf("The pointer indicates: %d\n", ti);
    for ( i=ti; i >= 0; i -- )
        printf("--%#X--\n", ts[i]);
}

void debugDim( ADDRESS *d ) {
    int i;
    for ( i=0; i < 32; i ++ )
        printf("[%#X] ",d[i]);
    printf("\n");
}

/* First 3 bytes used by ctes_predef ('\n', True, False) */
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
    printf("Turbo Compiler Usage:\n");
    printf("  %s <file.pas>       Compiles a Pascal source file.\n", prog_name);
    printf("  %s --run <file.bin> Loads and runs a binary bytecode directly.\n", prog_name);
}

void init_runtime() {
    ip = cs;            /* Initializes Instruction pointer */
    dx = ds;            /* Initializes Constant Data pointer */
    sp = ss;            /* Initializes Stack pointer          */
                        /*                                    */
    ri = 0;             /* Initializes Activation Index      */
    rs[ri] = (ACTIVACION *) malloc(sizeof(*rs[0]));
    rs[ri]->codigo= -1;
    rs[ri]->datos = ds;
    rs[ri]->ptr = NULL;

    ti = 0;             /* Initializes Type Checking Index */
    ctes_predef();    
}
