#ifndef _WORKER_H_
#define _WORKER_H_

#include <common.h>
#include <base_task.h>
#include <msg_queue.h>

namespace lncc {
    class Worker {
    public:
        Worker(MsgQueue<BaseTask*>* queue);
        int Start();
        void Stop();
    private:
        static void* ThreadFunc(void* arg);
    private:
        MsgQueue<BaseTask*>* queue_;
        bool running_;
    };
}

#endif
