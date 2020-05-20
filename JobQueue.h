#ifndef JOBQUEUE_H
#define JOBQUEUE_H

#include <QObject>
#include "Job.h"
#include <QMutex>
#include <QThread>

class JobQueue : public QThread
{
    Q_OBJECT
public:
    static JobQueue* instance();

    void add(Job* j);
    void remove(int index);

    // QThread interface
protected:
    void run();

private:
    JobQueue(QObject *parent = nullptr);

private:
    Job* currentJob;
    QVector<Job*> jobs;
    QVector<Job*> finishedJobs;
    QMutex mutex;

};

#endif // JOBQUEUE_H
