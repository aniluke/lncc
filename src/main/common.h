#ifndef _COMMON_H_
#define _COMMON_H_

#include <string>
#include <map>
#include <vector>
#include <stdlib.h>
#include <stdint.h>

namespace lncc{
    typedef std::map<std::string, std::string> StringMap;

    struct TrainConfig {
    public:
        std::string train_fv;
        std::string train_qu;
        std::string model_folder;
        std::string exclude_features;

        int round_num;
        int node_num;
        double learning_rate;
        
    };

    struct TestConfig {
    public:
        std::string test_fv;
        std::string test_qu;
        std::string model_folder;
    };

}

#endif
