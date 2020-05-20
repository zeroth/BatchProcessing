#include "JobProducer.h"
#include <QApplication>
#include "JobQueue.h"
#include <QStandardItem>
#include <QDateTime>
#include <QTime>
#include <QMimeData>
#include <QDebug>

JobProducer *JobProducer::instance()
{
    static JobProducer* _instance = new JobProducer(qApp);
    return _instance;
}

bool JobProducer:: createJob(const QVariantMap &config)
{
    QStringList dirList;
         dirList = qvariant_cast<QStringList>(config.value("inputDir"));

//    if(dirList.isEmpty()){
//        Job *j = new Job(jobs.size()+1);

//        if(!j->createJob(config)) {
//            delete j;
//            return false;
//        }


//        jobs[j->jobId()] = j;
//        jobModel->addJob(j);
////        JobQueue::instance()->add(j);
//    } else {
        for(QString s: dirList) {
            QVariantMap localConfig = config;
            localConfig["inputDir"] = s;
//            qDebug() << localConfig;
//            qDebug() << config;

            Job *j = new Job(jobs.size()+1);

            if(!j->createJob(localConfig)) {
                delete j;
                return false;
            }


            jobs[j->jobId()] = j;
            jobModel->addJob(j);
            JobQueue::instance()->add(j);
        }
//    }
    return true;
}



JobProducer::JobProducer(QObject *parent) : QObject(parent), jobModel(new JobModel(this))
{

}



////////////////////////////////////////////////////////// Job Display

JobModel::JobModel(QObject *parent): QAbstractListModel(parent)
{
}

QVariant JobModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= jobs.size())
        return QVariant();

    QVariantList job = jobs[index.row()];

    if (role == JobId)
        return job[0];
    else if (role == Status)
        return job[1];
    else if (role == Process)
        return job[2];
    else if (role == Progress)
        return job[3];
    else if (role == StartTime)
        return job[4];
    else if (role == EndTime)
        return job[5];
    else if (role == Path)
        return job[6];
    return QVariant();
}

int JobModel::rowCount(const QModelIndex &parent) const
{
    return jobs.size();
}

QVariant JobModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == JobId)
        return QString("Id");
    else if (role == Status)
        return QString("Status");
    else if (role == Process)
        return QString("Process");
    else if (role == Progress)
        return QString("Progress");
    else if (role == StartTime)
        return QString("StartTime");
    else if (role == EndTime)
        return QString("EndTime");
    else if (role == Path)
        return QString("Path");
    return QVariant();
}

QHash<int, QByteArray> JobModel::roleNames() const
{
    QHash<int, QByteArray> mapping;

    mapping[JobId] = "jobid";
    mapping[Status] = "status";
    mapping[Process] = "process";
    mapping[Progress] = "progress";
    mapping[StartTime] = "startTime";
    mapping[EndTime] = "endTime";
    mapping[Path] = "path";

    return mapping;
}

void JobModel::addJob(Job* j)
{
    int row = jobs.size();
    QVariantList job;
    job << QString::number(j->jobId())
        << "Queued"
        << "."
        << 0
        << "."
        << "."
        << j->inputPath();

    beginInsertRows(QModelIndex(), row, row);
    jobs[j->jobId()-1] = job;
    endInsertRows();
    connect(j, &Job::jobUpdate, this, &JobModel::jobUpdate);
    connect(j, &Job::jobStarted, this, &JobModel::jobStarted);
    connect(j, &Job::jobEnded, this, &JobModel::jobEnded);
//    connect(j, &Job::finished, this, &QObject::deleteLater);
}

void JobModel::jobUpdate(int jobId, const QString &process, const QString &state, float progress)
{
//    qDebug() << Q_FUNC_INFO << jobId << process << state << progress;

    jobs[jobId -1][1] = state;
    jobs[jobId -1][2] = process;
    jobs[jobId -1][3] = progress;

    QModelIndex index = createIndex(jobs.keys().indexOf(jobId-1), 0, &jobs[jobId -1]);
    emit dataChanged(index, index, roleNames().keys().toVector());
}

void JobModel::jobStarted(int jobId)
{
//    jobModel->setData(jobModel->index(jobId-1, 0), QDateTime::currentDateTime().toLocalTime().toString(), JobModel::StartTime);
    jobs[jobId -1][4] = QTime::currentTime().toString(Qt::ISODateWithMs);

    QModelIndex index = createIndex(jobs.keys().indexOf(jobId-1), 0, &jobs[jobId -1]);
    emit dataChanged(index, index, roleNames().keys().toVector());
}

void JobModel::jobEnded(int jobId)
{
    jobs[jobId -1][5] = QTime::currentTime().toString(Qt::ISODateWithMs);

    QModelIndex index = createIndex(jobs.keys().indexOf(jobId-1), 0, &jobs[jobId -1]);
    emit dataChanged(index, index, roleNames().keys().toVector());
//    jobModel->setData(jobModel->index(jobId-1, 0), QDateTime::currentDateTime().toLocalTime().toString(), JobModel::EndTime);
}

