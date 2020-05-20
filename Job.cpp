#include "Job.h"
#include <QMap>
#include <QVariant>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QQmlEngine>
#include <QQmlFileSelector>
#include <QQmlFile>
#include <QFileSelector>
#include <QRegExp>

#include <algorithm>

#include "arrayfire.h"
#include "Tiff/BioImage.h"
#include "DeskewRunner.h"
#include "HyperStackRunner.h"

#define M_PI std::atan(1) * 4

void Job::deskewKernel(const QString& inputPath, const QString& outputPath, float dz, float xy, double angle) {

    BioImage image;
    image.setSource(inputPath);

    int nx = image.width();
    int ny = image.height();
    int nz = image.depth();

    af::array afImg;
    if(image.dtype() == zeroth::UINT8) {
        uint8_t* floatArray = (uint8_t*)image.data();
         afImg = af::array(nx, ny, nz, floatArray);
    } else {
        // for now consider it as uin16
        uint16_t* floatArray = (uint16_t*)image.data();
         afImg = af::array(nx, ny, nz, floatArray);
    }

    //shiftc = dzdata * np.abs(np.cos(angle * np.pi / 180)) / dxdata
    double xshift = dz * std::abs(std::cos(angle * M_PI / 180)) / xy;
    int newWidth = nx + std::ceil(nz * xshift);

    af::array afDeskewedImg(newWidth, ny, nz);

    for (int slice = 0; slice < nz; slice++) {
        afDeskewedImg(af::span, af::span, slice) = af::translate(afImg(af::span, af::span, slice), -(xshift * slice), 0, newWidth, afImg.dims(1));
    }

    BioImage imageOut;
    imageOut.setWidth(newWidth);
    imageOut.setHeight(ny);
    imageOut.setDepth(nz);
    imageOut.setDtype(image.dtype());

    if(image.dtype() == zeroth::UINT8){
        afDeskewedImg = afDeskewedImg.as(af::dtype::u8);
        uint8_t* _floatImage = new uint8_t[imageOut.depth()* imageOut.height() * imageOut.width()];
        afDeskewedImg.host(_floatImage);
        imageOut.setData(_floatImage);
        imageOut.save(outputPath);
        delete [] _floatImage;
    }
    else if(image.dtype() == zeroth::UINT16){
        afDeskewedImg = afDeskewedImg.as(af::dtype::u16);
        uint16_t* _floatImage = new uint16_t[imageOut.depth()* imageOut.height() * imageOut.width()];
        afDeskewedImg.host(_floatImage);
        imageOut.setData(_floatImage);
        imageOut.save(outputPath);
        delete [] _floatImage;
    }
}


Job::Job(int id, QObject *parent):QThread(parent), mode(None), id(id)
{
}

Job::~Job()
{
//    qDebug() << "Deleting job";
}

int Job::jobId() const
{
    return id;
}

bool Job::createJob(const QVariantMap &config)
{

    inputDir =  config.value("inputDir", QString()).toString();
    outputDir =  config.value("outputDir").toString();
    deskewAngle =config.value("deskewAngle", 0).toDouble();
    xyPixel =   config.value("xyPixel", 0).toDouble();
    zStep =   config.value("zStep", 0).toDouble();
    channelInfo = qvariant_cast<QStringList>(config.value("channelInfo").toList());
    timePointSelector = config.value("timePointSelector", QString()).toString();
    if(inputDir.isEmpty()) {
        // chances of this are verly less
        qWarning("Missing Input Directory");
        return false;
    }
    inputDir = this->getDirInfo(inputDir).absoluteFilePath();

    // selects the tasks and validate them
    if(config.value("deskew", "0").toInt()) {
       if(!deskewAngle) {
           deskewAngle = 31.0;
           qWarning("Angle is defaulting to 31.0");
       }
       if(!xyPixel) {
           xyPixel= 0.104;
           qWarning("XY is defaulting to 0.104");
       }
       if(!zStep) {
           zStep= 0.4;
           qWarning("XY is defaulting to 0.4");
       }
       deskew = true;
       totalTasks++;
    }
    if(config.value("decon", "0").toInt()) {
        if(channelInfo.isEmpty()){
            return false;
        }
        decon = true;
        totalTasks++;
    }
    if(config.value("bleachCorrect", "0").toInt()) {
        if(channelInfo.isEmpty()){
            return false;
        }
        bleachCorrect = true;
        totalTasks++;
    }
    if(config.value("hyperStack", "0").toInt()) {
        if(channelInfo.isEmpty()){
            return false;
        }
        hyperStack = true;
        totalTasks++;
    }

    // sort the channels
    QDir inputPath(inputDir);
    if(channelInfo.isEmpty() || ((channelInfo.size() == 1) && channelInfo.at(0).isEmpty())) {
        QStringList filter;
        filter << "*.tif" << "*.tiff";
        QFileInfoList tiffFiles = inputPath.entryInfoList(filter, QDir::Files, QDir::Name);
        QString rxPattern = QString("%1[0-9]+").arg(timePointSelector);
        std::sort(tiffFiles.begin(), tiffFiles.end(), [&](const QFileInfo& a, const QFileInfo& b){
            QRegExp rx(rxPattern); // should be replace with the user input for the pattern
            QString _a = a.fileName();
            rx.indexIn(_a);
            _a = rx.cap(0);

            QString _b = b.fileName();
            rx.indexIn(_b);
            _b = rx.cap(0);
            return _a < _b;
        });
        filters["*"] = tiffFiles;
    } else {
        for(const QString& s: channelInfo ) {
            QStringList filter;
            filter << QString("*%1*.tif").arg(s.trimmed()) <<QString("*%1*.tiff").arg(s.trimmed());
            QFileInfoList tiffFiles = inputPath.entryInfoList(filter, QDir::Files, QDir::Name);
            QString rxPattern = QString("%1[0-9]+").arg(timePointSelector);
            std::sort(tiffFiles.begin(), tiffFiles.end(), [&](const QFileInfo& a, const QFileInfo& b){
                QRegExp rx(rxPattern); // should be replace with the user input for the pattern
                QString _a = a.fileName();
                rx.indexIn(_a);
                _a = rx.cap(0);

                QString _b = b.fileName();
                rx.indexIn(_b);
                _b = rx.cap(0);
                return _a < _b;
            });
            filters[s.trimmed()] = tiffFiles;
        }
    }

    if(config.value("hyperStackCreator", "0").toInt()) {
       hyperStackCreator = true;
       totalTasks++;
    }
    return true;
}

void Job::operator()()
{

}

void Job::callDelete()
{
    emit deteleMe();
}

void Job::run()
{
    if(outputDir.isNull() || outputDir.isEmpty()) {
        QDir opPath(inputDir);
        outputDir = opPath.filePath("output");
    }
    QDir().mkdir(outputDir);

    jobStarted(this->id);
    if(deskew) {
        jobUpdate(this->id, "Deskew", "Started", 0);

        QDir opPath(outputDir);
        QString deskewOp = opPath.filePath("Deskewed");
        QDir().mkdir(deskewOp);
        QMap<QString, QFileInfoList>::const_iterator it = filters.constBegin();

        while(it != filters.constEnd()) {
            QFileInfoList newInputList; // output of deskew will be the input for rest
            QString deskewChOp;
            if(it.key() != "*") {
                QDir opPath(deskewOp);
                deskewChOp = opPath.filePath(it.key());
                QDir().mkdir(deskewChOp);
            } else {
                deskewChOp = deskewOp;
            }
            int currentFilterCount = filters.keys().indexOf(it.key());
            int totalFilters = filters.keys().size();
            int totalFiles =  it.value().size() * totalFilters;
            for(int i= 0; i < it.value().size(); i++){
                QFileInfo file = it.value().at(i);
                int currentFileNumber = it.value().size() * currentFilterCount +i;
                float p =((float)currentFileNumber/(float)totalFiles);

                jobUpdate(this->id, QString("Deskew %1").arg(it.key()), "Running", p);

                DeskewRunner dh(file.absoluteFilePath(),
                                QDir(deskewChOp).filePath(file.fileName()),
                                zStep, xyPixel, deskewAngle);

                newInputList << QFileInfo(QDir(deskewChOp).filePath(file.fileName()));
            }
            filters[it.key()] = newInputList;
            ++it;
        }

        jobUpdate(this->id, "Deskew", "Finished", 1);
    }
    if(decon) {
        jobUpdate(this->id, "Decon", "Started", 0);
        jobUpdate(this->id, "Decon", "Finished", 1);
    }
    if(bleachCorrect) {
        jobUpdate(this->id, "Bleach Correct", "Started", 0);
        jobUpdate(this->id, "Bleach Correct", "Finished", 1);
    }
    if(hyperStack) {
        jobUpdate(this->id, "Hyper Stack", "Started", 0);
        QDir opPath(outputDir);
        QString opdir = opPath.filePath("HyperStack");
        QDir().mkdir(opdir);
        int numberOfChannels = filters.size();
        int numberOfVolumesPerChannel = filters.first().size();
        int numberOfPagesPerVolume = 0;
        uint32_t width =0;
        uint32_t height = 0;
        zeroth::SampleFormat dType;
        {
            BioImage im;
            im.setSource(filters.first().first().absoluteFilePath());
            numberOfPagesPerVolume = im.depth();
            width = im.width();
            height = im.height();
            dType = im.dtype();
        }

        QStringList channels = filters.keys();

        HyperStackRunner dh(QDir(opdir).filePath("hyperstack.tif"),numberOfChannels, numberOfVolumesPerChannel,
                            numberOfPagesPerVolume, width, height, dType);


        for(int i =0; i < numberOfVolumesPerChannel; i++) {
            QStringList files;
            for(const QString& channel: channels) {
                files << filters[channel].at(i).absoluteFilePath();
            }
            jobUpdate(this->id, QString("Hyper Stack"), "Running", (float)i/(float)numberOfVolumesPerChannel);
            dh.append(files);
        }
        dh.save();

        jobUpdate(this->id, "Hyper Stack", "Finished", 1);
    }
    if(hyperStackCreator) {
        qDebug() << "Doing Hyper Stack Creator";
    }
    jobEnded(this->id);
}

QFileInfo Job::getDirInfo(const QUrl &pathUrl) const
{
//    QQmlEngine *engine = qmlEngine(this);
//    if (!engine) {
//        qWarning() << "load() called before Job has QQmlEngine";
//        return QFileInfo();
//    }
//    const QUrl path = QQmlFileSelector::get(engine)->selector()->select(pathUrl);
//    const QString fileName = QQmlFile::urlToLocalFileOrQrc(path);
//    const QFileInfo fileInfo(fileName);
//    if(fileInfo.isDir()) {
//        return fileInfo;
//    }
//    QFileInfo dirInfo(fileInfo.dir().absolutePath());
    QUrl url(pathUrl);
    if(url.isValid() && url.scheme().compare("file", Qt::CaseInsensitive) == 0) {
        return QFileInfo(url.toLocalFile());
    } else{
        return QFileInfo(pathUrl.toString());
    }
}
