#include "common_utils.h"
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <time.h>
#include <stdarg.h>
#include <fstream>
#include <stdlib.h>
#include <sys/time.h>

using namespace std;
using namespace lncc;

static int g_log_level = lncc::E_LOG_INFO;

void lncc::init_log_level(int log_level) {
    g_log_level = log_level;
}

static void log_internal(int log_level, const char* format, va_list& args) {
    if (log_level < g_log_level) {
        return;
    }
    string prefix;
    switch (log_level) {
    case E_LOG_DEBUG:
        prefix = "[DEBUG] ";
        break;
    case E_LOG_INFO:
        prefix = "[INFO] ";
        break;
    case E_LOG_WARN:
        prefix = "[WARN] ";
        break;
    case E_LOG_ERROR:
        prefix = "[ERROR] ";
        break;
    }
    
    time_t curr_time;
    struct tm * timeinfo;
    char buffer[256];
    time (&curr_time);
    timeinfo = localtime (&curr_time);
    strftime(buffer, 256, "%Y-%m-%d %H:%M:%S %Z ", timeinfo);
    prefix = buffer + prefix;

    std::string final_format;
    final_format = prefix + format;
    vprintf (final_format.c_str(), args);
}

void lncc::log_debug(const char* format, ...) {
    va_list arg_list;
    va_start(arg_list, format);
    log_internal(E_LOG_DEBUG, format, arg_list);
}

void lncc::log_info(const char* format, ...) {
    va_list arg_list;
    va_start(arg_list, format);
    log_internal(E_LOG_INFO, format, arg_list);
}

void lncc::log_warn(const char* format, ...) {
    va_list arg_list;
    va_start(arg_list, format);
    log_internal(E_LOG_WARN, format, arg_list);
}

void lncc::log_error(const char* format, ...) {
    va_list arg_list;
    va_start(arg_list, format);
    log_internal(E_LOG_ERROR, format, arg_list);
}

int lncc::str_split(const string& str, char del, vector<string>& output) {
    size_t len = str.size();
    if (len == 0)
        return 0;
    
    char* buf = new char[len+1];
    char* start = buf;
    memcpy(buf, str.c_str(), len+1);
    
    for(int i = 0; i < len; i ++) {
        if (buf[i] == del) {
            buf[i] = 0;
            string token = start;
            output.push_back(token);
            buf[i] = del;
            start = &buf[i+1];
        }
    }
    if (start != &buf[len]) {
        string token = start;
        output.push_back(token.c_str());
    } else if (buf[len - 1] == del) {
        output.push_back("");
    }

    delete[] buf;
    return output.size();
}

string lncc::tolower(const string& str) {
    string out;
    out.resize(str.size());
    std::transform(str.begin(), str.end(), out.begin(), ::tolower);
    return out;
}

string lncc::toupper(const string& str) {
    string out;
    out.resize(str.size());
    std::transform(str.begin(), str.end(), out.begin(), ::toupper);
    return out;
}


int lncc::load_vector(std::vector<double>& data, const std::string& path) {
    std::ifstream ifs(path.c_str());
    if (ifs.fail())
        return -1;
    while (!ifs.eof()) {
        std::string line;
        std::getline(ifs, line);
        if (line.size() == 0) 
            break;
        
        double val = atof(line.c_str());
        data.push_back(val);
    }
    return 0;
}

std::string lncc::str_strip(const std::string& str) {
    int len = str.size();
    if (len == 0)
        return "";

    const char* trims = " \t\n\r";
    int trims_len = 4;
    char* buf = new char[len+1];
    memcpy(buf, str.c_str(), len+1);

    int i,j;
    int start = 0;
    for(i = 0; i < len; i ++) {
        for(j = 0; j < trims_len; j ++) {
            if (buf[i] == trims[j]) {
                break;
            }
        }
        if (j == trims_len) {
            start = i;
            break;
        }
        
    }
    if (i == len) {
        string output = str;
        delete[] buf;
        return output;
    }

    int end = 0;
    for(i = len-1; i>=0; i --) {
        for(j = 0; j < trims_len; j ++) {
            if (buf[i] == trims[j]) {
                break;
            }
        }
        if (j == trims_len) {
            end = i;
            break;
        }        
    }
    buf[end+1] = 0;
    string output = &buf[start];

    delete[] buf;
    return output;
}

uint64_t lncc::get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*1000+tv.tv_usec/1000;
}

double lncc::get_time_precise() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (double)tv.tv_sec+tv.tv_usec/1000000.0;
}
