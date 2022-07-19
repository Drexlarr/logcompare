#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <unistd.h>

#include "../lib/getEvidence.h"

#include "../lib/global.h"
#include "../lib/utilities.h"
#include "../lib/values.h"
#include "../lib/getTables.h"
#include "../lib/validateLog.h"
#include "../lib/saveEvidence.h"

std::vector<Database*>* dbs;
std::vector<Table*>* tables;

std::ifstream* cfgfile;
std::ifstream* cfgbdfile;
FILE* resultcommand;

std::string input;

char localdate[9];
char localtimen[7];

using namespace std;

//En esta función se realizan carga de archivos y limpieza de logs,
//y en caso sea especificado, tablas de base de datos antes de
//recolectar evidencia
int beforeRecolect(){

    // Verificamos si existe la variable de entorno SIXDIR
    if (!getMainDir("SIXDIR")) {
      printf("  %sERROR%s La variable de entorno %sSIXDIR%s no existe\n", RED,
             WHT, YEL, WHT);
      return BAD;
    }
    
    printf("Ingrese el nombre del archivo de configuración: ");
    cin >> input;

    cfg_log =  path_cfg + "/" + input;

    if(!checkIfFileExists(cfg_log)) {
        printf("\n%sERROR:%s No se encontró el archivo de configuración, nombre o ruta inválida\n", RED, WHT);
        return CFG_FILE_DOESNT_EXIST; 
    }

    printf("Ingrese el nombre del archivo de configuración de bd: ");
    cin >> input;

    cfg_bd = path_cfg + "/" + input;

    if(!checkIfFileExists(cfg_bd)){
        printf("\n%sERROR:%s No se encontró el archivo de configuración de bd, nombre o ruta inválida\n", RED, WHT);
        return CFG_BD_FILE_DOESNT_EXIST; 
    }

    initParameters(cfg_log, cfg_bd);

    printf("Ingrese el codigo del caso de prueba: ");

    cin >> prefix;

    resultcommand = popen("echo $(date +'%Y%m%d') && echo $(date +'%H%M%S')", "r");

    fgets(localdate, 9*sizeof(char), resultcommand);
 
    fgets(localtimen, 7*sizeof(char), resultcommand);
    fgets(localtimen, 7*sizeof(char), resultcommand);

    deleteDbs();

    pclose(resultcommand);

    printf("Después de lanzar la trama, pulse cualquier tecla para capturar Evidencia\n");

    char wait;
    scanf("%c", &wait);
    date_after_launch = currentDateTime();

    return OK;
}

int afterEvidence(){
   return process();
}

int recolectEvidence(){    
    if(beforeRecolect() != 0) return BAD; 
    if(afterEvidence() != 0) return BAD;
    string new_log_six = path_evidence + "/" + prefix + " LOG-SIX.log";
    recolectAllTables(new_log_six); 
    getchar();

    return OK;
}
