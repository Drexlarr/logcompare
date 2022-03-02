#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "values.h"
#include "getEvidence.h"
#include "getTables.h"

struct Database** dbs;
struct Table** tables;

FILE* cfgfile = NULL;
FILE* cfgbdfile = NULL;
FILE* result_command = NULL;



char localdate[9];
char localtime[7];
char cfg_filename[101];
char cfgbd_filename[101];
char input[101];


int checkIfFileExists(const char * filename){
    FILE *file;
    if (file = fopen(filename, "r")){
        fclose(file);
        return 1;
    }

    return 0;
}


int beforeDelete(){
    
    printf("Ingrese el nombre del archivo de configuración: ");
    scanf("%s", input);

    sprintf(cfg_filename, "../cfgfiles/%s", input);

    if(!checkIfFileExists(cfg_filename)) {
        printf("\n%sERROR:%s No se encontró el archivo de configuración, nombre o ruta inválida\n", RED, WHT);
        sleep(1);
        return CFG_FILE_DOESNT_EXIST; 
    }

    printf("Ingrese el nombre del archivo de configuración de bd: ");
    scanf("%s", input);

    sprintf(cfgbd_filename, "../bdconfig/%s", input);

    if(!checkIfFileExists(cfgbd_filename)){
        printf("\n%sERROR:%s No se encontró el archivo de configuración de bd, nombre o ruta inválida\n", RED, WHT);
        sleep(1);
        return CFG_BD_FILE_DOESNT_EXIST; 
    }

    initParameters(cfg_filename, cfgbd_filename);

    printf("\nPresione una tecla en cuanto la ejecución de la prueba haya culminado...");
    getchar();
    return OK;

}

void recolectEvidence(){    
    if(beforeDelete() != 0) return; 

    selectTracesQuery(dbs[1], tables[1], "../files/lognuevo.log");

    dbs = NULL;
    tables = NULL;
    
}