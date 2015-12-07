#ifndef _LN_CONFIG_H_
#define _LN_CONFIG_H_
#include <common.h>

namespace lncc {
    class LnConfig {
    public:
        LnConfig() {}
        int LoadConfig(const std::string& cfg_name);

        std::string GetString(const std::string& key, const std::string& def) const;
        int64_t GetInt(const std::string& key, int64_t def) const;

        double GetDouble(const std::string& key, double def) const;

        
    private:
        StringMap cfg_map_;
    };
}
#endif
