#include <iostream>
#include <ctime>
#include <fstream>

#include "../lib/saveEvidence.h"
#include "../lib/values.h"
#include "../lib/saveOptionalLog.h"
#include "../lib/global.h"

using namespace std;

int createDirEvidence(){
    printf("- Se ha creado la carpeta en: %s%s%s\n", CYN, path_evidence.c_str(), WHT);

    // Borramos la evidencia anterior
    string command = "rm -rf " + path_evidence;
    system(command.c_str());

    // Creamos la nueva carpeta evidencia
    command = "mkdir " + path_evidence;
    return system(command.c_str());
}

int renameLogSix(string prefix){
    string mensaje = "\n- Se renombra el Log del SIX a: " + prefix + " LOG-SIX.log\n";
    printf("%s", mensaje.c_str());
    string command = "ls " + path_evidence + " | xargs -I{} mv " +
                     path_evidence + "/{} \"" + path_evidence +
                     "/" + prefix + " LOG-SIX.log\"";
    return system(command.c_str());
}

int deleteAnotherLogs(){
    // Realizamos diferentes acciones dependiendo la cantidad de 
    // Log que encuentre
    fstream outputFile;
    outputFile.open("./.outputFile.txt", ios::in);
    if(outputFile.is_open()){
        string count;
        getline(outputFile, count);
        outputFile.close();
        system("rm ./.outputFile.txt");

        int count_files = stoi(count);
        if (count_files <= 1){
            switch (count_files){
                case 0:
                    // Si no encuentra nada retorna un error
                    return BAD;
                    break;
                case 1:
                    // Si encuentra solo 1 no sigue con el flujo
                    printf("- Guardando el log:\n\n");
                    string command = "ls -1 " + path_evidence;
                    system(command.c_str());
                    return OK;
                    break;
            }
        }
    } else {
      printf("%sERROR%s No se pudo abrir el archivo de salida "
             "\".outputFile.txt\"\n",
             RED, WHT);
      outputFile.close();
      return BAD;
    }

    printf("- Elija el archivo que quiere guardar:\n\n");

    string command = "ls -1 " + path_evidence;
    system(command.c_str());

    printf("\n");
    string log_name = "";
    
    do {
        printf("Ingrese el nombre del archivo elegido: ");
        cin >> log_name;
    
        command = "if [ -e \"" + path_evidence + "/" + log_name +
                  "\" ]; then exit 0; else exit 1; fi";
    
    } while(system(command.c_str()));

    // Elimina todos los archivos que no sean el elegido
    command = "find " + path_evidence + "/ -type f -not -name \"" +
              log_name + "\" -delete";
    return system(command.c_str());
}

int getLogSix(){
    printf("- Recolectando el Log del SIX\n");

    string command =
        "find " + sixdir +
        "/log -maxdepth 1 -type f -newermt \"" +
        date_after_launch + "\" -not -newermt \"" + currentDateTime() +
        "\"|grep -v six | awk -F '/' '{print $(NF)}' | xargs -I {} cp " + 
        sixdir + "/log/{} " + path_evidence +
        " && ls -1 " + path_evidence + " | wc -l > ./.outputFile.txt";

    if (!system(command.c_str())) {
      if (!deleteAnotherLogs()) {
        int rsp = renameLogSix(prefix);
        printf("- Se ha recuperado el Log del SIX %sexitosamente%s\n", GRN, WHT);
        return rsp;
      }
    }
    
    printf("\n  %sERROR%s al capturar el Log del SIX\n", RED, WHT);
    return BAD;
}

int getLogDDL(){
    printf("- Recolectando el log del ddl\n");
    string ddl_file = sixdir + "/log/ddl/" + ddlFormat();

    // Verificamos si exsite el archivo DDL
    string command = "if [ -e " + ddl_file + " ]; then exit 0; else exit 1; fi";

    // Si existe el ddl
    if (!system(command.c_str())) {
      // Copiamos el DDL a nuestra carpeta de evidencia
      command =
          "cp " + ddl_file + " " + path_evidence + "/" + prefix + "\\ DDL.log";

      if (!system(command.c_str())) {
        printf("- Log del DDL recuperado %sexitosamente%s\n", GRN, WHT);

        // Limpiamos el DDL
        ofstream ofs;
        ofs.open(ddl_file, ofstream::out | std::ofstream::trunc);
        ofs.close();
        printf("- Log del DDL %slimpiado%s\n", YEL, WHT);

        return OK;
      }
    }

    printf("\n  %sERROR%s al capturar el DDL\n", RED, WHT);
    return BAD;
}

int process(){
    printf("\nRECOLECTANDO EVIDENCIA\n\n");
    
    if (!createDirEvidence()) {
      if (!getLogSix()) {
        if(!getLogDDL()){
            if(optional_log){
                if(!loadOptionalLog()){
                    return OK;
                }else return BAD;
            }
            return OK;
        }
      }
    }

    return BAD;
}
