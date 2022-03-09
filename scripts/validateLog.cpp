#include <string>
#include "validateLog.h"
#include <bits/stdc++.h>
#include "rabin_karp.h"
#include "values.h"

std::fstream* logfile;
std::ifstream* xmlfile;
std::fstream* resultfile;
std::fstream* inputfile;
std::fstream* dirtyxmlfile;

using namespace std;

bool checkIfFileExists(string filename){
    ifstream *file = new ifstream(filename);
    return file->good();
}

void writeLog(int pos, string line){
    *inputfile << "===>";
    for(int i = pos+7; i < line.size(); i++){
        if(line[i] == '<') break;
        *inputfile << line[i];
    } 
    *inputfile << endl;
}

void findLogLines(){ 
    string line;
    char* lineaux;
    int arr[100];
    int* posfound; 
    int ocd;

    while (getline(*xmlfile, line)) {
        posfound = rabin_karp("===&gt;", &line[0], 101);
        if(posfound[99] > 0){
            for(int i = 0; i < posfound[99]; i++){
                writeLog(posfound[i], line);
            }
        }
    }
}
    
string getProcessName(string line){
    string pname;

    for(int i = 0; i < 10; i++)
        pname += line[42+i];

    return pname;
}

void compFiles(){
    
    string linelog;
    string lineinput;
    int* arr; 
    int ocd; 
    string process_name;
    string aux;

    while (getline(*inputfile, lineinput)) {
        logfile->clear();
        logfile->seekg(0);
        ocd = 0;
        *resultfile << lineinput << endl;
        while (getline(*logfile, linelog)) {
            arr = rabin_karp(&lineinput[0], &linelog[0], 101);
            ocd += arr[99];       
            if (arr[99] > 0){
                process_name = getProcessName(linelog);
                *resultfile << " ---------- HALLADO - NOMBRE DEL PROCESO \t\t" << process_name << endl;
            }
        }
        if(ocd) *resultfile << endl;
        if(!ocd)
            *resultfile << " ---------- NO HALLADO" << endl << endl;
    }
}

void clearXmlFile(string dirtyxmlfile_name){
    string line;
    string command;
    FILE* results;
    char numberdivs[3];
    int divs;
    int aux = 0;
    int count = 0;
    bool completedivs = false;
    command = "sed '1,/<expectedresults>/d' '" + dirtyxmlfile_name + "' > ../files/clear.xml";
    system(command.c_str());
	system("sed -i 's/&nbsp;/ /g' ../files/clear.xml");
	system("sed -i 's/ </</g' ../files/clear.xml");
	system("sed -i \"s,/ol,-ol,\" ../files/clear.xml");
    
    xmlfile->close();

    xmlfile->open("../files/clear.xml");
    results = popen("grep -o '<ol start' ../files/clear.xml | wc -l", "r");

    fgets(numberdivs,3, results);
    divs = atoi(numberdivs);

    if(divs == 0)
        system("sed -i \"/-ol>/Q\" ../files/clear.xml");

    else{
        while(getline(*xmlfile, line)){
            count++;
            if(line.find("<ol start") != string::npos)
                aux++;

            if(aux == divs) completedivs = true;

            if(completedivs && line == "<-ol>"){
                count++;
                command = "sed -i '" + to_string(count) + "'',$d' ../files/clear.xml";
                system(command.c_str());
                break;
            }
        }
    }
    xmlfile->close();
}

void validateLog(){
    string logfile_name;
    string dirtyxmlfile_name;

    printf("Ingrese el nombre del archivo xml: "); 
    cin >> dirtyxmlfile_name;
    dirtyxmlfile_name = "../files/" + dirtyxmlfile_name;

    if(!checkIfFileExists(dirtyxmlfile_name)){
        printf("%sERROR:%s No se encontró el archivo, nombre o ruta invalida", RED, WHT);
        return;
    }

    printf("Ingrese el nombre del archivo log: ");
    cin >> logfile_name;
    logfile_name = "../files/" + logfile_name;

    if(!checkIfFileExists(dirtyxmlfile_name)){
        printf("%sERROR:%s No se encontró el archivo, nombre o ruta invalida", RED, WHT);
        return;
    }

    inputfile = new fstream("../files/input.txt", std::ofstream::out | std::ofstream::trunc);
    xmlfile = new ifstream("../files/clear.xml");
    resultfile = new fstream("../files/results.txt", std::ofstream::out | std::ofstream::trunc);
    logfile = new fstream(logfile_name);
    dirtyxmlfile = new fstream(dirtyxmlfile_name);

    clearXmlFile(dirtyxmlfile_name);

    xmlfile->open("../files/clear.xml");

    findLogLines();
    inputfile->close();
    inputfile->open("../files/input.txt");

    compFiles();

    inputfile->close();
    xmlfile->close();
    resultfile->close();
    logfile->close();
}

