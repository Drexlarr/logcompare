#include <iostream>
#include <ctime>
#include <string>
#include <fstream>

#define RED "\e[0;31m"
#define WHT "\e[0;37m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"

const std::string env_p = std::getenv("HOME");

std::fstream configFile;
std::fstream outputFile;

std::string main_log_filename;

int system(const char *__command);

// Crea la carpeta que contendrá toda la evidencia
void createEvidenceDir(){
    system("rm -rf $HOME/tmp/evidencia");
    printf("%s- Se crea el directorio evidencia%s $HOME/tmp/evidencia\n", YEL, WHT);
    system("mkdir $HOME/tmp/evidencia");
}

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
}

const std::string ddlFormat() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "ddl%Y%m%d.log", &tstruct);

    return buf;
}

void findLogs(std::string path, std::string launch_time){
    std::string time_param = currentDateTime();

    std::string command = "find $HOME/log -maxdepth 1 -type f -printf \"\%f\\n\" -newermt \"" +
                          launch_time + "\" -not -newermt \"" + time_param +
                          "\"|grep ^log|xargs -I{} cp "
                          "$HOME/log/{} $HOME/tmp/evidencia";
    const char *command_char = command.c_str();
    system(command_char);
}

int isDirectoryEmpty(std::string path){
    std::string command =
        "if [ -z \"$(ls -A " + path +
        " )\" ]; then echo \"1\" > ./.outputFile.txt; else echo \"0\" > ./.outputFile.txt; fi";
    system(command.c_str());
    outputFile.open("./.outputFile.txt", std::ios::in);
    if(outputFile.is_open()){
        std::string is_empty;
        std::getline(outputFile, is_empty);
        outputFile.close();
        system("rm ./.outputFile.txt");
        return std::stoi(is_empty);
    }else{
        printf(" %sERROR%s No se pudo abrir el archivo de salida \".outputFile\"\n", RED, WHT);
        outputFile.close();
        exit(1);
    }
}

void getMainLogName(std::string path){
    std::string command = "ls -A " + path + " > ./.outputFile.txt";
    system(command.c_str());
    outputFile.open("./.outputFile.txt", std::ios::in);
    if(outputFile.is_open()){
        getline(outputFile, main_log_filename);
        std::cout << main_log_filename << std::endl;
        outputFile.close();
        system("rm ./.outputFile.txt");
    }else{
        printf(" %sERROR%s No se pudo abrir el archivo de salida \".outputFile\"\n", RED, WHT);
        outputFile.close();
        exit(1);
    }
}

void getMainLog(std::string prefix, std::string launch_time){
    printf("\n- Recolectando el log del proceso...\n\n");
    std::string path = env_p + "/tmp/evidencia";
    const char* command_char;
    /*Proceso de búsqueda*/
    findLogs(path, launch_time);
    if(!isDirectoryEmpty(path)){
        std::cout << "  Seleccione el número del archivo correspondiente a guardar:\n";
        int cnt = 1;
        std::string command = "ls -A -x " + path + " > ./.outputFile.txt";
        system(command.c_str());
        outputFile.open("./.outputFile.txt", std::ios::in);
        if(outputFile.is_open()){
            std::string filename;
            while(getline(outputFile, filename)){
                std::cout << "    [" << cnt++ << "] " << filename << " " << std::endl; 
            }
            cnt--;
            int number = 0;
            do{
                std::cout << "  Opción: ";
                std::cin >> number;
            }while(number > cnt || number < 1);
            
            int i = 1;
            while(getline(outputFile, filename)){    
                std::string delete_file = "rm $HOME/tmp/evidencia/";
                if(i++ != number){
                    delete_file += filename;
                    command_char = delete_file.c_str();
                    system(command_char);
                } 
            }
            std::string change_name = "ls $HOME/tmp/evidencia | xargs -I {} mv "
                                      "$HOME/tmp/evidencia/{} $HOME/tmp/evidencia/" +
                                      prefix + "\\ LOG-SIX.log";
            command_char = change_name.c_str();
            system(command_char);
            outputFile.close();
            system("rm ./.outputFile.txt");
            getMainLogName(path);
            printf("\n- Log principal recuperado %sexitosamente%s\n",GRN, WHT);
        }else{
            printf(" %sERROR%s No se pudo abrir el archivo de salida \".outputFile\"\n", RED, WHT);
            outputFile.close();
            exit(1);
        }
    }else{
        printf("  %sERROR%s No se encontraron logs\n\n", RED, WHT);
        exit(1);
    }
}

int existsFile(std::string file_path){
    std::string command =
        "if [ -e " + file_path + " ]; then echo \"1\" > ./.outputFile.txt; else echo "
        "\"0\" > ./.outputFile.txt; fi";
    system(command.c_str());
    outputFile.open("./.outputFile.txt", std::ios::in);
    if(outputFile.is_open()){
        std::string exists;
        std::getline(outputFile, exists);
        outputFile.close();
        system("rm ./.outputFile.txt");
        return std::stoi(exists);
    }else{
        printf(" %sERROR%s No se pudo abrir el archivo de salida \".outputFile\"\n", RED, WHT);
        outputFile.close();
        exit(1);
    }
}

void getDdlLog(std::string prefix){
    printf("\n- Recolectando el ddl...\n\n");
    std::string path = env_p + "/log/ddl/";
    std::string ddl_file = path + ddlFormat();
    if (existsFile(ddl_file)) {
      std::string command =
          "cp " + ddl_file + " $HOME/tmp/evidencia/" + prefix + "\\ DDL.log";
      const char *command_char = command.c_str();
      system(command_char);
      printf("- Log del DDL recuperado %sexitosamente%s\n\n", GRN, WHT);
      std::ofstream ofs;
      ofs.open(ddl_file, std::ofstream::out | std::ofstream::trunc);
      ofs.close();
      printf("- Log del DDL %slimpiado%s\n\n", GRN, WHT);
    }
    else {
      printf("  %sERROR%s No se encontró el DDL\n\n", RED, WHT);
      exit(1);
    }
}

std::string getValuebyLine(std::string line){
    std::string delimiter = "=";
    size_t pos = 0;
    std::string value;
    pos = line.find(delimiter);
    value = line.substr(pos+1, std::string::npos);
    return value;
}

void deleteOptionalLogsContent(std::string path, std::string launch_time,
                               std::string time_param, std::string id) {
    const char *command_char;
    std::string command =
        "find " + path + " -maxdepth 1 -type f -newermt \"" +
        launch_time + "\" -not -newermt \"" + time_param + "\" |grep " +
        id + " |xargs -I{} cp /dev/null {}";
    command_char = command.c_str();
    system(command_char); 
}

void getMultipleIdentifiers(std::string ids, std::string path,
                            std::string launch_time, std::string time_param,
                            std::string prefix) {
  std::string delimiter = ",";
  size_t pos = 0;
  const char *command_char;
  while ((pos = ids.find(delimiter)) != std::string::npos) {
    std::string command =
        "find " + path + " -maxdepth 1 -type f -printf \"\%f\\n\" -newermt \"" +
        launch_time + "\" -not -newermt \"" + time_param + "\" |grep " +
        ids.substr(0, pos) + " |xargs -I{} cp " + path +
        "/{} $HOME/tmp/evidencia/" + prefix + "-{}";
    command_char = command.c_str();
    system(command_char);
    deleteOptionalLogsContent(path, launch_time, time_param, ids.substr(0, pos));
    ids.erase(0, pos + delimiter.length());
  }
    std::string command =
        "find " + path + " -maxdepth 1 -type f -printf \"\%f\\n\" -newermt \"" +
        launch_time + "\" -not -newermt \"" + time_param + "\" |grep " +
        ids.substr(0, pos) + " |xargs -I{} cp " + path +
        "/{} $HOME/tmp/evidencia/" + prefix + "-{}";
  command_char = command.c_str();
  system(command_char);
  deleteOptionalLogsContent(path, launch_time, time_param, ids.substr(0, pos));
}

void getOptionalLogs(std::string opt_tag, std::string launch_time, std::string prefix){
    if(configFile.is_open()){
        std::string line;
        std::string path;
        std::string identifiers;
        std::string time_param = currentDateTime();

        while(std::getline(configFile, line)){
            // Evitemos que tome evidencia y el main tag que ya fue leído
            if(line[0] != '#' && line.find("tags") == std::string::npos){
                if(line.find(opt_tag) != std::string::npos){
                    std::getline(configFile, line);
                    path = getValuebyLine(line);
                    std::getline(configFile, line);
                    identifiers = getValuebyLine(line);
                    if(path.empty() || identifiers.empty()){
                      std::cout << "  " << RED << "ERROR" << WHT
                                << " Archivo de configuración incompleto"
                                << std::endl;
                      configFile.close();
                      exit(1);
                    }
                    getMultipleIdentifiers(identifiers, path, launch_time,
                                           time_param, prefix);
                }
            }
        }
    }
    // Reseteamos la lectura
    configFile.clear();
    configFile.seekg(0);
}

void getMultipleTag(std::string string_to_split, std::string launch_time,
                    std::string prefix) {
  std::string delimiter = ",";
  size_t pos = 0;
  while ((pos = string_to_split.find(delimiter)) != std::string::npos) {
    getOptionalLogs(string_to_split.substr(0, pos), launch_time, prefix);
    string_to_split.erase(0, pos + delimiter.length());
  }
  getOptionalLogs(string_to_split, launch_time, prefix);
}

std::string getTagConfig(std::string tag){
    if(configFile.is_open()){
        std::string line;
        while(std::getline(configFile, line)){
            if(line[0] != '#'){
                if(line.find(tag) != line.npos){
                    std::getline(configFile, line);
                    return getValuebyLine(line);
                }
            }
        }
    }
    return "";
}

void readConfigFile(std::string launch_time, std::string prefix){
    // Get main Tag values
    std::string main_values = getTagConfig("[MAIN_TAG]");
    if(main_values == ""){
        printf("-  %sNo se recolectará%s logs opcionales\n", YEL, WHT);
        return;
    }
    std::cout << "- Recuperando logs de: "<< YEL << main_values << WHT << "\n\n";
    getMultipleTag(main_values, launch_time, prefix);
    std::cout << "- Logs opcionales recuperados " << GRN
                << "exitosamente" << WHT << std::endl;
    // Limpiar archivos
    std::cout << "\n- Logs opcionales " << GRN
                << "limpiados" << WHT << std::endl;
}

void getGatewayLog(std::string filename, std::string launch_time, std::string prefix){
    printf("- Leyendo archivo de configuración...\n\n");
    configFile.open(filename, std::ios::in);
   if(!configFile){
        printf("  %sERROR%s No se encontró el archivo de configuración\n\n", RED, WHT);
        configFile.close();
        exit(1);
   }else{
       readConfigFile(launch_time, prefix);
   }
    configFile.close();
}

void interactive(std::string filename, std::string prefix, std::string launch_time){
    printf("\nRECOLECTANDO EVIDENCIA\n\n");
    createEvidenceDir();
    getMainLog(prefix, launch_time);
    getDdlLog(prefix);
    getGatewayLog(filename, launch_time, prefix);
    std::cout << "\n" << GRN << "COMPLETADO " << WHT << "RECOLECCIÓN GUARDADA EN: " << env_p << "/tmp/evidencia\n\n" ;
}

int main(int argc, char* argv[]){
    if(argc != 4)
      printf("\n%sERROR%s Uso: ./getLogs [1] [2] [3]\n  1) Ubicación del "
             "archivo de configuración\n  2) Prefijo (PEFTCMP-XXXX)\n  3) Fecha "
             "antes del lanzamiento (YYYY-mm-dd HH:MM:SS)\n\n",
             RED, WHT);
    else {
      interactive(argv[1], argv[2], argv[3]);
    }
    return 0;
}
