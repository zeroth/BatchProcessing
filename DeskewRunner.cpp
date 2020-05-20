#include "DeskewRunner.h"
#include "arrayfire.h"
#include "Tiff/BioImage.h"
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QQmlEngine>
#include <QQmlFileSelector>
#include <QQmlFile>
#include <QFileSelector>
#define M_PI std::atan(1) * 4

DeskewRunner::DeskewRunner(const QString& inputList, const QString& outputList, float dz, float xy, double angle)
    :input(inputList), output(outputList), dz(dz), xy(xy), angle(angle)
{
    run();
}

QString DeskewRunner::inputFile() const {
    return input;
}

QString DeskewRunner::outputFile() const {
    return output;
}

void DeskewRunner::run()
{
    BioImage image;
    image.setSource(input);

    uint32_t nx = image.width();
    uint32_t ny = image.height();
    uint32_t nz = image.depth();

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
    uint32_t newWidth = nx + std::ceil(nz * xshift);

    af::array afDeskewedImg(newWidth, ny, nz);

    for (uint32_t slice = 0; slice < nz; slice++) {
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
        imageOut.save(output);
        delete [] _floatImage;
    }
    else if(image.dtype() == zeroth::UINT16){
        afDeskewedImg = afDeskewedImg.as(af::dtype::u16);
        uint16_t* _floatImage = new uint16_t[imageOut.depth()* imageOut.height() * imageOut.width()];
        afDeskewedImg.host(_floatImage);
        imageOut.setData(_floatImage);
        imageOut.save(output);
        delete [] _floatImage;
    }

}
