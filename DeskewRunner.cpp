#include "DeskewRunner.h"
#include "arrayfire.h"
#include "Tiff/BioImage.h"
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QFileSelector>
#include <QDebug>

#define M_PI std::atan(1) * 4

DeskewRunner::DeskewRunner(const QString& input, const QString& output,
                           double dz, double xy, double angle, bool save8Bit)
    :input(input), output(output), dz(dz), xy(xy), angle(angle), save8Bit(save8Bit)
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
    qDebug () << Q_FUNC_INFO;
    BioImage image;
    image.setSource(input);
    qDebug() << "image.dtype() " << image.dtype();
    uint32_t nx = image.width();
    uint32_t ny = image.height();
    uint32_t nz = image.depth();

    try {


    af::array afImg;
    if(image.dtype() == zeroth::UINT8) {
        uint8_t* floatArray = (uint8_t*)image.data();
         afImg = af::array(nx, ny, nz, floatArray);
    } else {
        // for now consider it as uin16
        uint16_t* floatArray = (uint16_t*)image.data();
         afImg = af::array(nx, ny, nz, floatArray);
    }


//    try {

//    } catch (...) {
//    }
    qDebug() << "AF array done ";
    //shiftc = dzdata * np.abs(np.cos(angle * np.pi / 180)) / dxdata
    double xshift = dz * std::abs(std::cos(angle * M_PI / 180)) / xy;
    uint32_t newWidth = nx + std::ceil(nz * xshift);

    af::array afDeskewedImg(newWidth, ny, nz);

    for (uint32_t slice = 0; slice < nz; slice++) {
        afDeskewedImg(af::span, af::span, slice) = af::translate(afImg(af::span, af::span, slice),
                                                                 -(xshift * slice), 0,
                                                                 newWidth, afImg.dims(1));
    }
    qDebug() << "Deskew done " << output;

    BioImage imageOut;
    imageOut.setWidth(newWidth);
    imageOut.setHeight(ny);
    imageOut.setDepth(nz);
    imageOut.setDtype(image.dtype());

    if(image.dtype() == zeroth::UINT8){
        qDebug() << "if(image.dtype() == zeroth::UINT8)";
        afDeskewedImg = afDeskewedImg.as(af::dtype::u8);
        uint8_t* _floatImage = new uint8_t[imageOut.depth()* imageOut.height() * imageOut.width()];
        afDeskewedImg.host(_floatImage);
        imageOut.setData(_floatImage);
        imageOut.save(output);
        delete [] _floatImage;
    }
    else if(image.dtype() == zeroth::UINT16){
        qDebug() << "if(image.dtype() == zeroth::UINT16)";
        afDeskewedImg = afDeskewedImg.as(af::dtype::u16);
        uint16_t* _floatImage = new uint16_t[imageOut.depth()* imageOut.height() * imageOut.width()];
        afDeskewedImg.host(_floatImage);
        imageOut.setData(_floatImage);
        if(save8Bit)
            imageOut.save8Bit(output);
        else
            imageOut.save(output);
        delete [] _floatImage;
    }
    } catch (af::exception& e) {
      qDebug() << e.what();
    }

}
