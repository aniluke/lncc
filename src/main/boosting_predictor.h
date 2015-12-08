#ifndef _BOOSTING_PREDICTOR_H_
#define _BOOSTING_PREDICTOR_H_

#include <common.h>
#include <predictor.h>
#include <ln_context.h>
#include <base_function.h>

namespace lncc {
    class BoostingPredictor: public Predictor {
    public:
        BoostingPredictor(const LnContext* ln_ctx):ln_ctx_(ln_ctx) {}
        virtual ~BoostingPredictor();

    public:
        virtual int Predict();
        virtual int LoadModel();

    private:
        int PredictIterations(int max_iters);
        
    private:
        const LnContext* ln_ctx_;
        DataSet data_set_;
        std::vector<BaseFunction*> base_funcs_;

    };
}


#endif
