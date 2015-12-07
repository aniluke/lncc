#include <local_task.h>

using namespace lncc;

LocalTask::LocalTask() {
    sem_init(&cond_, 0, 0);
}

LocalTask::~LocalTask() {
    sem_destroy(&cond_);
}

int LocalTask::Wait() {
    sem_wait (&cond_);
    return 0;
}

int LocalTask::Notify() {
    sem_post (&cond_);
    return 0;
}
