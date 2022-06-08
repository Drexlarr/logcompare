#ifndef SAVE_OPTIONAL_LOG_H
#define SAVE_OPTIONAL_LOG_H

#include "utilities.h"

// Función que obtiene la informacion del archivo de configuración
int readConfigFile(std::ifstream &config_file);

// Función que lee el archivo de configuración
int openConfigFile(std::string config_path);

// Función que obtiene los Logs opcionales si el usuario lo desea
int getOptionalLog();

#endif
