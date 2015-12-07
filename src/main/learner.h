#ifndef _LEARNER_H_
#define _LEARNER_H_

namespace lncc {
    class Learner {
    public:
        Learner() {}
        virtual ~Learner() {}
    public:
        virtual int Train() = 0;
        virtual int SaveModel() = 0;
        virtual int PrintFeatureStats() = 0;
    };
}

#endif
