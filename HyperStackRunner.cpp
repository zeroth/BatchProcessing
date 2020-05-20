#include "HyperStackRunner.h"
#include <QString>
#include <string>
#include "Tiff/TiffReader.h"
#include <QDebug>

HyperStackRunner::HyperStackRunner(const QString& outputFile,  int numberOfChannels,int numberOfVolumesPerChannel,
                                   int numberOfPagesPerVolume, uint32_t width, uint16_t height,zeroth::SampleFormat dType):

    outputFile(outputFile),numberOfChannels(numberOfChannels), numberOfVolumesPerChannel(numberOfVolumesPerChannel),
    numberOfPagesPerVolume(numberOfPagesPerVolume), width(width), height(height), dType(dType)
{
    run();
}

HyperStackRunner::~HyperStackRunner()
{
}

void HyperStackRunner::append(const QStringList &inputs)
{

    //    zeroth::TiffWriter outputImage(outputFile.toStdString(), width, height, dType, std::string(), isBigTiff, isAppend);
    QList<zeroth::TiffReader*> images;
    for(const QString& file: inputs) {
        zeroth::TiffReader* image = new zeroth::TiffReader(file.toStdString());
        images.append(image);
    }


    for(int slice =0; slice < numberOfPagesPerVolume; slice++) {
        for(int i =0; i < images.length(); i++) {
            outputImage->write(images[i]->data());
            images[i]->next();
        }
    }

    for(zeroth::TiffReader* im : images) {
        im->close();
        delete im;
    }
    images.clear();
    isAppend = true;
}

void HyperStackRunner::run()
{

    // 4g 4294967296
    uint16_t byteSize = 1;
    switch (dType) {

    case zeroth::INT8:
    case zeroth::UINT8:{
        byteSize = 1;
        break;
    }
    case zeroth::UINT16:
    case zeroth::INT16:{
        byteSize = 2;
        break;
    }


    }

    // check if its bigTiff
    uint64_t expectedSize = (uint64_t)width * (uint64_t)height * (uint64_t)numberOfChannels * (uint64_t)numberOfVolumesPerChannel * (uint64_t)numberOfPagesPerVolume * (uint64_t)byteSize;
    uint64_t limit = std::pow(2,32);
    isBigTiff = false;
    if(expectedSize > limit)
        isBigTiff = true;

    outputImage = new zeroth::TiffWriter(outputFile.toStdString(), width, height, dType, std::string(), isBigTiff); // this->iJdescription().toStdString()
}

void HyperStackRunner::save()
{
    outputImage->close();
}

QString HyperStackRunner::iJdescription() const
{
    /*
        ImageJ=1.52p
        images=40
        channels=2
        slices=4
        frames=5
        hyperstack=true
        mode=composite
        loop=false
        min=0.0
        max=65535.0

    */
    /*
ImageJ=1.52p
images=2020
channels=2
slices=101
frames=10
hyperstack=true
mode=color
loop=false
min=0.0
max=170.0

    */
    QStringList descriptionList;
    descriptionList << QString("ImageJ=1.52p")
                    << QString("images=%1").arg(QString::number(numberOfChannels*numberOfPagesPerVolume*numberOfVolumesPerChannel))
                    << QString("channels=%1").arg(numberOfChannels)
                    << QString("slices=%1").arg(numberOfPagesPerVolume)
                    << QString("frames=%1").arg(numberOfVolumesPerChannel)
                    << QString("hyperstack=true")
                    << QString("mode=color")
                    << QString("loop=false");
    //                    << QString("min=0.0")
    //                    << QString("max=65535.0");
    qDebug() << descriptionList.join("\n").toStdString().c_str();

    return descriptionList.join("\n");
}
