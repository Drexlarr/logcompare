#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_GWY 10
#define MAX_COPIES 5

FILE *ptr;

char arr_gwy[MAX_GWY][10];
int len_gwy;

char arr_copies[MAX_COPIES][10];
int len_copies;

int system(const char *command);

//Cargamos el archivo de configuración
void loadConfigFile(char *fileName) {
    ptr = fopen("../cfgfiles/compar_finan.cfg", "r");
    //ptr = fopen(fileName, "r");

    if (NULL == ptr) {
        printf("file can't be opened\n");
    }
}

//Función que guarda variables que pueden tener más de un valor: "gwy" y "copyfile"
void getConfigValue(int type_tag, char* string){
    char* token = strtok(string, "=");
    token = strtok(NULL, "=");
    int length_arr = 0;
    for (char *gwy_name = strtok(token, ","); gwy_name != NULL;
         gwy_name = strtok(NULL, ",")) {
        if (!type_tag)
            strcpy(arr_gwy[length_arr++], gwy_name);
        else
            strcpy(arr_copies[length_arr++], gwy_name);
    }
    if(!type_tag)
        len_gwy = length_arr;
    else
        len_copies = length_arr;
}

void retrieveEvidenceGWY(char* gwy_name){
    char line[100];

    while(fgets(line,sizeof(line),ptr)){
        if(strstr(line, gwy_name) != NULL)
            break;
    }
    
    fgets(line, sizeof(line), ptr);
    char* path = strtok(line, "=");
    path = strtok(NULL, "=");
    path[strcspn(path, "\n")] = 0;

    //Construyendo el comando
    char* command;
    char* find = "find ";
    char* paramemters = " -maxdepth 1 -type f -mtime 0|grep -P '(";
    command = malloc(strlen(path)+1+50);
    strcpy(command, find);
    strcat(command, path);
    strcat(command, paramemters);

    fgets(line, sizeof(line), ptr);
    getConfigValue(1, line);
    for(int i = 0; i < len_copies; i++){
        printf("Copiado: %s\n", arr_copies[i]);
        arr_copies[i][strcspn(arr_copies[i], "\n")] = 0;
        strcat(command, arr_copies[i]);
        strcat(command, ")'");
        //printf("%s\n", command);
        system(command);
        command[strlen(command)-5] = '\0';
        //[TODO]
        //Copiar los archivos al directorio de evidencias
    } 
}

//Función que encuentra y obtiene los valores del tag GWY_LOGS
void readGWYS(){
    char* gwy = "[GWY_LOGS]";
    char retrieve_gwy[100];

    while(fgets(retrieve_gwy,sizeof(retrieve_gwy),ptr)){
        if(strstr(retrieve_gwy, gwy) != NULL)
            break;
    }

    fgets(retrieve_gwy,sizeof(retrieve_gwy), ptr);
    getConfigValue(0, retrieve_gwy);

    for(int i = 0; i < len_gwy; i++){ 
        printf("Gateway: %s\n", arr_gwy[i]);
        retrieveEvidenceGWY(arr_gwy[i]);
    }
}


int main() {
    loadConfigFile("hola");
    readGWYS();
    fclose(ptr);
    return 0;
}
