#include "least_square_loss_function.h"
using namespace std;

double lncc::LeastSquareLossFunction::ComputeNegativeGradient(vector<double>* out_gradients,
                                                   const vector<double>& targets,
                                                   const vector<double>& predictions) {
    int sample_num = targets.size();
    if (sample_num == 0)
        return 0;
    
    double mse = 0.0;
    for (int i = 0; i < sample_num; i ++) {
        // TODO: more loss function support
        // least-square
        double grad = predictions[i] - targets[i];
        (*out_gradients)[i] = -grad;
        mse += grad * grad;
    }
    return mse / sample_num;
}
