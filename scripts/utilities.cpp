#include "lib/utilities.h"

std::string date_after_launch = "2022-02-02 10:00:00";
std::string sixdir = "";
std::string path_evidence = "";
std::string path_cfg = "";
std::string prefix = "";

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
}

const std::string ddlFormat() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "ddl%Y%m%d.log", &tstruct);

    return buf;
}

bool getenv(const char *name, std::string &env){
    const char *ret = getenv(name);
    if (ret){
        env = std::string (ret);
        path_evidence = env + "/tmp/evidencia";
        // TEMPORAL
        path_cfg = env + "/collector/cfgfiles";
    }
    return !!ret;
}
