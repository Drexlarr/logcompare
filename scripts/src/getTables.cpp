#include "../lib/getTables.h"

#include <fstream>
#include <iostream>
#include <vector>

#include "../lib/global.h"
#include "../lib/rabin_karp.h"
#include "../lib/utilities.h"
#include "../lib/values.h"

// TODO - Pasar los extern a global
extern std::ifstream* cfgfile;
extern std::ifstream* cfgbdfile;
extern FILE* resultcommand;

std::vector<string>* traces;
std::vector<string>* times;
std::vector<string>* dates;

extern std::vector<Database*>* dbs;
extern std::vector<Table*>* tables;

extern char localtimen[7];
extern char localdate[9];

int n_tables;

// Funcion que retorna el numero de tablas que se recolectaran, segun lo definido
// por el parametro ntables en el archivo de configuracion
int getNTables() {
    string bdtag = "[TABLES_BD]";
    string line;
    string number;
    char* linen;
    int ntabl;
    while (std::getline(*cfgfile, line)) {
        if (line.find(bdtag) != std::string::npos)
            break;
    }
    std::getline(*cfgfile, line);

    number = line.substr(line.find("=") + 1);

    return atoi(number.c_str());
}

// Funcion que carga los parametros encontrados en el archivo de configuración
// en la estructura Table y asigna valores a sus atributos
void getTableParameters(Table* table) {
    string parameter;
    string line;
    int needed = 0;
    while (std::getline(*cfgfile, line)) {
        if (line.find("[") == 0) {
            break;
        }
    }

    while (std::getline(*cfgfile, line)) {
        if (line.size() == 0) break;

        if (table->deltable == "Y" && needed == 3) break;

        parameter = line.substr(0, line.find("="));
        if (parameter == "tablename") {
            table->tablename = line.substr(line.find("=") + 1, line.size());
            needed++;
        } else if (parameter == "database") {
            table->database = line.substr(line.find("=") + 1, line.size());
            needed++;
        } else if (parameter == "deltable") {
            table->deltable = line.substr(line.find("=") + 1, line.size());
            needed++;
        }

        else if (parameter == "tracefield") {
            table->tracefield = line.substr(line.find("=") + 1, line.size());
        } else if (parameter == "timefield") {
            table->tracefield = line.substr(line.find("=") + 1, line.size());
        } else if (parameter == "datefield") {
            table->tracefield = line.substr(line.find("=") + 1, line.size());
        } else if (parameter == "columntrace") {
            table->columntrace = line.substr(line.find("=") + 1, line.size());
        } else if (parameter == "columndate") {
            table->columndate = line.substr(line.find("=") + 1, line.size());
        } else if (parameter == "columntime") {
            table->columntime = line.substr(line.find("=") + 1, line.size());
        }
    }

    if (needed < 3 || table->deltable.size() == 0) {
        printf("%sERROR:%s No se encontraron todos los parametros necesarios\n", RED, WHT);
        return;
    }
}

// Funcion que carga los parametros encontrados en el archivo de configuración de bd
// en la estructura Database y asigna valores a sus atributos
void getCredentialsBD(string database, Database* DB) {
    string line;
    string parameter;
    char buffer[101];
    int needed = 0;
    bool bd_exists = false;

    sprintf(buffer, "[%s]", database.c_str());

    string db(buffer);

    while (std::getline(*cfgbdfile, line)) {
        if (line.find(db) != std::string::npos) {
            bd_exists = true;
            break;
        }
    }

    if (!bd_exists) {
        printf("%sERROR:%s No existe la etiqueta [%s]\n", RED, WHT, database.c_str());
        return;
    }

    while (std::getline(*cfgbdfile, line)) {
        if (line.size() == 0) break;

        parameter = line.substr(0, line.find("="));

        if (parameter == "host") {
            DB->host = line.substr(line.find("=") + 1, line.size());
            needed++;
        } else if (parameter == "port") {
            DB->port = line.substr(line.find("=") + 1, line.size());
            needed++;
        } else if (parameter == "user") {
            DB->user = line.substr(line.find("=") + 1, line.size());
            needed++;
        }

        else if (parameter == "password") {
            DB->password = line.substr(line.find("=") + 1, line.size());
            needed++;
        }
    }

    if (needed < 4) {
        printf("%sERROR:%s No se encontraron todos los parametros necesarios\n", RED, WHT);
        return;
    }
}

// Funcion que genera un comando de conexión a una base de datos
// según los valores que contengan los atributos del parametro db
string connectQuery(Database* db) {
    char comm[101];
    sprintf(comm, "export PGPASSWORD='%s'; psql -h %s -p %s -U %s", db->password.c_str(), db->host.c_str(), db->port.c_str(), db->user.c_str());
    return string(comm);
}

void initParameters(string cfg_filename, string cfgbd_filename) {
    string line;
    cfgfile = new ifstream(cfg_filename.c_str());
    cfgbdfile = new ifstream(cfgbd_filename.c_str());

    n_tables = getNTables();
    dbs = new vector<Database*>(n_tables);
    tables = new vector<Table*>(n_tables);

    for (int i = 0; i < n_tables; i++) {
        tables->at(i) = new Table;
        getTableParameters(tables->at(i));
    }
    for (int i = 0; i < n_tables; i++) {
        dbs->at(i) = new Database;
        getCredentialsBD(tables->at(i)->database, dbs->at(i));
        cfgbdfile->clear();
        cfgbdfile->seekg(0);
    }

    cfgfile->close();
    cfgbdfile->close();
}

void delQuery(Database* db, Table* table) {
    string delquery;
    string connect_command;
    connect_command = connectQuery(db);
    printf("El comando de conexion es %s\n", connect_command.c_str());
    delquery += connect_command + " -c 'DELETE FROM " + table->tablename + ";'";
    system(delquery.c_str());
}

bool isInTraces(string probtrace) {
    for (int i = 0; i < traces->size(); i++) {
        if (traces->at(i) == probtrace) return true;
    }
    return false;
}

bool isInDates(string probdate) {
    for (int i = 0; i < traces->size(); i++) {
        if (traces->at(i) == probdate) return true;
    }
    return false;
}

bool isInTimes(string probtime) {
    for (int i = 0; i < traces->size(); i++) {
        if (traces->at(i) == probtime) return true;
    }
    return false;
}

void getTracesFromLog(string log_file, Table* table) {
    ifstream* flog = new ifstream(log_file.c_str());
    string command;
    string line;
    int aux;

    traces = new vector<string>;
    dates = new vector<string>;
    times = new vector<string>;

    string year = currentDateTime();
    year = line.subtr(0, 4);

    while (std::getline(*flog, line)) {
        if (line.find("B0" + table->tracefield + "[") != string::npos) {
            aux = line.find("B0" + table->tracefield + "[");
            if (!isInTraces(line.substr(aux + 5, 6))) {
                traces->push_back(line.substr(aux + 5, 6));
            }
        }
        if (line.find("DE0" + table->tracefield + "[") != string::npos) {
            aux = line.find("DE0" + table->tracefield + "[");
            if (!isInTraces(line.substr(aux + 6, 6))) {
                traces->push_back(line.substr(aux + 6, 6));
            }
        }
        if (line.find("B0" + table->timefield + "[") != string::npos) {
            aux = line.find("B0" + table->timefield + "[");
            if (!isInTimes(line.substr(aux + 5, 6))) {
                traces->push_back(line.substr(aux + 5, 6));
            }
        }
        if (line.find("DE0" + table->timefield + "[") != string::npos) {
            aux = line.find("DE0" + table->timefield + "[");
            if (!isInTimes(line.substr(aux + 6, 6))) {
                traces->push_back(line.substr(aux + 6, 6));
            }
        }
        if (line.find("B0" + table->datefield + "[") != string::npos) {
            aux = line.find("B0" + table->datefield + "[");
            if (!isInDates(line.substr(aux + 5, 4))) {
                traces->push_back(year + line.substr(aux + 5, 4));
            }
        }
        if (line.find("DE0" + table->datefield + "[") != string::npos) {
            aux = line.find("DE0" + table->datefield + "[");
            if (!isInDates(line.substr(aux + 6, 4))) {
                traces->push_back(year + line.substr(aux + 6, 4));
            }
        }
    }
}

void printNRegisters() {
    char result[9];
    string cutresult;
    fgets(result, 9, resultcommand);
    if (result)
        cutresult = string(result);
    if (cutresult.size() > 0) {
        cutresult = cutresult.substr(cutresult.find("COPY") + 5, cutresult.size() - 6);
        printf("\t%s**%s Se recolectaron %s registros\n", BLU, WHT, cutresult.c_str());
    } else {
        printf("\t%s**%s No se recolectaron registros\n", BLU, WHT);
    }
}

void printQuery(string fullquery, Table* table) {
    string clearselectquery;
    clearselectquery = fullquery.substr(fullquery.find("SELECT"));
    cout << "Para la tabla " << table->tablename << " se ejecutara el siguiente query: " << clearselectquery << endl;
}

void selectTracesQuery(Database* db, Table* table, string log_file) {
    getTracesFromLog(log_file, table);

    if (traces->size() == 0)
        printf("WARNING: No se encontraron traces\n");

    char format_trace[14];
    char format_date[14];
    char format_time[16];
    string connect_command;
    string fullquery;
    string clearselectquery;
    char selectquery[151];

    connect_command = connectQuery(db);

    sprintf(selectquery, "%s -c \"\\copy (SELECT * FROM %s WHERE ", connect_command.c_str(), table->tablename.c_str());

    fullquery = string(selectquery);
    if (traces->size() == 1) {
        fullquery += table->columntrace + " in ";
        sprintf(format_trace, "('%s') AND ", traces->at(0).c_str());
        fullquery += string(format_trace);
    } else if (traces->size() > 0) {
        fullquery += table->columntrace + " in ";
        for (int i = 0; i < traces->size(); i++) {
            if (i < traces->size() - 1 && i != 0)
                sprintf(format_trace, "'%s',", traces->at(i).c_str());
            else
                sprintf(format_trace, "'%s') AND ", traces->at(i).c_str());
            if (i == 0) {
                sprintf(format_trace, "('%s',", traces->at(i).c_str());
                fullquery += string(format_trace);
            } else
                fullquery += string(format_trace);
        }
    }

    if (dates->size() == 1) {
        fullquery += table->columndate + " in ";
        sprintf(format_date, "('%s') AND ", dates->at(0).c_str());
        fullquery += string(format_date);
    } else if (dates->size() > 0) {
        fullquery += table->columndate + " in ";
        for (int i = 0; i < dates->size(); i++) {
            if (i < dates->size() - 1 && i != 0)
                sprintf(format_date, "'%s',", dates->at(i).c_str());
            else
                sprintf(format_date, "'%s') AND ", dates->at(i).c_str());
            if (i == 0) {
                sprintf(format_date, "('%s',", dates->at(i).c_str());
                fullquery += string(format_date);
            } else
                fullquery += string(format_date);
        }
    }

    if (times->size() == 1) {
        fullquery += table->columntime + " in ";
        sprintf(format_time, "('%s') AND ", times->at(0).c_str());
        fullquery += string(format_time);
    } else if (times->size() > 0) {
        fullquery += table->columntime + " in ";
        for (int i = 0; i < dates->size(); i++) {
            if (i < times->size() - 1 && i != 0)
                sprintf(format_time, "'%s',", times->at(i).c_str());
            else
                sprintf(format_time, "'%s') AND ", times->at(i).c_str());
            if (i == 0) {
                sprintf(format_time, "('%s',", times->at(i).c_str());
                fullquery += string(format_time);
            } else
                fullquery += string(format_time);
        }
    }

    printQuery(fullquery, table);

    fullquery += ") To '" + path_evidence + "/" + prefix + " " + table->tablename + ".csv' With CSV DELIMITER ',' HEADER;\"";

    resultcommand = popen(fullquery.c_str(), "r");

    printNRegisters();

    pclose(resultcommand);
}

void deleteDbs() {
    for (int i = 0; i < tables->size(); i++) {
        if (tables->at(i)->deltable == "Y") {
            delQuery(dbs->at(i), tables->at(i));
        }
    }
}

void selectAllQuery(Database* db, Table* table) {
    string query;
    string connect_command;

    connect_command = connectQuery(db);
    query += connect_command + " -c \"\\copy (SELECT * FROM " + table->tablename;
    printQuery(query, table);
    query += ") To '" + path_evidence + "/" + prefix + " " + table->tablename + ".csv' With CSV DELIMITER ',' HEADER;\"";

    resultcommand = popen(query.c_str(), "r");

    printNRegisters();

    pclose(resultcommand);
}

void recolectAllTables(string log_file) {
    for (int i = 0; i < tables->size(); i++) {
        if (tables->at(i)->deltable == "Y")
            selectAllQuery(dbs->at(i), tables->at(i));
        else
            selectTracesQuery(dbs->at(i), tables->at(i), log_file);
    }

    printf("%sSUCCESS:%s Se culmino la recoleccion de registros de las tablas", GRN, WHT);
}
