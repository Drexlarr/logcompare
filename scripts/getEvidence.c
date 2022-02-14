#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

#define MAX_GWY 10
#define MAX_COPIES 5

#define RED "\e[0;31m"
#define WHT "\e[0;37m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"

FILE *ptr;

char arr_gwy[MAX_GWY][10];
int len_gwy;

char arr_copies[MAX_COPIES][10];
int len_copies;

int system(const char *command);

//Cargamos el archivo de configuración
void loadConfigFile(char *fileName) {
    ptr = fopen(fileName, "r");

    if (NULL == ptr) {
      printf("\n%sERROR:%s Archivo no puede ser abierto (No existe el "
             "directorio o archivo)\n\n",
             RED, WHT);
      exit(1);
    }
}

char *remove_brackets(char *str){
    int i = 0, j = 0;
    while(str[i]){
        if(str[i] != '[' && str[i] != ']')
            str[j++] = str[i];
        i++;
    }
    str[j] = '\0';
    return str;
}

//Función que guarda variables que pueden tener más de un valor: "gwy" y "copyfile"
void getConfigValue(int type_tag, char* string){
    char* token = strtok(string, "=");
    token = strtok(NULL, "=");
    int length_arr = 0;
    for (char *gwy_name = strtok(token, ","); gwy_name != NULL;
         gwy_name = strtok(NULL, ",")) {
        gwy_name[strcspn(gwy_name, "\n")] = 0;
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

char* createDirectory(char* gwy_name){
    char* path = "./evidencias/";
    char* command = malloc(strlen(path)+101);
    char* gwy_name_without_brackets = malloc(strlen(gwy_name)+1);
    char* path_complete = malloc(strlen(path)+50);
    
    strcpy(command, "mkdir -p ");
    strcat(command, path);
    
    strcpy(gwy_name_without_brackets, gwy_name);
    remove_brackets(gwy_name_without_brackets);

    strcpy(path_complete, path);
    strcat(path_complete, gwy_name_without_brackets);

    strcat(command, gwy_name_without_brackets);

    DIR* dir = opendir(path_complete);

    if (dir){
        closedir(dir);
    } else if(ENOENT == errno){
      printf("    %s** Se crea el directorio de evidencias %s**%s\n", YEL,
             gwy_name_without_brackets, WHT);
      system(command);
    }
    return gwy_name_without_brackets;
}

//Función que se encarga de obtener el path de donde se obtendrán las evidencias
//de los gateways y luego, se encarga de copiarlas a la carpeta de evidencias
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
    //IMPORTANTE -mtime 0
    char* first_params = " -maxdepth 1 -type f -mtime 1 | grep -P '(";
    char* second_params = ")'| xargs -I{} cp -n {} ./evidencias/";

    printf("    Extrayendo de: %s\n", path);
    command = malloc(strlen(path)+101);
    strcpy(command, "find ");
    strcat(command, path);
    strcat(command, first_params);

    fgets(line, sizeof(line), ptr);
    getConfigValue(1, line);
    for(int i = 0; i < len_copies; i++){
        printf("    Extrayendo copia: %s\n", arr_copies[i]);

        arr_copies[i][strcspn(arr_copies[i], "\n")] = 0;
        strcat(command, arr_copies[i]);
        strcat(command, second_params);

        char* gwy_wt_brackets = createDirectory(gwy_name);
        strcat(command, gwy_wt_brackets);
        createDirectory(gwy_wt_brackets);

        system(command);
        command[strlen(command) - strlen(arr_copies[i]) -
                strlen(second_params) - strlen(gwy_wt_brackets)] = '\0';
    } 
    printf("\n    EXTRACCIÓN DE %s %sTERMINADA!%s\n", gwy_name, GRN, WHT);
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
        printf("\nGateway: %s\n", arr_gwy[i]);
        retrieveEvidenceGWY(arr_gwy[i]);
    }
}

int main(int argc, char *argv[]) {
    if(argc == 2){
        loadConfigFile(argv[1]);
        readGWYS();
        fclose(ptr);
    }else{
      printf("\n%sERROR:%s Ingrese la ruta en donde se encuentra el archivo de "
             "configuracion\n\n",
             RED, WHT);
    }
    return 0;
}
