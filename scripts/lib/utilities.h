#ifndef MY_UTILITIES_H
#define MY_UTILITIES_H

#include <string>

// Función que obtiene la fecha actual con formato YYYY-MM-DD HH:mm:ss
const std::string currentDateTime();

// Función que obtiene el ddl actual 
const std::string ddlFormat();

// Función que obtiene la ruta del six
bool getMainDir(const char* name);

#endif
