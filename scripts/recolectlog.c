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
        posfound = rabin_karp("===&gt;", linexml, 101);
        if(posfound[99] > 0){
            for(int i = 0; i < posfound[99]; i++){
                write_log(flog, posfound[i], linexml);
            }
        }
    }
}
    
char* get_process_name(char* line){
    char pname[10];
    char* name; 

    for(int i = 0; i < 10; i++)
        pname[i] = line[42+i];

    name = pname;
    return name;
}




void comp_files(FILE* flog, FILE* finput, FILE* fresults){
    
    char* linelog = NULL;
    char* lineinput = NULL;
    size_t lenllog = 0, lenlinput = 0;
    ssize_t readlog, readinput;
    int* arr; 
    int ocd; 
    char* process_name;

    while ((readinput = getline(&lineinput, &lenlinput, finput)) != -1) {
        rewind(flog);
        ocd = 0;
        fprintf(fresults, "\n%s", lineinput);
        while ((readlog = getline(&linelog, &lenllog, flog)) != -1) {
            arr = rabin_karp(lineinput, linelog, 101);
            ocd += arr[99];       
            if (arr[99] > 0){
                process_name = get_process_name(linelog);
                fprintf(fresults, " ---------- HALLADO - NOMBRE DEL PROCESO \t\t%s \n", process_name );
            }
        }
        if(!ocd) 
            fprintf(fresults, " ---------- NO HALLADO\n");
    }
}

//int main(int argc, char *argv[]){
    //FILE* finput;
    //FILE* fxml; 
    //FILE* flog;
    //FILE* fresults;
    


    //finput = fopen("files/input.txt", "w");
    //fxml = fopen("files/clear.xml", "r"); 
    //fresults = fopen("files/results.txt", "w");
    //flog = fopen(argv[1], "r");


    //find_log_lines(finput,fxml);
    //fclose(finput);
    //finput = fopen("files/input.txt", "r");

    //comp_files(flog, finput, fresults);

    //fclose(finput);
    //fclose(fxml);
    //fclose(fresults);
    //fclose(flog);

    //return 0;
//}