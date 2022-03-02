#ifndef GET_TABLES
#define GET_TABLES

struct Database{
   char host[101];
   char port[101];
   char user[101];
   char password[101];
};

struct Table{
   char tablename[101];
   char database[101];
   char deltable[101];
   char tracefield[101];
   char columntrace[101];
   char columntime[101];
   char columndate[101];
};

int getNTables();
void getTableParameters(struct Table* table);
void getCredentialsBD(char* database, struct Database* DB);
void execQuery();
void initParameters(char* cfg_filename, char* cfgbd_filename);
void selectAllQuery(struct Database* db, struct Table* table);
void delQuery(struct Database* db);
void connectQuery(struct Database* db, char* command);
int getTracesFromLog(char* log_file);
void removeElement(int size, int index);
int removeDuplicate(int n_traces);
void selectTracesQuery(struct Database* db, struct Table* table, char* log_file);
int isInTraces(int n, char* aux);

#endif