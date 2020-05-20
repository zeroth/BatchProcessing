#ifndef VALIDATORS_H
#define VALIDATORS_H

#include <QIntValidator>
#include <QDoubleValidator>
#include <QValidator>

class IStringValidator : public QValidator {
Q_OBJECT
public:
    IStringValidator(QObject* parent = nullptr);
    ~IStringValidator();

    QValidator::State validate(QString& input, int&) const;

    void fixup(QString&) const;
};

/*class IIntValidator : public QIntValidator {

public:
    IIntValidator(int bottom, int top, QObject* parent = nullptr);
    ~IIntValidator();


    void fixup(QString& in) const;
};*/

class IDoubleValidator : public QDoubleValidator {
Q_OBJECT
public:
    IDoubleValidator(double bottom, double top, int decimals, QObject* parent = nullptr);
    ~IDoubleValidator();


    void fixup(QString& in) const;
};

#endif // VALIDATORS_H
