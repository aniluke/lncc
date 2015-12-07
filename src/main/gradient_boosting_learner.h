#ifndef _GRADIENT_BOOSTING_LEARNER_H_
#define _GRADIENT_BOOSTING_LEARNER_H_

#include <common.h>
#include <learner.h>
#include <ln_context.h>
#include <base_function.h>

namespace lncc {
    class GradientBoostingLearner: public Learner {
    public:
        GradientBoostingLearner(const LnContext* ln_ctx):ln_ctx_(ln_ctx) {}
        virtual ~GradientBoostingLearner();

    public:
        virtual int Train();
        virtual int SaveModel();
        virtual int PrintFeatureStats();

    private:
        const LnContext* ln_ctx_;
        DataSet data_set_;
        std::vector<BaseFunction*> base_funcs_;
    };
}

#endif
