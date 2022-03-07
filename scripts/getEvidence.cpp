#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include "values.h"
#include "getEvidence.h"
#include "getTables.h"

std::vector<Database*>* dbs;
std::vector<Table*>* tables;

std::ifstream* cfgfile;
std::ifstream* cfgbdfile;
FILE* resultcommand;

std::string cfg_filename;
std::string cfgbd_filename;
std::string input;
std::string testcase_prefix;

char localdate[9];
char localtimen[7];

using namespace std;

bool checkIfFileExists(string filename){
    ifstream *file = new ifstream(filename);
    return file->good();
}

//En esta función se realizan carga de archivos y limpieza de logs,
//y en caso sea especificado, tablas de base de datos antes de
//recolectar evidencia
int beforeRecolect(){
    
    printf("Ingrese el nombre del archivo de configuración: ");
    cin >> input;

    cfg_filename += "../cfgfiles/" + input;

    if(!checkIfFileExists(cfg_filename)) {
        printf("\n%sERROR:%s No se encontró el archivo de configuración, nombre o ruta inválida\n", RED, WHT);
        sleep(1);
        return CFG_FILE_DOESNT_EXIST; 
    }

    printf("Ingrese el nombre del archivo de configuración de bd: ");
    cin >> input;

    cfgbd_filename += "../bdconfig/" + input;

    if(!checkIfFileExists(cfgbd_filename)){
        printf("\n%sERROR:%s No se encontró el archivo de configuración de bd, nombre o ruta inválida\n", RED, WHT);
        sleep(1);
        return CFG_BD_FILE_DOESNT_EXIST; 
    }

    initParameters(cfg_filename, cfgbd_filename);

    printf("Ingrese el codigo del caso de prueba: ");

    cin >> testcase_prefix;

    resultcommand = popen("echo $(date +'%Y%m%d') && echo $(date +'%H%M%S')", "r");

    fgets(localdate, 9*sizeof(char), resultcommand);
 
    fgets(localtimen, 7*sizeof(char), resultcommand);
    fgets(localtimen, 7*sizeof(char), resultcommand);

    deleteDbs();

    printf("Para iniciar con la recoleccion de evidencia, pulse una tecla cuando la ejecución de prueba haya finalizado...\n");;

    cfg_filename = "";
    cfgbd_filename = "";
    getchar();
    getchar();
    return OK;
}

void recolectEvidence(){    
    if(beforeRecolect() != 0) return; 

    recolectAllTables("../files/lognuevo.log"); 
}