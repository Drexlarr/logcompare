#include <iostream>
#include <ctime>
#include <filesystem>
#include <chrono>
#include <string>

namespace fs = std::filesystem;

#define RED "\e[0;31m"
#define WHT "\e[0;37m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"

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

void findLogs(std::string path){
    //parametro
    std::string launch_time = "2022-03-06 21:45:00";
    std::string time_param = currentDateTime();

    std::string command = "find ./log -maxdepth 1 -type f -newermt \"" +
                          launch_time + "\" -not -newermt \"" + time_param +
                          "\"|sed 's/\\.\\/log\\///g'|grep ^log|xargs -I{} cp "
                          "./log/{} ./tmp/evidencia";
    const char *command_char = command.c_str();
    system(command_char);
}

void getMainLog(){
    printf("\n- Recolectando el log del proceso...\n\n");
    std::string path = "./tmp/evidencia";
    const char* command_char;
    /*Proceso de búsqueda*/
    findLogs(path);

    //[TODO]
    // Si hay más de un archivo en evidencias
    // Entonces iterarlos y preguntarle al usuario con cual quedarse
    // Sino, muestra que archivo se guardó y pasa al siguiente

    std::cout << "  Seleccione el número del archivo correspondiente a guardar:\n";
    int i = 1;
    for(const auto & entry : fs::directory_iterator(path)){
        std::string filename = entry.path().filename();
        std::cout << "    [" << i++ << "] " << filename << " " << std::endl;
    }
    int number;
    std::cout << " Opción: ";
    std::cin >> number;
    
    i = 1;
    for(const auto & entry : fs::directory_iterator(path)){
        std::string filename = entry.path().filename();
        std::string delete_file = "rm ./tmp/evidencia/";
        if(i++ != number){
            delete_file += filename;
            command_char = delete_file.c_str();
            system(command_char);
        } 
    }
    printf("- Log principal recuperado %sexitosamente%s\n",GRN, WHT);
}

void getDdlLog(){}

void getGatewayLog(){
}

void interactive(){
    printf("\nRECOLECTANDO EVIDENCIA\n\n");
    createEvidenceDir();
    getMainLog();

}

int main(){
    interactive();
    return 0;
}
