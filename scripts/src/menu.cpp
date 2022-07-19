#include <stdio.h>
#include <stdlib.h>

#include "../lib/getEvidence.h"
#include "../lib/values.h"
#include "../lib/validateLog.h"

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
            if (recolectEvidence() != 0){
                choice = 3;
            }
            break;
        case 3:
            break;
        default:
            system("clear");
            break;
        }
    } while(choice != 3);
}

int main(){
    menu();
    return 0;
}
