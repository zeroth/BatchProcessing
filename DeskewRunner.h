#ifndef DESKEW_H
#define DESKEW_H

#include <QString>
#include <QList>

class DeskewRunner
{
public:
    DeskewRunner(const QString& inputList, const QString& outputList, double dz,
                 double xy, double angle, bool save8Bit=false);
    QString inputFile()  const;

    QString outputFile()  const;

    void run();
private:
    QString input;
    QString output;
    double dz; double xy; double angle;
    bool save8Bit;
};

#endif // DESKEW_H
