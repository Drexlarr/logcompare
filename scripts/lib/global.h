#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>

// Variable que contiene el prefijo del Test Case a renombrar
extern std::string prefix;

// Fecha que se obtiene después de que se lanzó la trama
extern std::string date_after_launch;

// Variable que contiene la ruta del six
extern std::string sixdir;

// Variable que contiene la ruta al directorio de evidencia
extern std::string path_evidence;

// Variable que contiene la ruta al directorio de files
extern std::string path_validate;

// Variable que contiene la ruta de los archivos .cfg
extern std::string path_cfg;

// Variable que contiene la ruta y nombre del archivo cfg log
extern std::string cfg_log;

// variable que contiene la ruta y nombre del archivo cfg bd
extern std::string cfg_bd;

// variable que almacena la desicion del usuario sobre si recolectar el log opcional
// (0: No se hará recolección - 1: Se hará recolección)
extern int optional_log;

#endif  // !GLOBAL_H
