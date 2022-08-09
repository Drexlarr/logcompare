#ifndef GET_TABLES
#define GET_TABLES

#include <string>
using namespace std;

struct Database {
    string host;
    string port;
    string user;
    string password;
};

struct Table {
    string tablename;
    string database;
    string deltable;
    string tracefield;
    string columntrace;
    string columntime;
    string columndate;
};

int getNTables();
void getTableParameters(Table* table);
void getCredentialsBD(string database, Database* DB);
void initParameters(string cfg_filename, string cfgbd_filename);
void selectAllQuery(Database* db, Table* table);
void delQuery(Database* db, Table* table);
string connectQuery(Database* db);
void getTracesFromLog(string log_file, Table* table);
void removeElement(int size, int index);
int removeDuplicate(int n_traces);
void selectTracesQuery(Database* db, Table* table, string log_file);
bool isInTraces(string probtrace);
void deleteDbs();
void prinNRegisters();
void recolectAllTables(string log_file);
void printQuery(string fullquery, Table* table);

#endif
