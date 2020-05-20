#ifndef JOBPRODUCER_H
#define JOBPRODUCER_H

#include <QObject>
#include <QAbstractListModel>
#include <QMap>
#include "Job.h"
class JobModel;

class JobProducer : public QObject
{
    Q_OBJECT
public:
    static JobProducer* instance();
    Q_INVOKABLE bool createJob(const QVariantMap& config);
    Q_INVOKABLE JobModel* mode(){return jobModel;};

private:
    JobProducer(QObject *parent = nullptr);
    QMap<int, Job*> jobs;
    JobModel* jobModel;
};


///////////////////////////////////// Job Diaplay
class JobModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        JobId = Qt::UserRole + 1,
        Status,
        Process,
        Progress,
        StartTime,
        EndTime,
        Path
    };

    JobModel(QObject *parent = nullptr);

    Q_INVOKABLE void addJob(Job *job);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant  headerData(int section, Qt::Orientation orientation, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void jobUpdate(int jobId, const QString& taskName, const QString& state, float progress);
    void jobStarted(int jobId);
    void jobEnded(int jobId);
//    void addBook(const QString &title, const QString &author, const QString &price, const QString &notes);
private:
    QMap<int, QVariantList> jobs;
};

#endif // JOBPRODUCER_H
