#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "rabin_karp.h"

void write_result(FILE* fresults, int found, char* pattern){
    if(found == 1){
        fprintf(fresults, "%s", pattern);
        fprintf(fresults, " ---------- HALLADO %d vez \n", found);
    }
    else if(found > 1){
        fprintf(fresults, "%s", pattern);
        fprintf(fresults, " ---------- HALLADO %d veces \n", found);
    }
    else{
        fprintf(fresults, "%s", pattern);
        fprintf(fresults, " ---------- NO HALLADO\n");
    }
}


void comp_files(FILE* flog, FILE* finput, FILE* fresults){
    
    char* linelog = NULL;
    char* lineinput = NULL;
    size_t lenllog = 0, lenlinput = 0;
    ssize_t readlog, readinput;
    int arr[100];
    int* posfound; 
    int ocd;

    while ((readinput = getline(&lineinput, &lenlinput, finput)) != -1) {
        rewind(flog);
        ocd = 0;
        while ((readlog = getline(&linelog, &lenllog, flog)) != -1) {
            posfound = rabin_karp(arr, lineinput, linelog, 101);
            ocd += posfound[99];
        }
        write_result(fresults, ocd, lineinput);
    }
}

int main(){
    FILE* flog;
    FILE* finput;
    FILE* fresults;

    char namelog[30];
    
    printf("Ingresa el nombre del archivo log a comparar: ");
    scanf("%[^\n]s", namelog);
  
    flog = fopen(namelog, "r");
    finput = fopen("input.txt", "r");
    fresults = fopen("results.txt", "w");

    if(flog == NULL)
        printf("Error leyendo el archivo log");

    if(finput == NULL)
        printf("Error leyendo el archivo de comparacion");

    if(fresults == NULL)
        printf("Error leyendo el archivo de resultados");

    comp_files(flog, finput, fresults);
    fclose(flog);
    fclose(finput);
    fclose(fresults);


    return 0;
}