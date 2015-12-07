#include "logistic_loss_function.h"
#include <math.h>
using namespace std;

double lncc::LogisticLossFunction::ComputeNegativeGradient(vector<double>* out_gradients,
                                                           const vector<double>& targets,
                                                           const vector<double>& predictions) {
    int sample_num = targets.size();
    if (sample_num == 0)
        return 0;
    
    double mse = 0.0;
    for (int i = 0; i < sample_num; i ++) {

        double grad = -2.0 * targets[i] / (1.0 + exp(2.0 * targets[i] * predictions[i]));
        //double grad = -0.5 * targets[i] / (1.0 + exp(targets[i] * predictions[i]));
        (*out_gradients)[i] = -grad;
        mse += grad * grad;
    }
    return mse / sample_num;
}
