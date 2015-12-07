#ifndef _LOSS_FUNCTION_FACTORY_H_
#define _LOSS_FUNCTION_FACTORY_H_

#include "loss_function.h"
#include <string>

namespace lncc {
    class LossFunctionFactory {
    public:
        static LossFunction* CreateLossFunction(const std::string& loss_func);
        
    };
}

#endif
