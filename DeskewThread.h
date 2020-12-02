#ifndef DESKEWTHREAD_H
#define DESKEWTHREAD_H

#include <QObject>
#include <QMutex>
#include <QThread>

class DeskewThread : public QThread
{
    Q_OBJECT
public:
    static DeskewThread* instance();

    void add(const QString& j);
    void remove(int index);
    void setOutputDir(const QString& path);
    void setInputDir(const QString& path);
    void setDelay(uint sec);

    // QThread interface
protected:
    void run();

private:
    DeskewThread(QObject *parent = nullptr);

private:
    QString currentFile;
    QVector<QString> jobs;
    QVector<QString> finishedJobs;
    QMutex mutex;
    QString outputDir;
    QString inputDir;
    uint delay;
};

#endif // DESKEWTHREAD_H
