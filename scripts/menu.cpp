#include <stdio.h>
#include <stdlib.h>
#include "getEvidence.h"
#include "values.h"
#include "validateLog.h"

using namespace std;

void menu(){
    int choice = 0;
    system("clear");
    do {
        printf("%s#########################\n", WHT);
        printf("#    FIND RECOLECTOR    #\n");
        printf("#########################\n");
        printf("                         \n");
        printf("1. Validar Flujo         \n");
        printf("2. Recolectar Evidencia  \n");
        printf("3. Salir                 \n");
        printf("Ingrese una opción: ");
        scanf("%d", &choice);

        switch (choice){
        case 1: 
            validateLog(); 
            system("clear");
            break;
        case 2:
            recolectEvidence();
            system("clear");
            break;
        case 3:
            break;
        default:
            break;
        }
    } while(choice != 3);
}

int main(){
    menu();
    return 0;
}