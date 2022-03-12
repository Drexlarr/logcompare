#include <iostream>
#include <ctime>
#include <filesystem>
#include <string>
#include <fstream>

namespace fs = std::filesystem;

#define RED "\e[0;31m"
#define WHT "\e[0;37m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"

const char* env_p = std::getenv("HOME");

std::fstream myFile;

int system(const char *__command);

// Crea la carpeta que contendrá toda la evidencia
void createEvidenceDir(){
    system("rm -rf ./tmp/evidencia");
    printf("%s- Se crea el directorio evidencia%s ./tmp/evidencia\n", YEL, WHT);
    // borrar el -p
    system("mkdir -p ./tmp/evidencia");
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

void findLogs(std::string path){
    //parametro
    std::string launch_time = "2022-03-06 21:45:00";
    //std::string launch_time = "2022-03-07 12:45:00";
    std::string time_param = currentDateTime();

    std::string command = "find ./log -maxdepth 1 -type f -newermt \"" +
                          launch_time + "\" -not -newermt \"" + time_param +
                          "\"|sed 's/\\.\\/log\\///g'|grep ^log|xargs -I{} cp "
                          "./log/{} ./tmp/evidencia";
    const char *command_char = command.c_str();
    system(command_char);
}

void getMainLog(std::string prefix){
    printf("\n- Recolectando el log del proceso...\n\n");
    std::string path = "./tmp/evidencia";
    const char* command_char;
    /*Proceso de búsqueda*/
    findLogs(path);

    if(!fs::is_empty(path)){
        std::cout << "  Seleccione el número del archivo correspondiente a guardar:\n";
        int cnt = 1;
        for(const auto & entry : fs::directory_iterator(path)){
            std::string filename = entry.path().filename();
            std::cout << "    [" << cnt++ << "] " << filename << " " << std::endl;
        }
        cnt--;
        int number = 0;
        do{
            std::cout << "  Opción: ";
            std::cin >> number;
        }while(number > cnt || number < 1);
        
        int i = 1;
        for(const auto & entry : fs::directory_iterator(path)){
            std::string filename = entry.path().filename();
            std::string delete_file = "rm ./tmp/evidencia/";
            if(i++ != number){
                delete_file += filename;
                command_char = delete_file.c_str();
                system(command_char);
            } 
        }
        std::string change_name = "ls ./tmp/evidencia | xargs -I {} mv "
                                  "./tmp/evidencia/{} ./tmp/evidencia/" +
                                  prefix + "\\ LOG-SIX.log";
        command_char = change_name.c_str();
        system(command_char);
        printf("\n- Log principal recuperado %sexitosamente%s\n",GRN, WHT);
    }else{
        printf("  %sERROR%s No se encontraron logs\n\n", RED, WHT);
        exit(1);
    }
}

void getDdlLog(){
    printf("\n- Recolectando el ddl...\n\n");
    std::string path = "./log/ddl/";
    std::string ddl_file = path + ddlFormat();

    if(fs::exists(ddl_file)){
        std::string command = "cp " + ddl_file + " ./tmp/evidencia";
        const char* command_char = command.c_str();
        system(command_char);
        printf("- Log del DDL recuperado %sexitosamente%s\n\n",GRN, WHT);
        std::ofstream ofs;
        ofs.open(ddl_file, std::ofstream::out | std::ofstream::trunc);
        ofs.close();
        printf("- Log del DDL %slimpiado%s\n\n",GRN, WHT);
    }else{
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

void getMultipleIdentifiers(std::string ids, std::string path,
                            std::string launch_time, std::string time_param) {
  std::string delimiter = ",";
  size_t pos = 0;
  const char *command_char;
  while ((pos = ids.find(delimiter)) != std::string::npos) {
    std::string command = "find " + path + " -maxdepth 1 -type f -newermt \"" +
                          launch_time + "\" -not -newermt \"" + time_param +
                          "\" |grep " + ids.substr(0, pos) +
                          " |xargs -I{} cp "
                          "{} ./tmp/evidencia";
    command_char = command.c_str();
    system(command_char);
    ids.erase(0, pos + delimiter.length());
  }
  std::string command = "find " + path + " -maxdepth 1 -type f -newermt \"" +
                        launch_time + "\" -not -newermt \"" + time_param +
                        "\" |grep " + ids +
                        " |xargs -I{} cp "
                        "{} ./tmp/evidencia";
  command_char = command.c_str();
  system(command_char);
}

void getOptionalLogs(std::string opt_tag){
    if(myFile.is_open()){
        std::string line;
        std::string path;
        std::string identifiers;
        //param
        std::string launch_time = "2022-02-06 18:00:00";
        //std::string launch_time = "2022-03-07 12:45:00";
        std::string time_param = currentDateTime();
        while(std::getline(myFile, line)){
            // Evitemos que tome evidencia y el main tag que ya fue leído
            if(line[0] != '#' && line.find("tags") == std::string::npos){
                if(line.find(opt_tag) != std::string::npos){
                    std::getline(myFile, line);
                    path = getValuebyLine(line);
                    std::getline(myFile, line);
                    identifiers = getValuebyLine(line);
                    if(path.empty() || identifiers.empty()){
                      std::cout << "  " << RED << "ERROR" << WHT
                                << " Archivo de configuración incompleto"
                                << std::endl;
                      myFile.close();
                      exit(1);
                    }
                    getMultipleIdentifiers(identifiers, path, launch_time,
                                           time_param);
                }
            }
        }
    }
    // Reseteamos la lectura
    myFile.clear();
    myFile.seekg(0);
}

void getMultipleTag(std::string string_to_split){
    std::string delimiter = ",";
    size_t pos = 0;
    while((pos = string_to_split.find(delimiter)) != std::string::npos){
        getOptionalLogs(string_to_split.substr(0, pos));
        string_to_split.erase(0, pos + delimiter.length());
    }
    getOptionalLogs(string_to_split);
}

std::string getTagConfig(std::string tag){
    if(myFile.is_open()){
        std::string line;
        while(std::getline(myFile, line)){
            if(line[0] != '#'){
                if(line.find(tag) != line.npos){
                    std::getline(myFile, line);
                    return getValuebyLine(line);
                }
            }
        }
    }
    printf("  %sERROR%s No se encontró los tags necesarios\n\n", RED, WHT);
    myFile.close();
    exit(1);

}

void readConfigFile(){
    // Get main Tag values
    std::string main_values = getTagConfig("[MAIN_TAG]");
    std::cout << "- Recuperando logs de: "<< YEL << main_values << WHT << "\n\n";
    getMultipleTag(main_values);
    std::cout << "- Logs alternativos recuperados " << GRN
                << "exitosamente" << WHT << std::endl;
    // Limpiar archivos
}

void getGatewayLog(std::string filename){
    printf("- Leyendo archivo de configuración...\n\n");
    myFile.open(filename, std::ios::in);
   if(!myFile){
        printf("  %sERROR%s No se encontró el archivo de configuración\n\n", RED, WHT);
        myFile.close();
        exit(1);
   }else{
       readConfigFile();
   }
    myFile.close();
}

void interactive(std::string filename, std::string prefix){
    printf("\nRECOLECTANDO EVIDENCIA\n\n");
    createEvidenceDir();
    getMainLog(prefix);
    getDdlLog();
    getGatewayLog(filename);
    std::cout << "\nRECOLECCIÓN GUARDADA EN: " << env_p << "/tmp/evidencia\n\n" ;
}

int main(int argc, char* argv[]){
    if(argc != 3)
      printf("\n%sERROR%s Uso: ./getLogs [1] [2]\n  1) Ubicación del "
             "archivo de configuración\n  2) Prefijo (PEFTCMP-XXXX)\n\n",
             RED, WHT);
    else {
      interactive(argv[1], argv[2]);
    }
    return 0;
}
