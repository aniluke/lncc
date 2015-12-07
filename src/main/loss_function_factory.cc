#include "loss_function_factory.h"
#include "least_square_loss_function.h"
#include "logistic_loss_function.h"
#include <common_utils.h>

using namespace lncc;
using namespace std;

LossFunction* LossFunctionFactory::CreateLossFunction(const std::string& loss_func) {
    LossFunction* obj = NULL;
    if (loss_func == "logistic") {
        obj = new LogisticLossFunction();
    } else if (loss_func == "least_square") {
        obj = new LeastSquareLossFunction();
    } else {
        log_error("Invalid loss function: %s\n", loss_func.c_str());
    }
    if (obj) {
        log_info("Using loss function %s\n", loss_func.c_str());
    }
    return obj;
}
