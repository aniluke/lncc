#ifndef _LOGISTIC_LOSS_FUNCTION_H_
#define _LOGISTIC_LOSS_FUNCTION_H_

#include <loss_function.h>

namespace lncc {
    class LogisticLossFunction: public LossFunction {
    public:
        virtual double ComputeNegativeGradient(std::vector<double>* out_gradients, 
                            const std::vector<double>& targets,
                            const std::vector<double>& predictions);
    };
}

#endif
