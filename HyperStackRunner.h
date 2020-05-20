#ifndef HYPERSTACKRUNNER_H
#define HYPERSTACKRUNNER_H

#include <QString>
#include "Tiff/BioImage.h"
#include "Tiff/TiffWriter.h"
#include <QMap>
#include <QFileInfoList>

class HyperStackRunner
{
public:
    HyperStackRunner(const QString& outputFile, int numberOfChannels=0,int numberOfVolumesPerChannel = 0,
                     int numberOfPagesPerVolume = 0, uint32_t width = 0, uint16_t height=0,
                     zeroth::SampleFormat dType=zeroth::UINT8);

    ~HyperStackRunner();

    void append(const QStringList&  inputs);
    void run();
    void save();

    QString iJdescription() const;
private:
    QString outputFile;
    int numberOfChannels = 0;
    int numberOfVolumesPerChannel = 0;
    int numberOfPagesPerVolume = 0;
    zeroth::TiffWriter *outputImage;
    QMap<QString, QFileInfoList> filters;
    uint32_t width;
    uint32_t height;
    zeroth::SampleFormat dType;
    bool isBigTiff = false;
    bool isAppend = false;


};

#endif // HYPERSTACKRUNNER_H
