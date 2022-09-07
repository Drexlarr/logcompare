#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

#include "../lib/saveOptionalLog.h"

#include "../lib/utilities.h"
#include "../lib/values.h"
#include "../lib/global.h"

using namespace std;

configData cfg_data;

int readMainTag(ifstream &config_file){
    string line;
    size_t pos = string::npos;
    while(getline(config_file, line)){
        pos = line.find("tags=");
        if(line[0] != '#' && pos != line.npos){
            cfg_data.main_tag = line.substr(pos+5, line.npos);
            return OK;
        }
        cfg_data.number_line = config_file.tellg();
    }
    return BAD;
}

int readOptionalPath(ifstream &config_file){
    string line;
    bool search_path = false;
    size_t tag_pos = string::npos;
    size_t pos = string::npos;

    while(getline(config_file, line)){
        // Busca el log opcional hasta encontrarlo
        if (!search_path){
            tag_pos = line.find(cfg_data.main_tag);
            cfg_data.number_line = config_file.tellg();
        }

        // Busca la posición del path si encontró el tag de log opcional
        if (search_path) pos = line.find("path=");

        // Busca el path del log opcional
        if (search_path && line[0] != '#' && pos != line.npos){
            cfg_data.path = line.substr(pos+5, line.npos);
            return OK;
        }
        // Si no encuentra PATH hasta llegar a otro posible tag retorna BAD
        if (line[0] == '[' && search_path) return BAD;

        // Si encuentra el tag del log opcional, habilita la bandera para
        // buscar el PATH
        if (!search_path && line[0] != '#' && tag_pos != line.npos){
            search_path = true;
        }
    }
    return BAD;
}

int readCopies(ifstream &config_file){
    string line;

    bool search_copies = false;
    size_t pos = string::npos;

    config_file.seekg(cfg_data.number_line);

    while(getline(config_file, line)){
        pos = line.find("copyfile=");

        if (line[0] != '#' && pos != line.npos){
			string values = line.substr(pos + 9, line.npos);
			size_t pos_delimiter = values.find(",");

			while (pos_delimiter != values.npos){
				cfg_data.copies.push_back(values.substr(0, pos_delimiter));
				values = values.substr(pos_delimiter + 1, values.npos);
				pos_delimiter = values.find(",");
			}
			cfg_data.copies.push_back(values);
			return OK;
		}
		if (line[0] == '[') return BAD;
	}
	return BAD;
}

int truncFile(){
	fstream outputFile;
	outputFile.open("./.optionalFiles.txt", ios::in);

	if (outputFile.is_open()){
		string file;
		while (getline(outputFile, file)){
			ofstream ofs;
            ofs.open(sixdir + "/" + cfg_data.path + "/" + file,
                    ofstream::out | std::ofstream::trunc);
			ofs.close();
		}
	} else return BAD;

    outputFile.close();
	printf("- Log Opcional %slimpiado%s\n", YEL, WHT);
    return OK;
}

int saveOptinalLog(){
	string command = "";
    int result = BAD;
	printf("- Recolectando copias\n");

	for(int i = 0; i < cfg_data.copies.size(); i++){
		// Guarda el nombre de los archivos en un texto plano
		command = "find " + sixdir + "/" + cfg_data.path +
			" -maxdepth 1 -type f -printf \"%f\\n\" | grep " +
			cfg_data.copies[i] + " > ./.optionalFiles.txt";

		result = system(command.c_str());	
	}

    truncFile();

	return result;
}

int loadOptionalLog(){
	// Copiamos los archivos al directorio de evidencia
	string command = "cat ./.optionalFiles.txt | xargs -I{} cp " +
        sixdir + "/" + cfg_data.path + "/{} " + path_evidence + "/" + prefix + "-{}";

	if (!system(command.c_str())){
        truncFile();
		command = "rm ./.optionalFiles.txt";
		return system(command.c_str());
	}

    return BAD;
}

int readConfigFile(ifstream &config_file){
	if (!readMainTag(config_file)) {
		printf("- Log a recuperar: %s\n", cfg_data.main_tag.c_str());

		if (!readOptionalPath(config_file)) {
			printf("- Ubicacion del log a recuperar: %s\n", cfg_data.path.c_str());

			if(!readCopies(config_file)){
				printf("- Copias a recuperar: ");

				for(int i = 0; i < cfg_data.copies.size() - 1; i++){
					printf("%s - ", cfg_data.copies[i].c_str());
				}

				printf("%s\n", cfg_data.copies.rbegin()->c_str());
				saveOptinalLog();
				return OK;
      } else {
        printf("\n%sERROR%s no se encontro las copias a recuperar\n", RED, WHT);
      }

    } else {
      printf("\n%sERROR%s no se encontro el path del log\n", RED, WHT);
    }
  } else {
    printf("\n%sERROR%s no se encontro el tag principal\n", RED, WHT);
  }
  config_file.close();
  return BAD;
}

int openConfigFile() {
  ifstream config_file(cfg_log);

  if (config_file.fail()) {
    printf("%sERROR%s No se encontró el archivo de configuracion\n\n", RED,
           WHT);
  } else {
    printf("- Se leyo el archivo de configuracion %sexitosamente\n%s", GRN,
           WHT);
    return readConfigFile(config_file);
  }

  config_file.close();
  return BAD;
}

int getOptionalLog() {
  string option;
  do {
    printf("- Desea obtener logs opcionales [y/n]: ");
    cin >> option;
  } while (option != "Y" && option != "y" && option != "n" && option != "N");

  if (option != "Y" && option != "y") {
      optional_log = 0;
    return OK;
  }

  printf("- Listando Logs opcionales\n");

  optional_log = 1;
  return openConfigFile();
}
