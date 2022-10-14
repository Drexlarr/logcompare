#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

#include "../lib/getEvidence.h"
#include "../lib/global.h"
#include "../lib/values.h"
#include "../lib/validateLog.h"
#include "../lib/utilities.h"

using namespace std;

void config_files(){
    if (!getMainDir("SIXDIR")) {
        printf("%sERROR%s La variable de entorno %sSIXDIR%s no existe\n", RED,
               WHT, YEL, WHT);
        return;
    }

    std::string input;
    printf("\nIngrese el nombre del archivo de configuracion: ");
    cin >> input;

    cfg_log = path_cfg + "/" + input;

    if(!checkIfFileExists(cfg_log)){
        printf("\n%sERROR:%s No se pudo encontrar los archivos opcionales\n", RED, WHT);
        return;
    }
    
    printf("Ingrese el nombre del archivo de configuracion de bd: ");
    cin >> input;

    cfg_bd = path_cfg + "/" + input;

    if(!checkIfFileExists(cfg_bd)){
        printf("\n%sERROR:%s No se encontro el archivo de configuracion de bd, nombre o ruta invalida\n", RED, WHT);
        return;
    }
}

void menu(){

    int choice = 0;
    system("clear");

    do {
        // Preconfiguración
        // Verificamos si existe la variable de entorno SIXDIR
        // Restauramos variables
        if (!getMainDir("SIXDIR")) {
            printf("%sERROR%s La variable de entorno %sSIXDIR%s no existe\n", RED,
                   WHT, YEL, WHT);
            return;
        }

        if (choice != 3){
            printf("+--------------------------------------------------------+\n");
            printf("|                                                        |\n");
            printf("|               LOG RECOLECTOR & VALIDATOR               |\n");
            printf("|                                                        |\n");
            printf("+--------------------------------------------------------+\n");
            printf("|                                                        |\n");
            printf("|          Ingrese el codigo del caso de prueba          |\n");
            printf("|                                                        |\n");
            printf("|                                                        |\n");
            printf("|               Para salir presione \"-\"                  |\n");
            printf("|                                                        |\n");
            printf("+--------------------------------------------------------+\n");
            printf("                   Codigo: ");
            cin >> prefix;

            if (prefix == "-"){
                system("clear");
                break;
            }
        }

        system("clear");

        printf("+--------------------------------------------------------+\n");
        printf("|                                                        |\n");
        printf("|               LOG RECOLECTOR & VALIDATOR               |\n");
        printf("|                                                        |\n");
        printf("+--------------------------------------------------------+\n");
        printf("|                                                        |\n");
        printf("|                                                        |\n");
        printf("|                  1. Recolectar Evidencias              |\n");
        printf("|                                                        |\n");
        printf("|                                                        |\n");
        printf("|                  2. Validar Log                        |\n");
        printf("|                                                        |\n");
        printf("|                                                        |\n");
        printf("|                  3. Cambiar Configuracion              |\n");
        printf("|                                                        |\n");
        printf("|                                                        |\n");
        printf("|                  4. Salir                              |\n");
        printf("|                                                        |\n");
        printf("|                                                        |\n");
        printf("+--------------------------------------------------------+\n");
        printf("                   Opcion: ");
        scanf("%d", &choice);

        switch (choice){
        case 1:
            recolectEvidence();
            sleep(5); 
            system("clear");
            break;
        case 2: 
            validateLog(); 
            system("clear");
            break;
        case 3:
            config_files();
            break;
        case 4:
            break;
        default:
            system("clear");
            break;
        }
    } while(choice != 4);
}

int main(){
    config_files();
    menu();
    return 0;
}
