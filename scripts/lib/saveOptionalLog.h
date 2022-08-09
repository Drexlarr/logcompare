#ifndef SAVE_OPTIONAL_LOG_H
#define SAVE_OPTIONAL_LOG_H

#include "utilities.h"
#include <string>
#include <vector>

struct configData {
    std::string main_tag;
    std::string path;
    std::vector<std::string> copies;
    int number_line = 0;

};

// Función que obtiene el Main Tag del archivo de configuración
int readMainTag(std::ifstream &config_file);

// Función que obtiene la ruta de los archivos opcionales
int readOptionalPath(std::ifstream &config_file);

// Función que guarda las copias de los archivos de opcionales
int readCopies(std::ifstream &config_file);

// Función que limpia el contenido de los archivos listados en ./ouputFile.txt
int truncFile();

// Función que obtiene la informacion del archivo de configuración
int readConfigFile(std::ifstream &config_file);

// Función que escribe en ./outputFile.txt la ruta de los archivos opcionales
int saveOptinalLog();

// Función que copia los archivos opcionales listados en ./outputFile.txt al directorio
// de evidencia
int loadOptionalLog();

// Función que realiza la obtención de los archivos opcionales a través de las funciones
// antes mencionadas
int readConfigFile(std::ifstream &config_file);

// Función que lee el archivo de configuración
int openConfigFile(std::string config_path);

// Función que obtiene los Logs opcionales si el usuario lo desea
int getOptionalLog();

#endif
