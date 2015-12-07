#include <worker.h>

using namespace lncc;

Worker::Worker(MsgQueue<BaseTask*>* queue):
    queue_(queue),
    running_(false) {
    
}

void* Worker::ThreadFunc(void* arg) {
    Worker* worker = (Worker*)arg;
    while (worker->running_) {
        BaseTask* task = worker->queue_->take();
        task->DoTask();
        task->Notify();
    }
    return NULL;
}

int Worker::Start() {
    running_ = true;
    pthread_t th;
    pthread_create(&th, NULL, Worker::ThreadFunc, this );
    return 0;
}

void Worker::Stop() {
    running_ = false;
}
