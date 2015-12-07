#ifndef _COMMON_UTILS_H_
#define _COMMON_UTILS_H_

#include <common.h>
#include <string>
#include <vector>
#include <sstream>

namespace lncc {
    enum {
        E_LOG_DEBUG,
        E_LOG_INFO,
        E_LOG_WARN,
        E_LOG_ERROR
    };
    void init_log_level(int log_level);

    void log_debug(const char* format, ...);
    void log_info(const char* format, ...);
    void log_warn(const char* format, ...);
    void log_error(const char* format, ...);

    int str_split(const std::string& str, char del, std::vector<std::string>& output);
    std::string str_strip(const std::string& str);

    template<class T>
    std::string to_string(const T& data) {
        std::ostringstream os;
        os << data;
        return os.str();
    }

    std::string tolower(const std::string& str);
    std::string toupper(const std::string& str);

    int load_vector(std::vector<double>& data, const std::string& path);
    uint64_t get_time_ms();
    double get_time_precise();
    
}

#endif
