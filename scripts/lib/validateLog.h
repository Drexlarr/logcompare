#ifndef VALIDATE_LOG
#define VALIDATE_LOG    
#include <string>


bool checkIfFileExists(std::string filename);
void writeLog(int pos, std::string line);
void findLogLines();
std::string getProcessName(std::string line);
void compFiles();
void clearXmlFile(std::string dirtyxmlfile_name);
void validateLog();

#endif