#include "../lib/utilities.h"
#include "../lib/global.h"

// TODO - Retirar
std::string date_after_launch = "2022-02-02 10:00:00";

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
        path_cfg = env + "/collector/cfgfiles";
    }
    return !!ret;
}
