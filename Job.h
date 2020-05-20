#ifndef JOB_H
#define JOB_H

#include <QThread>
#include <QIODevice>
#include <QMap>
#include <QFileInfoList>

class Job : public QThread
{
    Q_OBJECT
public:

    enum Type{
        None,
        Deskew,
        Decon,
        BleachCorrect,
        HyperStack,
        HyperStackCreator
    };
    Q_DECLARE_FLAGS(Types, Type)
    Job(int id, QObject *parent = nullptr);
    ~Job();
    int jobId() const;
    bool createJob(const QVariantMap& config);
    void operator()();
    void deskewKernel(const QString& inputPath, const QString& outputPath, float dz, float xy, double angle);
    QString inputPath() const {
        return inputDir;
    }
public slots:
    void callDelete();

    // QThread interface
protected:
    void run();
    QFileInfo getDirInfo(const QUrl &pathUrl) const;

signals:
    void jobStarted(int jobId);
    void jobUpdate(int jobId, const QString& taskName, const QString& state, float progress);
    void jobEnded(int jobId);
    void deteleMe();

private:
    Types mode;
    int id = 0;
    QString inputDir;
    QString outputDir;
    qreal   deskewAngle = 0;
    qreal   xyPixel = 0;
    qreal   zStep = 0;
    QStringList channelInfo;
    bool deskew = false;
    bool decon = false;
    bool bleachCorrect = false;
    bool hyperStack = false;
    bool hyperStackCreator = false;
    QMap<QString, QFileInfoList> filters;
    int totalTasks =0;
    QString timePointSelector;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Job::Types)


#endif // JOB_H
