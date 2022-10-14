#include "../lib/getEvidence.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../lib/getTables.h"
#include "../lib/global.h"
#include "../lib/saveEvidence.h"
#include "../lib/utilities.h"
#include "../lib/validateLog.h"
#include "../lib/saveOptionalLog.h"
#include "../lib/values.h"

std::vector<Database *> *dbs;
std::vector<Table *> *tables;

std::ifstream *cfgfile;
std::ifstream *cfgbdfile;
FILE *resultcommand;

std::string input;

char localdate[9];
char localtimen[7];

using namespace std;

// En esta función se realizan carga de archivos y limpieza de logs,
// y en caso sea especificado, tablas de base de datos antes de
// recolectar evidencia
int beforeRecolect() {
    printf("\n- Ingrese el nombre del archivo de configuración: ");
    cin >> input;

    cfg_log = path_cfg + "/" + input;

    if (!checkIfFileExists(cfg_log)) {
        printf("\n%sERROR:%s No se encontró el archivo de configuración, nombre o ruta inválida\n", RED, WHT);
        return CFG_FILE_DOESNT_EXIST;
    }

    if(getOptionalLog()){
        printf("\n%sERROR:%s No se pudo encontrar los archivos opcionales\n", RED, WHT);
        return BAD;
    }

    printf("- Ingrese el nombre del archivo de configuración de bd: ");
    cin >> input;

    cfg_bd = path_cfg + "/" + input;

    if (!checkIfFileExists(cfg_bd)) {
        printf("\n%sERROR:%s No se encontró el archivo de configuración de bd, nombre o ruta inválida\n", RED, WHT);
        return CFG_BD_FILE_DOESNT_EXIST;
    }

    initParameters(cfg_log, cfg_bd);

    deleteDbs();

    date_after_launch = currentDateTime();

    printf("- Después de lanzar la trama, pulse cualquier tecla para capturar Evidencia\n");

    getchar();
    getchar();

    return OK;
}

int afterEvidence() {
    return process();
}

int recolectEvidence() {
    if (beforeRecolect() != 0)
        return BAD;
    if (afterEvidence() != 0)
        return BAD;

    string new_log_six = path_evidence + "/" + prefix + "/" + prefix + " LOG-SIX.log";
    recolectAllTables(new_log_six);
    getchar();

    return OK;
}
