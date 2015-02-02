#ifndef CCDPARAMS_H
#define CCDPARAMS_H

#endif // CCDPARAMS_H

#include <QString>

class AndorCcDParams
{
public:
    AndorCcDParams()
    {
        ;
    }

public:
    QString imgSavPath;
    float expTime;
    bool shutter;
    QString fileName;
    qint32 fileAmount;
    bool conSnap;
    qint32 gain;
    bool gainSwitch;
    quint32 bin[2];
    bool binSwitch;
    qint32 temp;
    bool coolerSwitch;
    bool isAcquiring;
    qint32 width;
    qint32 height;
    qint32 readMode;
    bool connected;
    qint32 curNumb;
    float acqProc;
};
