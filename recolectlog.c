#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "rabin_karp.h"


void write_log(FILE* flog, int pos, char* line){
    fprintf(flog, "===>");
    for(int i = pos+7; i < strlen(line); i++){
        if(line[i] == '<') break;
        fprintf(flog, "%c", line[i]);
    } 
    fprintf(flog, "\n");
}

void find_log_lines(FILE* flog, FILE* fxml){
    
    char* linexml = NULL;
    size_t lenlxml = 0;
    ssize_t readxml;
    int arr[100];
    int* posfound; 
    int ocd;

    while ((readxml = getline(&linexml, &lenlxml, fxml)) != -1) {
        posfound = rabin_karp(arr, "===&gt;", linexml, 101);
        if(posfound[99] > 0){
            for(int i = 0; i < posfound[99]; i++){
                write_log(flog, posfound[i], linexml);
            }
        }
    }
}


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
    FILE* finput;
    FILE* fxml; 
    FILE* flog;
    FILE* fresults;
    char* linelog = NULL;
    char* lineinput = NULL;
    size_t lenllog = 0, lenlinput = 0;
    char namelog[30];


    printf("Ingrese el nombre del archivo log: ");
    scanf("%s", namelog);

    if (access(namelog, F_OK) != 0 && strcmp(namelog, "results.txt") != 0 && strcmp(namelog, "input.txt") != 0){
        printf("Nombre de archivo invalido\n");
        return 0;
    }

    finput = fopen("input.txt", "w");
    fxml = fopen("clear.xml", "r"); 
    fresults = fopen("results.txt", "w");
    flog = fopen(namelog, "r");

    find_log_lines(finput,fxml);

    fclose(finput);
    finput = fopen("input.txt", "r");

    comp_files(flog, finput, fresults);

    fclose(finput);
    fclose(fxml);
    fclose(fresults);
    fclose(flog);

    return 0;
}