#include <QCoreApplication>
#include "DeskewRunner.h"
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;
    parser.setApplicationDescription("Single file deskew task");
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
    if(args.isEmpty()) {
        parser.showHelp(0);
    }
    QDir inputDir(args[0]);
    QDir outputDir;
    if(args.length() > 1) {
        // we have input and output path
        outputDir = QDir(args[1]);
    } else {
        outputDir = QDir(inputDir.filePath("output"));
    }

    // create output dir
    QDir().mkdir(outputDir.absolutePath());

    // create deskew Dir
    QDir deskewDir = QDir(outputDir.filePath("deskewed"));

    // create deskews dir
    QDir().mkdir(deskewDir.absolutePath());

    qDebug () << "Input dir " << inputDir.absolutePath();
    qDebug () << "Output dir "<< outputDir.absolutePath();


    QStringList filter;
    filter << "*.tif" << "*.tiff";
    QFileInfoList tiffFiles = inputDir.entryInfoList(filter, QDir::Files, QDir::Name);
    int totalCount = tiffFiles.length();
    qDebug () << "Total input files " << totalCount;

    for(int i = 0; i < totalCount; i++) {
        QFileInfo fileInfo = tiffFiles.at(i);
        qDebug() << "Processing " << i+1 << "/" <<  totalCount  << " : "<< fileInfo.fileName();
        DeskewRunner deskew(fileInfo.absoluteFilePath(),
                            deskewDir.absoluteFilePath(fileInfo.fileName()), 0.4, 0.104, 31.8);
    }

    return 0;
}
