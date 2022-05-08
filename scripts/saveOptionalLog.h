#ifndef SAVE_OPTIONAL_LOG_H
#define SAVE_OPTIONAL_LOG_H

#include "utilities.h"
#include <string>

// Función que lee y obtiene los datos del archivo de configuración
int readConfigFile(std::string config_path);

// Función que obtiene los Logs opcionales si el usuario lo desea
int getOptionalLog();

#endif
