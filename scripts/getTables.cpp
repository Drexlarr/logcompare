#include <bits/stdc++.h>
#include "values.h"
#include "getTables.h"
#include "rabin_karp.h"

extern std::ifstream* cfgfile;
extern std::ifstream* cfgbdfile;
extern FILE* resultcommand;

std::vector<string>* traces;

extern std::vector<Database*>* dbs;
extern std::vector<Table*>* tables;

extern char localtimen[7];
extern char localdate[9];
extern std::string testcase_prefix;


int n_tables; 

using namespace std;
//Funcion que retorna el numero de tablas que se recolectaran, segun lo definido
//por el parametro ntables en el archivo de configuracion
int getNTables(){
    string bdtag = "[TABLES_BD]";
    string line;
    string number;
    char* linen;
    int ntabl;
    while(getline(*cfgfile, line)){
        if(line.find(bdtag) != std::string::npos)
            break;
    }    
    getline(*cfgfile, line);

    number = line.substr(line.find("=")+1);

    return atoi(number.c_str());
}

//Funcion que carga los parametros encontrados en el archivo de configuración
//en la estructura Table y asigna valores a sus atributos
void getTableParameters(Table* table){
    string parameter;
    string line;
    int needed = 0;
    while(getline(*cfgfile, line)){     
        if(line.find("[") == 0) {
            break;
        }
    } 

    while(getline(*cfgfile, line)){
        if(line.size() == 0) break;

        if(table->deltable == "Y" && needed == 3) break;

        parameter = line.substr(0, line.find("="));
        if(parameter == "tablename"){
            table->tablename = line.substr(line.find("=") + 1, line.size());
            needed++;
        }
        else if(parameter == "database"){
            table->database = line.substr(line.find("=") + 1, line.size());
            needed++;
        }
        else if(parameter == "deltable"){
            table->deltable = line.substr(line.find("=") + 1, line.size());
            needed++;
        }

        else if(parameter == "tracefield"){
            table->tracefield = line.substr(line.find("=") + 1, line.size());
        }
        else if(parameter == "columntrace"){
            table->columntrace = line.substr(line.find("=") + 1, line.size());
        }
        else if(parameter == "columndate"){
            table->columndate = line.substr(line.find("=") + 1, line.size());
        }
        else if(parameter == "columntime"){
            table->columntime = line.substr(line.find("=") + 1, line.size());
        }
    }

    if(needed < 3 || table->deltable.size() == 0){
        printf("%sERROR:%s No se encontraron todos los parametros necesarios\n", RED, WHT);
        return;
    }
}

//Funcion que carga los parametros encontrados en el archivo de configuración de bd
//en la estructura Database y asigna valores a sus atributos
void getCredentialsBD(string database, Database* DB){
    string line;
    string parameter;
    char buffer[101];
    int needed = 0;
    bool bd_exists = false;

    sprintf(buffer, "[%s]", database.c_str());
    
    string db(buffer);

    while(getline(*cfgbdfile, line)){     
        if(line.find(db) != std::string::npos) {
            bd_exists = true;
            break;
        }
    }
    
    if(!bd_exists){
        printf("%sERROR:%s No existe la etiqueta [%s]\n", RED, WHT, database.c_str());
        return;
    }

    while(getline(*cfgbdfile, line)){
        if(line.size() == 0) break;

        parameter = line.substr(0, line.find("="));

        if(parameter == "host"){
            DB->host = line.substr(line.find("=") + 1, line.size());
            needed++;
        }
        else if(parameter == "port"){
            DB->port = line.substr(line.find("=") + 1, line.size());
            needed++;
        }
        else if(parameter == "user"){
            DB->user = line.substr(line.find("=") + 1, line.size());
            needed++;
        }

        else if(parameter == "password"){
            DB->password = line.substr(line.find("=") + 1, line.size());
            needed++;
        }
    }

    if(needed < 4){ 
        printf("%sERROR:%s No se encontraron todos los parametros necesarios\n", RED, WHT);
        return;
    }     
}

//Funcion que genera un comando de conexión a una base de datos
//según los valores que contengan los atributos del parametro db
string connectQuery(Database* db){
    char comm[101];
    sprintf(comm, "export PGPASSWORD='%s'; psql -h %s -p %s -U %s", db->password.c_str(), db->host.c_str(), db->port.c_str(), db->user.c_str());
    return string(comm);
}

void initParameters(string cfg_filename, string cfgbd_filename){
    string line;
    cfgfile = new ifstream(cfg_filename.c_str());
    cfgbdfile = new ifstream(cfgbd_filename.c_str());

    n_tables = getNTables();
    dbs = new vector<Database*>(n_tables);
    tables = new vector<Table*>(n_tables);

    for(int i = 0; i < n_tables; i++){
        tables->at(i) = new Table;
        getTableParameters(tables->at(i));
    }
    for(int i = 0; i < n_tables; i++){
        dbs->at(i) = new Database;
        getCredentialsBD(tables->at(i)->database, dbs->at(i));
        cfgbdfile->clear();
        cfgbdfile->seekg(0);
    } 
    
    cfgfile->close();
    cfgbdfile->close();
}

void delQuery(Database* db, Table* table){
    string delquery;
    string connect_command;
    connect_command = connectQuery(db);
    printf("El comando de conexion es %s\n", connect_command.c_str());
    delquery += connect_command + " -c 'DELETE FROM " + table->tablename + ";'";
    delquery += "> null";
    system(delquery.c_str());
}

bool isInTraces(string probtrace){
    for(int i = 0; i < traces->size(); i++){
        if(traces->at(i) == probtrace) return true;
    }
    return false;
}

void getTracesFromLog(string log_file, Table* table){ 
    ifstream* flog = new ifstream(log_file.c_str());
    string command;
    string line;
    int aux;

    traces = new vector<string>;

    while(getline(*flog, line)){
        if(line.find("B0" + table->tracefield + "[") != string::npos){
            aux = line.find("B0" + table->tracefield + "[");
            if(!isInTraces(line.substr(aux + 5, 6))){
                traces->push_back(line.substr(aux + 5, 6));
            }
        }
        if(line.find("DE0" + table->tracefield + "[") != string::npos){
            aux = line.find("DE0" + table->tracefield + "[");
            if(!isInTraces(line.substr(aux + 6, 6))){
                traces->push_back(line.substr(aux + 6, 6));
            }
        }
    }
}

void printNRegisters(){
    char result[9];
    string cutresult;
   
    fgets(result, 9, resultcommand);
    cutresult = string(result);
    cutresult = cutresult.substr(cutresult.find("COPY") + 5, cutresult.size() - 6);

    printf("\t%s**%s Se recolectaron %s registros\n", BLU, WHT, cutresult.c_str());
}

void printQuery(string fullquery, Table* table){
    string clearselectquery;
    clearselectquery = fullquery.substr(fullquery.find("SELECT"));
    cout << "Para la tabla " << table->tablename <<" se ejecutara el siguiente query: " << clearselectquery << endl;
}

void selectTracesQuery(Database* db, Table* table, string log_file){
    getTracesFromLog(log_file, table);
    
    if(traces->size() == 0) 
        printf("WARNING: No se encontraron traces\n");
    
    char format_trace[14];
    string connect_command;
    string fullquery;
    string clearselectquery;
    char selectquery[151];
    
    connect_command = connectQuery(db);

    sprintf(selectquery, "%s -c \"\\copy (SELECT * FROM %s WHERE ", connect_command.c_str() , table->tablename.c_str());

    fullquery = string(selectquery);
    if(traces->size() == 1){
        fullquery += table->columntrace + " in ";
        sprintf(format_trace, "('%s') AND ", traces->at(0).c_str());
        fullquery += string(format_trace);
    }
    else if(traces->size() > 0){
        fullquery += table->columntrace + " in ";
        for(int i = 0; i < traces->size(); i++){
            if(i < traces->size()-1 && i != 0)
                sprintf(format_trace, "'%s',", traces->at(i).c_str());
            else
                sprintf(format_trace, "'%s') AND ", traces->at(i).c_str());
            if(i == 0) {
                sprintf(format_trace, "('%s',", traces->at(i).c_str());
                fullquery += string(format_trace);
            }
            else
                fullquery += string(format_trace);
        }
    }

    fullquery += table->columndate + " >= '" + string(localdate) + "' AND " + table->columntime + " >= '" + string(localtimen) + "'";

    printQuery(fullquery, table);

    fullquery += ") To '../files/" + testcase_prefix +  "-" + table->tablename + ".csv' With CSV DELIMITER ',' HEADER;\"";    

    resultcommand = popen(fullquery.c_str(), "r");

    printNRegisters();

    pclose(resultcommand); 
}

void deleteDbs(){
    for(int i = 0; i < tables->size(); i++){
        if (tables->at(i)->deltable == "Y"){
            delQuery(dbs->at(i), tables->at(i));
        }
    }
}

void selectAllQuery(Database* db, Table* table){
    string query;
    string connect_command;
    
    connect_command = connectQuery(db);
    query += connect_command + " -c \"\\copy (SELECT * FROM "+ table->tablename;
    printQuery(query, table);
    query += ") To '../files/" + testcase_prefix +  "-" + table->tablename + ".csv' With CSV DELIMITER ',' HEADER;\"";
    
    resultcommand = popen(query.c_str(), "r");

    printNRegisters();

    pclose(resultcommand);
}

void recolectAllTables(string log_file){
    for(int i = 0; i< tables->size(); i++){
        if (tables->at(i)->deltable == "Y")
            selectAllQuery(dbs->at(i), tables->at(i));
        else
            selectTracesQuery(dbs->at(i), tables->at(i), log_file);
    }

    printf("%sSUCCESS:%s Se culmino la recoleccion de registros de las tablas", GRN, WHT);
}

