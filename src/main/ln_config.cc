#include "ln_config.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include <common_utils.h>

using namespace std;

int lncc::LnConfig::LoadConfig(const std::string& cfg_name) {
    ifstream ifs(cfg_name.c_str());
    if (ifs.fail()) {
        cerr << "cannot open config file: " << cfg_name << endl;
        return -1;
    }
    
    char line_buf[4096];

    while (!ifs.eof()) {
        ifs.getline(line_buf, 4096);
        line_buf[4095] = 0;
        if (strlen(line_buf) == 0 || line_buf[0] == '#') {
            continue;
        }
        char* token_start = strchr(line_buf, '=');
        if (token_start == NULL) {
            cerr << "invalid config: " << line_buf << endl;
            break;
        }
        int sep_pos = token_start - line_buf;
        line_buf[sep_pos] = 0;
        string key = lncc::str_strip(line_buf);
        string value = lncc::str_strip(&line_buf[sep_pos+1]);
        if (key.size() == 0) {
            cerr << "empty key" << endl;
            break;
        }
        cfg_map_[key] = value;

    }
    ifs.close();
    return 0;
}


std::string lncc::LnConfig::GetString(const std::string& key, const std::string& def) const {
    StringMap::const_iterator it = cfg_map_.find(key);
    if (it == cfg_map_.end()) {
        return def;
    }
    return it->second;
}


int64_t lncc::LnConfig::GetInt(const std::string& key, int64_t def) const {
    StringMap::const_iterator it = cfg_map_.find(key);
    if (it == cfg_map_.end()) {
        return def;
    }
    return atoi(it->second.c_str());
}

double lncc::LnConfig::GetDouble(const std::string& key, double def) const {
    StringMap::const_iterator it = cfg_map_.find(key);
    if (it == cfg_map_.end()) {
        return def;
    }
    return atof(it->second.c_str());
}
