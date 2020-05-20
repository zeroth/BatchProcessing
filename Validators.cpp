#include "Validators.h"


IStringValidator::IStringValidator(QObject *parent) :
    QValidator(parent) {

}

IStringValidator::~IStringValidator() {}

QValidator::State IStringValidator::validate(QString &input, int &) const {
    if (input.isEmpty())
        return Invalid;
    else
        return Acceptable;
}

void IStringValidator::fixup(QString &) const {
    return;
}

/*IIntValidator::IIntValidator(int minimum, int maximum, QObject *parent) :
    QIntValidator(minimum, maximum, parent) {

}

IIntValidator::~IIntValidator() {}

void IIntValidator::fixup(QString &in) const {
    if (in.isEmpty())
        in = "0";
    return;
}*/

IDoubleValidator::IDoubleValidator(double bottom, double top, int decimals, QObject *parent)
    :QDoubleValidator(bottom, top, decimals, parent)
{

}

IDoubleValidator::~IDoubleValidator()
{

}

void IDoubleValidator::fixup(QString &in) const
{
    if (in.isEmpty())
        in = "0.0";
    return;
}
