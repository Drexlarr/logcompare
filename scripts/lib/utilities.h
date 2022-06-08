#ifndef MY_UTILITIES_H
#define MY_UTILITIES_H

#include <string>

// Función que obtiene la fecha actual con formato YYYY-MM-DD HH:mm:ss
const std::string currentDateTime();

// Función que obtiene el ddl actual 
const std::string ddlFormat();

// Variable que contiene el prefijo del Test Case a renombrar
extern std::string prefix;

// Fecha que se obtiene después de que se lanzó la trama
extern std::string date_after_launch;

// Variable que contiene la ruta del six
extern std::string sixdir;

// Variable que contiene la ruta al directorio de evidencia
extern std::string path_evidence;

// Variable que contiene la ruta del archivo de cfg
extern std::string path_cfg;

// Función que obtiene la ruta del six
bool getenv(const char* name, std::string &env);

#endif
