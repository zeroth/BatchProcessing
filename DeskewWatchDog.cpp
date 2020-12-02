#include <QCoreApplication>
#include "DeskewRunner.h"
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileSystemWatcher>
#include <QSet>
#include "DeskewThread.h"
#include <signal.h>

void handle_ctrl_c(int signal) {
    qDebug() << "About to close the application ";
    qApp->quit();
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Directory watcher task");
    parser.addHelpOption();

    parser.addPositionalArgument("input", "Input Dir");//{{"i", "input"}, "Input file path", "file"});
    parser.addPositionalArgument("output", "Output Dir");

    // Change wait period
    QCommandLineOption waitOption(QStringList() << "d" << "delay",
                                  QCoreApplication::translate("main", "delay before starting the deskew."),
                                  QCoreApplication::translate("main", "2"));
    parser.addOption(waitOption);
    parser.process(app);

    QString delay = parser.value(waitOption);
//    delay.toInt();
    uint delaySec = 0;
    bool ok;
    delaySec = delay.toUInt(&ok);
    if(!ok)
        delaySec = 2;

    const QStringList args = parser.positionalArguments();
    if(args.isEmpty() || args.length() < 2) {
        parser.showHelp(0);
    }
    QString inputDir(args[0]);
    QString outputDir(args[1]);

    if(!QDir(outputDir).exists()) {
        // create output dir
        QDir().mkdir(outputDir);
    }


    qDebug () << "Input dir " << inputDir;
    qDebug () << "Output dir "<< outputDir;
    qDebug () << "Delay "<< delaySec;
    DeskewThread::instance()->setInputDir(inputDir);
    DeskewThread::instance()->setOutputDir(outputDir);
    DeskewThread::instance()->setDelay(delaySec);
    QStringList filter;
    filter << "*.tif" << "*.tiff";

    QFileSystemWatcher fsw;
    fsw.addPath(inputDir);
    QStringList initialList = QDir(inputDir).entryList(filter, QDir::Files);

    qDebug() << "Watching " << fsw.directories();

    QObject::connect(&fsw, &QFileSystemWatcher::directoryChanged, &app, [&](const QString& path){

        QStringList currentList = QDir(inputDir).entryList(filter, QDir::Files);
        QSet <QString>initSet = initialList.toSet();
        QSet <QString>currentSet = currentList.toSet();

        QSet <QString>newSet = currentSet.subtract(initSet);
        foreach (const QString & fileName, newSet) {
            fsw.addPath(QDir(inputDir).absoluteFilePath(fileName));
        }

        initialList = QDir(inputDir).entryList(filter, QDir::Files);
    });

    QObject::connect(&fsw, &QFileSystemWatcher::fileChanged, &app, [&](const QString& path){
        qDebug() << "adding new path "<< path;
        DeskewThread::instance()->add(path);
    });


//    signal(SIGINT, [&](int s){
//        qDebug() << "About to quit";
//        DeskewThread::instance()->quit();
//       app.quit();
//    });
    signal(SIGINT, handle_ctrl_c);


    return app.exec();
}
