#include "BioImage.h"
#include "TiffReader.h"
#include "TiffWriter.h"
#include "Common.h"
#include <QMap>
#include <QDebug>
#include<cstring>
uint8_t* testToUint8(uint8_t*) {
    return nullptr;
}

struct BioImageData {
    QString path;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    zeroth::SampleFormat dType;
    double min;
    double max;
    uint8_t* buffer;
    std::vector<uint8_t> m_buffer;
    uint16_t bitsPerSample = 8;
    uint32_t imageSize = 0;
    uint32_t pageSize =0;
    QMap<zeroth::SampleFormat, uint16_t> sampleMap;
    bool isUpdated = false;

    BioImageData(){
        createSampleMap();
        isUpdated = false;
    }
    ~BioImageData(){
        delete [] buffer;
    }
    void createSampleMap() {
        sampleMap[zeroth::UINT8] = 8;
        sampleMap[zeroth::UINT16] = 16;
        sampleMap[zeroth::UINT32] = 32;
        sampleMap[zeroth::INT8] = 8;
        sampleMap[zeroth::INT16] = 16;
        sampleMap[zeroth::INT32] = 32;
        sampleMap[zeroth::FLOAT] = 32;
    }
    uint16_t bytesCnt() {
        return sampleMap[dType]/8;
    }

    template<typename T>
    double getMin() {
        return *std::min_element((T*)buffer, (T*)buffer+imageSize);
    }

    template<typename T>
    double getMax() {
        return *std::max_element((T*)buffer, (T*)buffer+imageSize);
    }

    template<typename T>
    T* convert() {
        T *result = new T[imageSize];
        switch (dType) {
        case zeroth::UINT8: {
            uint8_t* buff = (uint8_t*)buffer;
            for(uint32_t i = 0; i < imageSize; i++) {
                result[i] = (T)buff[i];
            }
            break;
        }
        case zeroth::UINT16: {
            uint16_t* buff = (uint16_t*)buffer;
            for(uint32_t i = 0; i < imageSize; i++) {
                result[i] = (T)buff[i];
            }
            break;
        }
        case zeroth::UINT32: {
            uint32_t* buff = (uint32_t*)buffer;
            for(uint32_t i = 0; i < imageSize; i++) {
                result[i] = (T)buff[i];
            }
            break;
        }
        case zeroth::INT8: {
            int8_t* buff = (int8_t*)buffer;
            for(uint32_t i = 0; i < imageSize; i++) {
                result[i] = (T)buff[i];
            }
            break;
        }
        case zeroth::INT16: {
            int16_t* buff = (int16_t*)buffer;
            for(uint32_t i = 0; i < imageSize; i++) {
                result[i] = (T)buff[i];
            }
            break;
        }
        case zeroth::INT32: {
            int32_t* buff = (int32_t*)buffer;
            for(uint32_t i = 0; i < imageSize; i++) {
                result[i] = (T)buff[i];
            }
            break;
        }
        case zeroth::FLOAT: {
            float* buff = (float*)buffer;
            for(uint32_t i = 0; i < imageSize; i++) {
                result[i] = (T)buff[i];
            }
            break;
        }
        default: {
            uint8_t* buff = (uint8_t*)buffer;
            for(uint32_t i = 0; i < imageSize; i++) {
                result[i] = (T)buff[i];
            }
        }
        }

        return  result;
    }


    void readBuffer(zeroth::TiffReader& reader) {
        buffer = new uint8_t[width * height * depth* bytesCnt()];
//        m_buffer.reserve(width * height * depth* bytesCnt());
        for(uint32_t i = 0; i < depth; i++) {
            uint32_t startPoint = i* width * height * bytesCnt();
            memcpy(buffer+startPoint, reader.data(), (width * height) * bytesCnt() );
            reader.next();
        }
    }

    void update(){
        switch (dType) {
        case zeroth::UINT8:{
            bitsPerSample =8;
            min = getMin<uint8_t>();
            max = getMax<uint8_t>();
            break;
        }
        case zeroth::UINT16:{
            bitsPerSample = 16;
            min = getMin<uint8_t>();
            max = getMax<uint8_t>();
            break;
        }
        case zeroth::UINT32:{
            bitsPerSample = 32;
            min = getMin<uint8_t>();
            max = getMax<uint8_t>();
            break;
        }
        case zeroth::INT8: {
            bitsPerSample =8;
            min = getMin<uint8_t>();
            max = getMax<uint8_t>();
            break;
        }
        case zeroth::INT16:{
            bitsPerSample =16;
            min = getMin<uint8_t>();
            max = getMax<uint8_t>();
            break;
        }
        case zeroth::INT32:{
            bitsPerSample =32;
            min = getMin<uint8_t>();
            max = getMax<uint8_t>();
            break;
        }
        case zeroth::FLOAT: {
            bitsPerSample =32;
            min = getMin<uint8_t>();
            max = getMax<uint8_t>();
            break;
        }
        default:{
            bitsPerSample =8;
            min = getMin<uint8_t>();
            max = getMax<uint8_t>();
        }
        }
        isUpdated = true;
    }

    void load() {
        // TODO: find the reader and use it
        // for now assuming its tiff
        zeroth::TiffReader reader(path.toStdString());
        width = reader.width();
        height = reader.height();
        depth = reader.numberofSlices();
        dType = reader.dataType();
        imageSize = width * height * depth;
        pageSize = width* height* bytesCnt();
        // read the data
        readBuffer(reader);
        isUpdated = false;
        reader.close();
    }
};

BioImage::BioImage(QObject *parent) : QObject(parent), d(new BioImageData)
{
}

BioImage::~BioImage()
{
    delete d;
}

QString BioImage::source() const
{
    return d->path;
}

void BioImage::setSource(const QString &val)
{
    d->path = val;
    d->load();
}

void BioImage::update()
{
    // if this is getting called it means the with, height, depth, dataType and data are set
    d->update();
}

uint32_t BioImage::width() const
{
    return d->width;
}

void BioImage::setWidth(uint32_t val)
{
    d->width = val;
}

uint32_t BioImage::height() const
{
    return d->height;
}

void BioImage::setHeight(uint32_t val)
{
    d->height = val;
}

uint32_t BioImage::depth() const
{
    return d->depth;
}

void BioImage::setDepth(uint32_t val)
{
    d->depth = val;
}

zeroth::SampleFormat BioImage::dtype() const
{
    return d->dType;
}

void BioImage::setDtype(zeroth::SampleFormat format)
{
    d->dType = format;
}

uint32_t BioImage::byteSize() const
{
    return  d->pageSize * d->depth;
}

double BioImage::imageMin() const
{
    if(!d->isUpdated) d->update();
    return d->min;
}

double BioImage::imageMax() const
{
    if(!d->isUpdated) d->update();
    return d->max;
}

void *BioImage::data()
{
    return d->buffer;
}

void BioImage::setData(void *buffer)
{
    // considering the width, height, depth and dType are set before reaching here this makes it really fast but very unsafe.
//    d->buffer = (uint8_t*)buffer; // memcpy is your friend
    d->imageSize = d->width*d->height*d->depth;
    d->pageSize = d->width*d->height* d->bytesCnt();
    d->buffer = new uint8_t[d->imageSize*d->bytesCnt()];
    memcpy(d->buffer, buffer, d->imageSize*d->bytesCnt());
}

void *BioImage::pageAt(int index)
{
    uint32_t startPoint = index* d->pageSize;
    return d->buffer+startPoint;
}

uint32_t BioImage::pageSize() const
{
    return d->pageSize;
}

uint8_t *BioImage::toUint8()
{
    return d->convert<uint8_t>();
}

float *BioImage::toFloat()
{
    return d->convert<float>();
}

void BioImage::save(const QString& path)
{
    zeroth::TiffWriter im(path.toStdString(), d->width, d->height, d->dType);
    for(uint32_t i = 0; i < d->depth; i++) {
        uint32_t startPoint = i* d->pageSize;
        uint8_t* tmp = new uint8_t[d->pageSize];
        memcpy(tmp, (uint8_t*)d->buffer+startPoint, d->pageSize );
        im.write(tmp);
        delete [] tmp;
    }
    im.close();
}

void BioImage::save8Bit(const QString& path)
{
    zeroth::TiffWriter im(path.toStdString(), d->width, d->height, zeroth::SampleFormat::UINT8);
    uint32_t pSize =  d->width * d->height;
    uint8_t * bff = this->toUint8();
    for(uint32_t i = 0; i < d->depth; i++) {
        uint32_t startPoint = i* pSize;
        uint8_t* tmp = new uint8_t[pSize];
        memcpy(tmp, (uint8_t*)bff+startPoint, pSize);
        im.write(tmp);
        delete [] tmp;
    }
    im.close();
}


template<typename T>
T *BioImage::data()
{
    return d->convert<T>();
}
