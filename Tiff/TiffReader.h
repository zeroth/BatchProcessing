#ifndef TIFFREADER_H
#define TIFFREADER_H

#include <string>
#include "Common.h"

namespace zeroth {

struct TiffReaderData;
class TiffReader
{
public:


    TiffReader(const std::string& path);
    ~TiffReader();

    bool next();
    void* data();
    uint32_t width();
    uint32_t height();
    zeroth::SampleFormat dataType() const;
    void close();
    char* description();
    int numberofSlices();

private:
    TiffReaderData *d;

};

}
#endif // TIFFREADER_H
