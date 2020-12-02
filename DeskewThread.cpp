#include "DeskewThread.h"
#include <QMutexLocker>
#include <QDebug>
#include "DeskewRunner.h"
#include <QDir>

DeskewThread *DeskewThread::instance()
{
    static DeskewThread* _instance = new DeskewThread();
    return  _instance;
}

void DeskewThread::add(const QString &j)
{
    QMutexLocker locker(&mutex);
    jobs.append(j);
    if(!this->isRunning()) {
        this->start();
    } else {
        qDebug() << "Is already running";
    }
}

void DeskewThread::remove(int index)
{
    QMutexLocker locker(&mutex);
    if(jobs.size() > index)
        return;

    QString j = jobs.takeAt(index);
}

void DeskewThread::setOutputDir(const QString &path)
{
    outputDir = path;
}

void DeskewThread::setInputDir(const QString &path)
{
    inputDir = path;
}

void DeskewThread::setDelay(uint sec)
{
    delay = sec;
}

void DeskewThread::run()
{
    qDebug() << Q_FUNC_INFO;
    if(jobs.isEmpty())
        return;
    while(jobs.size()){
        currentFile = jobs.takeFirst();
        currentFile = QFileInfo(currentFile).fileName();
        qDebug() << "Processing " << currentFile << " OP: " << outputDir;
//        qDebug() << "Delay " << delay;
//        QThread::sleep(delay);
        QString inputPath = QDir(inputDir).absoluteFilePath(currentFile);
        QString outputPath = QDir(outputDir).absoluteFilePath(currentFile);
        qDebug() << "Processing " << inputPath << " OPD: "<< outputDir << "  OP:  " << outputPath;
        DeskewRunner deskew(inputPath,
                            outputPath, 0.4, 0.104, 31.8, true);
        qDebug() << "Done " << outputPath;
    }
}

DeskewThread::DeskewThread(QObject *parent) : QThread(parent), delay(2)
{
    // Chill
}

