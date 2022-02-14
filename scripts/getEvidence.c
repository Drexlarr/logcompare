#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COPIES 5

#define RED "\e[0;31m"
#define WHT "\e[0;37m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"

FILE *ptr;

char gwy_picked[10];
char gwy_prefix[20];

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

//Función que guarda variables que pueden tener más de un valor: "copyfile"
void getConfigValue(char* string){
    char* token = strtok(string, "=");
    token = strtok(NULL, "=");
    int length_arr = 0;
    for (char *copies = strtok(token, ","); copies != NULL;
         copies = strtok(NULL, ",")) {
        copies[strcspn(copies, "\n")] = 0;
        strcpy(arr_copies[length_arr++], copies);
    }
    len_copies = length_arr;
}

//Crea el directorio de evidencias si es que este ya no está creado
void createDirectory(){
    char* path = "./evidencias";
    char* command = "mkdir ./evidencias/";
    system("rm -rf ./evidencias");

    printf("    %s** Se crea el directorio de evidencias **%s\n", YEL, WHT);
    system(command);
}

//Añade el prefijo introducido por el usuario a los archivos recolectados
void addingPrefix(){
    char* incomplete = "ls ./evidencias | xargs -I {} mv ./evidencias/{} ./evidencias/";
    char* complete = malloc(strlen(incomplete)+21);
    strcpy(complete, incomplete);
    strcat(complete, gwy_prefix);
    strcat(complete, "-{}");
    system(complete);
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
    char route[100];
    strcpy(route, path);

    //Construyendo el comando
    char* command;
    //IMPORTANTE -mtime 0
    char* first_params = " -maxdepth 1 -type f -mtime 0 | grep -P '(";
    char* second_params = ")'| xargs -I{} cp -n {} ./evidencias/";
    char* erase_command = ")'| xargs -I{} truncate -s 0 ";

    printf("    Extrayendo de: %s\n", path);
    command = malloc(strlen(path)+151);
    strcpy(command, "find ");
    strcat(command, route);
    strcat(command, first_params);

    fgets(line, sizeof(line), ptr);
    getConfigValue(line);

    createDirectory();
    for(int i = 0; i < len_copies; i++){
        printf("    Extrayendo copia: %s\n", arr_copies[i]);
        //Armando comando para copiar evidencias de una copia
        arr_copies[i][strcspn(arr_copies[i], "\n")] = 0;
        strcat(command, arr_copies[i]);
        strcat(command, second_params);

        //Ejecutando comando de copia
        //printf("    Comando: %s\n\n", command);
        system(command);

        //Restableciendo el comando para borrar el contenido de las copias
        //ya recolectadas
        command[strlen(command) -
                strlen(second_params)] = '\0';

        //Ejecutando comando de borrado de contenido
        strcat(command, erase_command);
        //strcat(command, route);
        strcat(command, "/{}");
        //printf("    Comando: %s\n\n", command);
        system(command);

        //Restableciendo el comando para utilizarlo en otras copias
        command[strlen(command) - strlen(arr_copies[i]) -
                strlen(erase_command) - 3] = '\0';
    } 
    addingPrefix();
    printf("\n    EXTRACCIÓN DE LAS COPIAS DE %s %sTERMINADA!%s\n\n", gwy_name, GRN, WHT);
}

//Función que encuentra y obtiene los valores del tag GWY_LOGS
//Se encarga de llamar a la función que recolecta las evidencia de los GWY
void readGWYS(){
    char* gwy = "[GWY_LOGS]";
    char retrieve_gwy[100];

    while(fgets(retrieve_gwy,sizeof(retrieve_gwy),ptr)){
        if(strstr(retrieve_gwy, gwy) != NULL)
            break;
    }

    fgets(retrieve_gwy,sizeof(retrieve_gwy), ptr);
    strcpy(gwy_picked, strtok(retrieve_gwy, "="));
    strcpy(gwy_picked, strtok(NULL, "=")); 
    gwy_picked[strcspn(gwy_picked, "\n")] = 0;

    printf("\nGateway: %s\n", gwy_picked);
    retrieveEvidenceGWY(gwy_picked);
}

int main(int argc, char *argv[]) {
    //TODO    
    //un prefijo dado por el usuario
    if(argc == 3){
        loadConfigFile(argv[1]);
        if(strlen(argv[2]) < 20){
            strcpy(gwy_prefix,argv[2]);
            readGWYS();
        }
        else{
            printf("\n%sERROR:%s El prefijo es demasiado grande\n\n",RED, WHT);
        }
        fclose(ptr);
    }else{
      printf("\n%sERROR:%s Ingrese la ruta en donde se encuentra el archivo de "
             "configuracion y el prefijo que llevarán los archivos "
             "recolectados\n\n",
             RED, WHT);
    }
    return 0;
}
