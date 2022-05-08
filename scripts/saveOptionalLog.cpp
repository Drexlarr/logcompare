#include <iostream>

#include "saveOptionalLog.h"
#include "values.h"

using namespace std;

int readConfigFile(string config_path){
    printf("%s\nLogica en construccion\n%s", YEL, WHT);
    return OK;
}

int getOptionalLog(){
    // Comenzando Función
    string option;
    do {
        printf("- Desea obtener logs opcionales [y/n]: ");
        cin >> option;
    } while (option != "Y" && option != "y" && option != "n" && option != "N");

    if (option != "Y" && option != "y"){
        return OK;
    }

    string command;
    string cfg_file;

    do {
      printf("- Ingrese el nombre del archivo: ");
      cin >> cfg_file;

      command = "if [ -e " + path_cfg + "/" + cfg_file +
                " ]; then return 0; else return 1; fi";

    } while (cfg_file == "" || system(command.c_str()));

    return readConfigFile(path_cfg + "/" + cfg_file);
}
