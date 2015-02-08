#ifndef CCDPARAMS_H
#define CCDPARAMS_H

#endif // CCDPARAMS_H

#include <QString>

class AndorCcDParams
{
public:
    AndorCcDParams()
    {
        //initial ccd params
        bin[0] = 1;
        bin[1] = 1;
        binSwitch = false;
        conSnap = false;
        coolerSwitch = false;
        expTime = 0.1;
        fileAmount = 1;
        fileName = "andor_image";
        gain = 1;
        gainSwitch = false;
        imgSavPath = "./";
        isAcquiring = false;
        shutter = false;
        tempSet = -80;
        width = 1024;
        height = 1024;
        readMode = 4;
        connected = false;
        curNumb=0;
        imgType = "OBJECT";
        dateObs="";
        st="";
        ut="";
        ra="";
        dec="";
        futID="";
        filColor="";
        tempAct=0;
        observer="";
        lt = "";
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
    qint32 tempSet;
    bool coolerSwitch;
    bool isAcquiring;
    qint32 width;
    qint32 height;
    qint32 readMode;
    bool connected;
    qint32 curNumb;
    float acqProc;
    QString imgType;
    QString dateObs;
    QString ut;
    QString st;
    QString ra;
    QString dec;
    QString futID;
    QString filColor;
    qint32 tempAct;
    QString observer;
    QString lt;
};
