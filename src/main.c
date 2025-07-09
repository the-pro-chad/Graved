/*
 * main.c: archivo encargado de ejecutar el interpreter*/
#include "../include/interpret.h"

void read_file(const char* filename) {
    FILE * file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: No se pudo abrir el  archivo: '%s'\n", filename);
        exit(1);
    }
    char content_file[MAX_FILE_READ];
    size_t bytes_read = fread(content_file, sizeof(char), MAX_FILE_READ - 1, file);
    fclose(file);
    if (bytes_read == 0) {
        fprintf(stderr, "Error: El archivo '%s' está vacío o no se pudo leer\n", filename);
        exit(1);
    }
    content_file[bytes_read] = '\0';
    tokenize(content_file);
    interpret();
}

void interpret_in_line (){
	char line[1024];
	puts("Graved v0.0.1-alpha");
	while(true){
          printf(">> ");
	  fgets(line, 1024, stdin);
	  tokenize(line);
          interpret();
	}
}

void print_help() {
    printf("Uso: Graved [opciones]\n");
    printf("Opciones:\n");
    printf("  -i <archivo>    Indicar que se va a interpretar un archivo de Graved\n");
    printf("  -h              Muestra este mensaje de ayuda\n");
    printf("  -v              Muestra la versión de Graved\n");
    printf("  -l              Prueba algunos codigos directamente desde la cmd\n");
}

void print_version () {
    printf("Graved v0.0.1-alpha\n");
}

int main(int argc, char *argv[]) {
    int opt;
    char* filename = NULL;
    while ((opt = getopt(argc, argv, "i:hvl")) != -1) {
        switch (opt) {
            case 'i':
                filename = optarg;
                break;
            case 'h':
                print_help();
                exit(0);
            case 'v':
                print_version();
                exit(0);
            case 'l':
                interpret_in_line ();
            default:
                fprintf(stderr, "Uso: %s -i <archivo> o %s -h\n", argv[0], argv[0]);
                exit(1);
        }
    }
    if (filename == NULL) {
        fprintf(stderr, "Error: No se especificó un archivo de entrada.\n");
        fprintf(stderr, "Uso: %s -i <archivo> o %s -h(para ayuda)\n", argv[0], argv[0]);
        exit(1);
    }
    read_file(filename);
    return 0;
}
