#include "JobQueue.h"
#include <QMutexLocker>

JobQueue *JobQueue::instance()
{
    static JobQueue* _instance = new JobQueue();
    return  _instance;
}

void JobQueue::add(Job *j)
{
    QMutexLocker locker(&mutex);
    jobs.append(j);
    if(!this->isRunning())
        this->start();
}

void JobQueue::remove(int index)
{
    QMutexLocker locker(&mutex);
    if(jobs.size() > index)
        return;

    Job* j = jobs.takeAt(index);
    j->callDelete();
}

void JobQueue::run()
{
    if(jobs.isEmpty())
        return;
    while(jobs.size()){
        currentJob = jobs.takeFirst();
        currentJob->start();
        currentJob->wait();
        currentJob->deleteLater();
    }
}

JobQueue::JobQueue(QObject *parent) : QThread(parent), currentJob(nullptr)
{
    // Chill
}
