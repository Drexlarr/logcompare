#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "values.h"
#include "getTables.h"
#include "rabin_karp.h"

extern FILE* cfgfile;
extern FILE* cfgbdfile;
extern FILE* result_command;

char **traces;

extern struct Database** dbs;
extern struct Table** tables;

int n_tables; 

//Funcion que retorna el numero de tablas que se recolectaran, segun lo definido
//por el parametro ntables en el archivo de configuracion
int getNTables(){
    char* bdtag = "[TABLES_BD]";
    char line[100];
    char* number;
    while(fgets(line,sizeof(line),cfgfile)){
        if(strstr(line, bdtag) != NULL)
            break;
    }    

    fgets(line, sizeof(line), cfgfile);

    number = strtok(line, "=");
    number = strtok(NULL, "=");
    return atoi(number);
}

//Funcion que carga los parametros encontrados en el archivo de configuración
//en la estructura Table y asigna valores a sus atributos
void getTableParameters(struct Table* table){
    char parameter[101];
    char line[101];
    int needed = 0;
    
    while(fgets(line, sizeof(line), cfgfile)){     
        if(strncmp(line, "[", 1) == 0) {
            break;
        }
    } 

    while(fgets(line, sizeof(line), cfgfile)){
        if(strcmp(line, "\n") == 0) break;

        if(strcmp(table->deltable, "Y") == 0 && needed == 3) break;

        strcpy(parameter, strtok(line, "="));
        if(strcmp(parameter, "tablename") == 0){
            strcpy(parameter, strtok(NULL, "="));
            strncpy(table->tablename, parameter, strlen(parameter) - 1);
            needed++;
        }
        else if(strcmp(parameter, "database") == 0){
            strcpy(parameter,strtok(NULL, "="));
            strncpy(table->database, parameter, strlen(parameter) - 1);
            needed++;
        }
        else if(strcmp(parameter, "deltable") == 0){
            strcpy(parameter,strtok(NULL, "="));
            strncpy(table->deltable, parameter, strlen(parameter) - 1);
            needed++;
        }

        else if(strcmp(parameter, "tracefield") == 0){
            strcpy(parameter,strtok(NULL, "="));
            strncpy(table->tracefield, parameter, strlen(parameter) - 1);
        }
        else if(strcmp(parameter, "columntrace") == 0){
            strcpy(parameter,strtok(NULL, "="));
            strncpy(table->columntrace, parameter, strlen(parameter) - 1);
        }
        else if(strcmp(parameter, "columndate") == 0){
            strcpy(parameter,strtok(NULL, "="));
            strncpy(table->columndate, parameter, strlen(parameter) - 1);
        }
        else if(strcmp(parameter, "columntime") == 0){
            strcpy(parameter,strtok(NULL, "="));
            strncpy(table->columntime, parameter, strlen(parameter) - 1);
        }
    }

    if(needed < 3 || table->deltable == NULL){
        printf("%sERROR:%s No se encontraron todos los parametros necesarios", RED, WHT);
        return;
    }
}

//Funcion que carga los parametros encontrados en el archivo de configuración de bd
//en la estructura Database y asigna valores a sus atributos
void getCredentialsBD(char* database, struct Database* DB){
    char line[101];
    char db[101];
    char parameter[101];
    int needed = 0;
    int bd_exists = 0;

    sprintf(db, "[%s]", database);
 
    while(fgets(line, sizeof(line), cfgbdfile)){     
        if(strncmp(line, db, strlen(line)-2) == 0) {
            bd_exists = 1;
            break;
        }
    }
    
    if(!bd_exists){
        printf("%sERROR:%s No existe la etiqueta [%s]\n", RED, WHT, database);
        return;
    }

    while(fgets(line, sizeof(line), cfgbdfile)){
        if(strcmp(line, "\n") == 0) break;
        strcpy(parameter, strtok(line, "="));

        if(strcmp(parameter, "host") == 0){
            strcpy(parameter, strtok(NULL, "="));
            strncpy(DB->host, parameter, strlen(parameter)-1);
            needed++;
        }
        else if(strcmp(parameter, "port") == 0){
            strcpy(parameter, strtok(NULL, "="));
            strncpy(DB->port, parameter, strlen(parameter)-1);
            needed++;
        }
        else if(strcmp(parameter, "user") == 0){
            strcpy(parameter, strtok(NULL, "="));
            strncpy(DB->user, parameter, strlen(parameter)-1);
            needed++;
        }

        else if(strcmp(parameter, "password") == 0){
            strcpy(parameter, strtok(NULL, "="));
            strncpy(DB->password, parameter, strlen(parameter)-1);
            needed++;
        }
    }

    if(needed < 4){ 
        printf("%sERROR:%s No se encontraron todos los parametros necesarios", RED, WHT);
        return;
    }     
}

//Funcion que genera un comando de conexión a una base de datos
//según los valores que contengan los atributos del parametro db
void connectQuery(struct Database* db, char* command){
    char comm[101];
    sprintf(comm, "export PGPASSWORD='%s'; psql -h %s -p %s -U %s", db->password, db->host, db->port, db->user);
    strcpy(command, comm);
    printf("%s", command);
}

void initParameters(char* cfg_filename, char* cfgbd_filename){
    printf("Entramos initparameters\n");
    char line[101];
    printf("Estamos en files antes \n");
    cfgfile = fopen(cfg_filename, "r");
    printf("Estamos en files antes \n");
    cfgbdfile = fopen(cfgbd_filename, "r");

    printf("Estamos en files \n");

    n_tables = getNTables();

    printf("Estamos en n_tables %d\n", n_tables);

    dbs = (struct Database**) malloc(n_tables * sizeof(struct Database*));
    tables = (struct Table**) malloc(n_tables * sizeof(struct Table*));

    for(int i = 0; i < n_tables; i++){
        tables[i] = (struct Table*) malloc(sizeof(struct Table));
        getTableParameters(tables[i]);
    }
    for(int i = 0; i < n_tables; i++){
        dbs[i] = (struct Database*) malloc(sizeof(struct Database));
        getCredentialsBD(tables[i]->database, dbs[i]);
        rewind(cfgbdfile);
    } 

   
    fclose(cfgfile);
    fclose(cfgbdfile); 
    cfgfile = NULL;
    cfgbdfile = NULL;
}

void removeElement(int size, int index){
    char** temp = (char**)malloc((size)* sizeof(char*));
    memmove(temp, traces, size * sizeof(char*));
    free(traces);
    traces = (char**)malloc((size - 1)* sizeof(char*));
    for(int i = 0; i < size-1; i++)
        traces[i] = (char*)malloc(7 * sizeof(char));
    
    if(index != 0){
        for(int i = 0; i < index; i++){
            strcpy(traces[i], temp[i]);
        }
    }

    if(index != (size - 1)){
        for(int i = index; i < size-1; i++){
            strcpy(traces[i], temp[i+1]);
        }
    } 

    for(int i = 0; i < size; i++)
        free(temp[i]);
    free(temp);
}


int removeDuplicate(int n_traces){
    int aux = n_traces;
    int first = 0;
    int* duplicated;
    for(int i = 0; i < aux; i++){
        for(int j = 0; j < aux; j++){
            if(strcmp(traces[i], traces[j]) == 0 && j != i){
                removeElement(aux, j);
                aux--;
            }
        }
    }
    return aux;
}

void delQuery(struct Database* db){
    char* query;
    sprintf(query, "-c %s DELETE FROM %s", db->host);
}

int isInTraces(int n, char* aux){
    for(int i = 0; i < n; i++){
        printf("Comparando %s con %s", traces[i], aux);
        if(strncmp(traces[i], aux, 6) == 0) return 1;
    }

    
    return 0;
}

int getTracesFromLog(char* log_file){ 
    FILE* flog = fopen(log_file, "r");
    int n_traces = 0;
    char command[101];
    char line[10];
    char* linelog = NULL;
    size_t lenllog = 0;
    ssize_t readlog;
    int* posfound;
    int index_traces = 0;
    char auxtrace[7];
    
    sprintf(command, "grep -o 'B011' '%s' | wc -l && grep -o 'DE011' '%s' | wc -l", log_file, log_file);

    result_command = popen(command, "r");
    fgets(line, sizeof(line), result_command);
    n_traces += atoi(line);
    fgets(line, sizeof(line), result_command);
    n_traces += atoi(line);

    if(n_traces == 0) return 0;


    //for(int i = 0; i < n_traces; i++)
    //    traces[i] = (char*)malloc(7 * sizeof(char));

    
    while ((readlog = getline(&linelog, &lenllog, flog)) != -1) {
        posfound = rabin_karp("B011[", linelog, 101);
        if(posfound[99] > 0){
            if(index_traces == 0){
                traces = (char**)malloc(sizeof(char*));
                traces[index_traces] = (char*)malloc(7*sizeof(char*));
                strncpy(traces[index_traces], linelog + posfound[0] + 5, 6);
                index_traces++; 
            }
            else {
                strncpy(auxtrace, linelog + posfound[0] + 5, 6);
                if(!isInTraces(index_traces, auxtrace)) {
                    realloc(traces, index_traces * sizeof(char*));
                    traces[index_traces] = (char*)malloc(7*sizeof(char*));
                    strncpy(traces[index_traces], auxtrace, 6);  
                    index_traces++;
                }
            }
        }
        posfound = rabin_karp("DE011[", linelog, 101);
        if(posfound[99] > 0){
            if(index_traces == 0){
                traces = (char**)malloc(sizeof(char*));
                traces[index_traces] = (char*)malloc(7*sizeof(char*));
                strncpy(traces[index_traces], linelog + posfound[0] + 6, 6);
                index_traces++; 
            }
            else {
                strncpy(auxtrace, linelog + posfound[0] + 6, 6);
                if(!isInTraces(index_traces, auxtrace)) {
                    realloc(traces, index_traces * sizeof(char*));
                    traces[index_traces] = (char*)malloc(7*sizeof(char*));
                    strncpy(traces[index_traces], auxtrace, 6);  
                    index_traces++;
                }
            }
        }
    } 

    //n_traces = removeDuplicate(n_traces); 

    return index_traces;
}

void selectTracesQuery(struct Database* db, struct Table* table, char* log_file){
    int n_traces = getTracesFromLog(log_file);
    
    if(n_traces == 0) {
        selectAllQuery(db, table);
        return;
    }

    char* group_of_traces = (char*)malloc((n_traces * 10 * sizeof(char)) + 101);
    char format_trace[14];
    char connect_command[101];
    char query[151];
    
    connectQuery(db, connect_command);

    printf("No es problema de la bd\n");

    sprintf(query, "%s -c 'SELECT * FROM %s WHERE %s in ", connect_command , table->tablename, table->columntrace);

    printf("El query es %s\n", query);

    if(n_traces == 1){
        strcpy(group_of_traces, query);
        sprintf(format_trace, "('%s')'", traces[0]);
        strcat(group_of_traces, format_trace);
    }
    else{
        for(int i = 0; i < n_traces; i++){
            if(i < n_traces-1 && i != 0)
                sprintf(format_trace, "'%s',", traces[i]);
            else
                sprintf(format_trace, "'%s');'", traces[i]);
            if(i == 0) {
                strcpy(group_of_traces, query);
                sprintf(format_trace, "('%s',", traces[i]);
                strcat(group_of_traces, format_trace);
            }
            else
                strcat(group_of_traces, format_trace);
        }
    }

    printf("Grupos de traces para el query %s", group_of_traces);

    for(int i = 0; i < n_traces; i++){
        free(traces[i]);
    }
    free(traces);
    free(group_of_traces);
    for(int i = 0; i < n_tables; i++){
        free(dbs[i]);
        free(tables[i]);
    }
    free(dbs);
    free(tables);
    getchar();

}

void selectAllQuery(struct Database* db, struct Table* table){
    char query[151];
    char connect_command[101];
    connectQuery(db, connect_command);
    sprintf(query, "%s -c 'SELECT * FROM %s;'", connect_command , table->tablename);
    system(query);
}

