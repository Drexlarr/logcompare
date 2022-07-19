#include "../lib/utilities.h"
#include "../lib/global.h"

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

bool getMainDir(const char *name){
    const char *ret = getenv(name);
    if (ret){
        sixdir = std::string (ret);
        path_evidence = sixdir + "/tmp/evidencia";
        path_cfg = sixdir + "/collector/cfgfiles";
    }
    return !!ret;
}
