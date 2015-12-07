#ifndef _LOCAL_TASK_H_
#define _LOCAL_TASK_H_

#include <base_task.h>
#include <semaphore.h>

namespace lncc {
    class LocalTask: public BaseTask {
    public:
        LocalTask();
        virtual ~LocalTask();
    public:
        virtual int Wait();
        virtual int Notify();
    private:
        sem_t cond_;
    };
}
#endif
