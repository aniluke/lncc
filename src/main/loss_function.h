#ifndef _LOSS_FUNCTION_H_
#define _LOSS_FUNCTION_H_
#include <common.h>

namespace lncc {
    class LossFunction {
    public:
        virtual double ComputeNegativeGradient(std::vector<double>* out_gradients, 
                            const std::vector<double>& targets,
                            const std::vector<double>& predictions) = 0;

    };
}

#endif
