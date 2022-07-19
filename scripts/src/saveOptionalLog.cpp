#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

#include "../lib/saveOptionalLog.h"

#include "../lib/utilities.h"
#include "../lib/values.h"
#include "../lib/global.h"

using namespace std;

struct configData {
    string main_tag;
    string path;
    vector<string> copies;
    int number_line = 0;

    int readMainTag(ifstream &config_file){
        string line;
        size_t pos = string::npos;
        while(getline(config_file, line)){
            pos = line.find("tags=");
            if(line[0] != '#' && pos != line.npos){
                main_tag = line.substr(pos+5, line.npos);
                return OK;
            }
            number_line = config_file.tellg();
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
                tag_pos = line.find(main_tag);
                number_line = config_file.tellg();
            }

            // Busca la posición del path si encontró el tag de log opcional
            if (search_path) pos = line.find("path=");

            // Busca el path del log opcional
            if (search_path && line[0] != '#' && pos != line.npos){
                path = line.substr(pos+5, line.npos);
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

        config_file.seekg(number_line);

        while(getline(config_file, line)){
            pos = line.find("copyfile=");

            if (line[0] != '#' && pos != line.npos){
				string values = line.substr(pos + 9, line.npos);
				size_t pos_delimiter = values.find(",");

				while (pos_delimiter != values.npos){
					copies.push_back(values.substr(0, pos_delimiter));
					values = values.substr(pos_delimiter + 1, values.npos);
					pos_delimiter = values.find(",");
				}
				copies.push_back(values);
				return OK;
			}
			if (line[0] == '[') return BAD;
		}
		return BAD;
	}

	int loadOptinalLog(){
		string command = "";
		int result = BAD;
		printf("- Recolectando copias\n");

		for(int i = 0; i < this->copies.size(); i++){
			// Guarda el nombre de los archivos en un texto plano
			command = "find " + this->path +
				" -maxdepth 1 -type f -printf \"%f\\n\" | grep " +
				this->copies[i] + " > ./.outputFile.txt";

			if (!system(command.c_str())){
				// Copiamos los archivos al directorio de evidencia
				command = "cat ./.outputFile.txt | xargs -I{} cp " +
					this->path + "/{} " + path_evidence + "/" + prefix + "-{}";

				if (!system(command.c_str())){
					// Limpiamos el contenido de cada archivo copiado
					fstream outputFile;
					outputFile.open("./.outputFile.txt", ios::in);

					if (outputFile.is_open()){
						string file;
						while (getline(outputFile, file)){
							ofstream ofs;
							ofs.open(this->path + "/" + file, ofstream::out | std::ofstream::trunc);
							ofs.close();
						}
					}
					printf("- Log Opcional %slimpiado%s\n", YEL, WHT);
					result = OK;
				}
			}
			command = "rm ./.outputFile.txt";
			system(command.c_str());
		}
		return result;
	}
};

int readConfigFile(ifstream &config_file){
	configData cfg_data;
	if (!cfg_data.readMainTag(config_file)) {
		printf("- Log a recuperar: %s\n", cfg_data.main_tag.c_str());

		if (!cfg_data.readOptionalPath(config_file)) {
			printf("- Ubicacion del log a recuperar: %s\n", cfg_data.path.c_str());

			if(!cfg_data.readCopies(config_file)){
				printf("- Copias a recuperar: ");

				for(int i = 0; i < cfg_data.copies.size() - 1; i++){
					printf("%s - ", cfg_data.copies[i].c_str());
				}

				printf("%s\n", cfg_data.copies.rbegin()->c_str());
				cfg_data.loadOptinalLog();
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
    return OK;
  }

  printf("- Recuperando Logs opcionales\n");

  return openConfigFile();
}
