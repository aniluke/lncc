#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

namespace lncc {
    class Predictor {
    public:
        Predictor() {}
        virtual ~Predictor() {}

    public:
        virtual int Predict() = 0;
        virtual int LoadModel() = 0;
    };
}
#endif
