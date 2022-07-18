#ifndef SAVE_EVIDENCE_H
#define SAVE_EVIDENCE_H

int system(const char *__command);

// Función que crea el directorio donde se guardará la evidencia
int createDirEvidence();

// Función que renombra al log del six con el prefijo dado
int renameLogSix();

// Función que guarda el Log del SIX que se indique
int deleteAnotherLogs();

// Función que recolecta el Log del SIX como evidencia
int getLogSix();

// Función que recolecta el Log del DDL como evidencia
int getLogDDL();

// Función que controla el flujo de la recolección
int process();

#endif
