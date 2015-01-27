#include "andoruser.h"
#include <QDebug>
#include <QThread>
#include <QFileInfo>
#include <QDir>

AndorUser::AndorUser(QObject *parent) : QObject(parent)
{
    m_andorCcdParams = new AndorCcDParams();
    //initial ccd params
    m_andorCcdParams->bin[0] = 1;
    m_andorCcdParams->bin[1] = 1;
    m_andorCcdParams->binSwitch = false;
    m_andorCcdParams->conSnap = false;
    m_andorCcdParams->coolerSwitch = false;
    m_andorCcdParams->expTime = 0.1;
    m_andorCcdParams->fileAmount = 1;
    m_andorCcdParams->fileName = "andor_image";
    m_andorCcdParams->gain = 1;
    m_andorCcdParams->gainSwitch = false;
    m_andorCcdParams->imgSavPath = "./";
    m_andorCcdParams->isAcquiring = false;
    m_andorCcdParams->shutter = false;
    m_andorCcdParams->temp = -80;
    m_andorCcdParams->width = 1024;
    m_andorCcdParams->height = 1024;
    m_andorCcdParams->readMode = 4;
    m_andorCcdParams->connected = false;

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(SelfUpdateStat()));
    m_timer.start(100);

}

AndorUser::~AndorUser()
{
    m_timer.stop();
}

void AndorUser::InitCamera()
{
    //Initialize CCD
    error = Initialize("/usr/local/etc/andor");
    if(error!=DRV_SUCCESS){
        qDebug() << "Initialisation error...exiting" ;
        m_andorCcdParams->connected = false;
        return;
    }
    else
        m_andorCcdParams->connected = true;

    QThread::sleep(2);//sleep to allow initialization to complete

    //Set Read Mode to --Image--
    SetReadMode(4);

    //Set Acquisition mode to --Single scan--
    SetAcquisitionMode(1);

    //Set initial exposure time
    SetExposureTime(0.1);

    //Get Detector dimensions
    GetDetector(&width, &height);

    //Initialize Shutter
    SetShutter(1,0,50,50);

        //Setup Image dimensions
    SetImage(1,1,1,width,1,height);

}

void AndorUser::UserSetImageSavPath(QString path)
{
    //check if path is valid
    if(QFileInfo::exists(path))
    {
        m_andorCcdParams->imgSavPath = path;
        qDebug()<<"Image path: "<<path<<" set successfully.";
    }
    else
    {
        // if path is not exist, create it
        qDebug()<<"Image path: "<<path<<" is not exist";

    }
}

void AndorUser::UserGetImage()
{

    if(m_andorCcdParams->connected == true)
    {
        StartAcquisition();

        int status;

        //Loop until acquisition finished
        m_andorCcdParams->isAcquiring = true;
        GetStatus(&status);
        while(status==DRV_ACQUIRING) GetStatus(&status);
        m_andorCcdParams->isAcquiring = false;

        SaveAsFITS("./image.fits", 0);

    }
}

void AndorUser::UserGetAmountImage()
{

}

void AndorUser::UserGetImage(QString fileName, bool shutterOpen, float expTime, qint32 amount)
{
    //if camera is not connected, return
    if(m_andorCcdParams->connected==false)
        return;

    //set shutter
    switch (shutterOpen) {
    case true:
        SetShutter(1, 0, 50, 50);
        break;
    case false:
        SetShutter(1, 2, 50, 50);
    default:
        break;
    }

    //set exposure time
    SetExposureTime(expTime);

    //set path and file name
    QString baseFileName = m_andorCcdParams->imgSavPath+"/"+fileName;

    //acq image
//    if(amount == 1)
//    {
//        StartAcquisition();

//        m_andorCcdParams->isAcquiring=true;
//        WaitForAcquisition();
//        m_andorCcdParams->isAcquiring=false;

//        SaveAsFITS(baseFileName.toLatin1().data(), 0);
//    }
//    else
    {
        for(int i=0; i<amount; i++)
        {
            StartAcquisition();

            m_andorCcdParams->isAcquiring=true;
            WaitForAcquisition();
            m_andorCcdParams->isAcquiring=false;

            QString surfix;
            surfix.sprintf("%06d", i+1);
            SaveAsFITS((baseFileName
                       +"_"
                       +surfix).toLatin1().data(), 0);
        }
    }


}


void AndorUser::SelfUpdateStat()
{
    if(m_andorCcdParams->connected == true)
    {
        unsigned int state;
        //cooler status
        state = GetTemperature(&m_andorCcdParams->temp);
        switch (state) {
        case DRV_TEMPERATURE_NOT_REACHED:
            m_andorCcdParams->coolerSwitch=true;
            break;
        case DRV_TEMPERATURE_OFF:
            m_andorCcdParams->coolerSwitch=false;
            break;
        default:
            break;
        }


    }
}

void AndorUser::UserSetExpTime(float time)
{
    SetExposureTime(time);
    m_andorCcdParams->expTime = time;
}

void AndorUser::UserSetShutter(qint32 type, qint32 mode, qint32 closingTime, qint32 openingTime)
{
//    SetShutter(1, 0, 50, 50);
    SetShutter(type, mode, closingTime, openingTime);
}

void AndorUser::UserSetFileName(QString fileName)
{
    m_andorCcdParams->fileName = fileName;
}

void AndorUser::UserSetFileAmount(qint32 fileAmount)
{
    m_andorCcdParams->fileAmount =fileAmount;
}

void AndorUser::UserSetGain(qint32 gain)
{
    SetGain(gain);
    m_andorCcdParams->gain = gain;
}

void AndorUser::UserSetGainSwitch(bool gainSwitch)
{
    m_andorCcdParams->gainSwitch = false;
}

void AndorUser::UserSetBinning(qint32 bin[])
{
    m_andorCcdParams->bin[0] = bin[0];
    m_andorCcdParams->bin[1] = bin[1];
}

void AndorUser::UserSetBinningSwitch(bool binSwitch)
{

}

void AndorUser::UserSetReadMode(qint32 readMode)
{
    SetReadMode(readMode);
    m_andorCcdParams->readMode = readMode;
}

void AndorUser::UserSetTemp(qint32 temp)
{
    SetTemperature(temp);
    m_andorCcdParams->temp = temp;
}

void AndorUser::UserSetCoolerSwitch(bool coolerSwitch)
{
    switch(coolerSwitch)
    {
    case true:
        CoolerON();
        break;
    case false:
        CoolerOFF();
        break;
    default:
        break;
    }
}

void AndorUser::UserGetTemp(qint32 *temp)
{

    //for getting the current state of the cooler
    *temp = m_andorCcdParams->temp;

}

void AndorUser::UserGetCoolerSwitch(bool *coolerSwitch)
{
    //for getting the current state of the cooler
    *coolerSwitch = m_andorCcdParams->coolerSwitch;
}

void AndorUser::UserGetStat(bool *isAcquiring)
{
    *isAcquiring=m_andorCcdParams->isAcquiring;
}

void AndorUser::UserGetGain(qint32 *gain)
{
    *gain = m_andorCcdParams->gain;
}

void AndorUser::UserGetBinning(quint32 bin[])
{
    bin[0]=m_andorCcdParams->bin[0];
    bin[1]=m_andorCcdParams->bin[1];
}

void AndorUser::UserGetImageSavPath(QString *path)
{
    *path = m_andorCcdParams->imgSavPath;
}

void AndorUser::UserAbortAcq()
{
    if(m_andorCcdParams->connected == true)
        AbortAcquisition();
}

void AndorUser::UserCreateDir(QString path)
{
    //check if path is valid
    if(QFileInfo::exists(path))
    {
        qDebug()<<path<<" already exists";
        return;
    }
    else
    {
        // if path is not exist, create it
        qDebug()<<"Image path: "<<path<<" is not exist";
        QDir dir;
        if(dir.mkpath(path))
            qDebug()<<"mkdir "<<path<<" successfully!";
        else
            qDebug()<<path<<" create failed";

    }
}

int CameraSelect (int iNumArgs, char* szArgList[])
{
  if (iNumArgs == 2) {

    at_32 lNumCameras;
    GetAvailableCameras(&lNumCameras);
    int iSelectedCamera = atoi(szArgList[1]);

    if (iSelectedCamera < lNumCameras && iSelectedCamera >= 0) {
      at_32 lCameraHandle;
      GetCameraHandle(iSelectedCamera, &lCameraHandle);
      SetCurrentCamera(lCameraHandle);
      return iSelectedCamera;
    }
    else
      return -1;
  }
  return 0;
}
