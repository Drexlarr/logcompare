#include "../lib/validateLog.h"

#include <bits/stdc++.h>

#include <sstream>
#include <string>

#include "../lib/global.h"
#include "../lib/rabin_karp.h"
#include "../lib/values.h"

std::fstream* logfile;
std::ifstream* xmlfile;
std::fstream* resultfile;
std::fstream* inputfile;
std::fstream* dirtyxmlfile;

std::string path_validate = sixdir + "/binappl/collector/files";

using namespace std;

bool checkIfFileExists(string filename) {
    ifstream* file = new ifstream(filename.c_str());
    return file->good();
}

void writeLog(int pos, string line, int index) {
    *inputfile << index << ". ===>";
    for (int i = pos + 7; i < line.size(); i++) {
        if (line[i] == '<') break;
        *inputfile << line[i];
    }
    *inputfile << endl;
}

void findLogLines() {
    string line;
    char* lineaux;
    int* posfound;
    int ocd;
    int index = 1;

    while (getline(*xmlfile, line)) {
        string param = "===&gt;";
        posfound = rabin_karp((char*)param.c_str(), &line[0], 101);
        if (posfound[99] > 0) {
            writeLog(posfound[0], line, index);
            index++;
        }
    }
}

string getProcessName(string line) {
    string pname;

    for (int i = 0; i < 10; i++)
        pname += line[42 + i];

    return pname;
}

void compFiles() {
    string linelog;
    string lineinput;
    int* arr;
    int ocd;
    string process_name;
    string aux;
    string notfound;
    int ntfound = 0;
    int count = 1;
    stringstream ss;
    string auxstr;

    while (getline(*inputfile, lineinput)) {
        logfile->clear();
        logfile->seekg(0);
        ocd = 0;
        *resultfile << lineinput << endl;
        while (getline(*logfile, linelog)) {
            arr = rabin_karp(&lineinput.substr(lineinput.find(".") + 1)[0], &linelog[0], 101);
            ocd += arr[99];
            if (arr[99] > 0) {
                process_name = getProcessName(linelog);
                *resultfile << " ---------- HALLADO - NOMBRE DEL PROCESO \t\t" << process_name << endl;
            }
        }
        if (ocd) *resultfile << endl;
        if (!ocd) {
            ntfound++;
            ss << count;
            auxstr = ss.str();
            notfound += auxstr + ", ";
            ss.str("");
            *resultfile << " ---------- NO HALLADO" << endl
                        << endl;
        }
        count++;
    }

    if (notfound.size() == 0)
        printf("%sSUCCESS%s: Se encontraron %d de %d lineas de log\n", GRN, WHT, count, count);

    else {
        printf("%sWARNING%s: No se encontraron %d de %d lineas de log\n", YEL, WHT, ntfound, count);
        printf("\t%s**%s No se encontraron las siguientes lineas de log %s(%s)%s. Los indices pueden ser encontrados en el archivo results.txt\n", BLU, WHT, BLU, notfound.substr(0, notfound.size() - 2).c_str(), WHT);
    }
}

void clearXmlFile(string dirtyxmlfile_name) {
    string line;
    string command;
    FILE* results;
    char numberdivs[3];
    int divs;
    int aux = 0;
    int count = 0;
    bool completedivs = false;
    stringstream ss;
    string auxstr;
    command = "sed '1,/<expectedresults>/d' '" + dirtyxmlfile_name + "' > " + path_validate + "clear.xml";
    system(command.c_str());
    command = "sed -i 's/&nbsp;/ /g' " + path_validate + "clear.xml";
    system(command.c_str());
    command = "sed -i 's/ </</g' " + path_validate + "clear.xml";
    system(command.c_str());
    command = "sed -i \"s,/ol,-ol,\" " + path_validate + "clear.xml";
    system(command.c_str());

    xmlfile->close();

    xmlfile->open(path_validate + "clear.xml");
    command = "grep -o '<ol start' " + path_validate + "clear.xml | wc -l";
    results = popen(command.c_str(), "r");

    fgets(numberdivs, 3, results);
    divs = atoi(numberdivs);

    if (divs == 0) {
        command = "sed -i \"/-ol>/Q\" " + path_validate + "clear.xml";
        system(command.c_str());
    } else {
        while (getline(*xmlfile, line)) {
            count++;
            if (line.find("<ol start") != string::npos)
                aux++;

            if (aux == divs) completedivs = true;

            if (completedivs && line == "<-ol>") {
                count++;
                ss << count;
                ss >> auxstr;
                command = "sed -i '" + auxstr + "'',$d' " + path_validate + "clear.xml";
                system(command.c_str());
                break;
            }
        }
    }
    xmlfile->close();
}

void validateLog() {
    string logfile_name;
    string dirtyxmlfile_name;

    printf("Ingrese el nombre del archivo xml: ");
    cin >> dirtyxmlfile_name;
    dirtyxmlfile_name = path_validate + dirtyxmlfile_name;

    if (!checkIfFileExists(dirtyxmlfile_name)) {
        printf("%sERROR:%s No se encontró el archivo, nombre o ruta invalida", RED, WHT);
        return;
    }

    printf("Ingrese el nombre del archivo log: ");
    cin >> logfile_name;
    logfile_name = path_validate + logfile_name;

    if (!checkIfFileExists(dirtyxmlfile_name)) {
        printf("%sERROR:%s No se encontró el archivo, nombre o ruta invalida", RED, WHT);
        return;
    }

    inputfile = new fstream(path_validate + "input.txt", std::ofstream::out | std::ofstream::trunc);
    xmlfile = new ifstream(path_validate + "clear.xml");
    resultfile = new fstream(path_validate + "results.txt", std::ofstream::out | std::ofstream::trunc);
    logfile = new fstream(logfile_name.c_str());
    dirtyxmlfile = new fstream(dirtyxmlfile_name.c_str());

    clearXmlFile(dirtyxmlfile_name);

    xmlfile->open(path_validate + "clear.xml");

    findLogLines();
    inputfile->close();
    inputfile->open(path_validate + "input.txt");

    compFiles();

    getchar();
    getchar();
    inputfile->close();
    xmlfile->close();
    resultfile->close();
    logfile->close();
}
