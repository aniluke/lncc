#ifndef _BASE_TASK_H_
#define _BASE_TASK_H_

namespace lncc {
    class BaseTask {
    public:
        BaseTask() {}
        virtual ~BaseTask() {}
    public:
        virtual void DoTask() = 0;
        virtual int Wait() = 0;
        virtual int Notify() = 0;
    };
}
#endif
