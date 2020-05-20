#ifndef DESKEW_H
#define DESKEW_H

#include <QString>
#include <QList>

class DeskewRunner
{
public:
    DeskewRunner(const QString& inputList, const QString& outputList, float dz, float xy, double angle);
    QString inputFile()  const;

    QString outputFile()  const;

    void run();
private:
    QString input;
    QString output;
    float dz; float xy; double angle;
};

#endif // DESKEW_H
