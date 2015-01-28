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

//    InitCamera();

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(SelfUpdateStat()));
    m_timer.start(100);

    m_ds9Id = "FUT";
    StartDs9();


}

AndorUser::~AndorUser()
{
    //stop timer
    m_timer.stop();

    //close ds9 process
    m_procDs9.close();
    m_procDs9.deleteLater();

    //shutdown ccd
    ShutDown();
}

void AndorUser::WriteFitsKeys(QString fileName)
{
    fitsfile* fptr;
    char card[500];
    int status = 0, nkeys;
    ffopen(&fptr, fileName.toLatin1().data(), READWRITE, &status);
//    fits_open_file(&fptr, fileName.toLatin1().data(), READONLY, &status);

    fits_delete_key(fptr, "HIERARCH", &status);
    fits_delete_key(fptr, "COMMENT", &status);
    fits_delete_key(fptr, "COMMENT", &status);

    fits_get_hdrspace(fptr, &nkeys, NULL, &status);
    for(int i=1; i<=nkeys; i++)
    {
        fits_read_record(fptr, i, card, &status);
        qDebug()<<i<<":"<<card;
    }
    qDebug()<<"END";

    //write keyword
//    char *value = "keyvalue";
//    fits_write_key(fptr, TSTRING, "keyname", value, "comm", &status);
    QString value = "bao";
    fits_write_key(fptr, TSTRING, "OBSERVAT", value.toLatin1().data(), "observatory", &status);
    value = "40:23:45.36";
    fits_write_key(fptr, TSTRING, "LATITUDE", value.toLatin1().data(), "latitude of the telescope", &status);
    value = "117:34:28.35";
    fits_write_key(fptr, TSTRING, "LONGITUD", value.toLatin1().data(), "longitude of the telescope", &status);
    value = "YYMMDDHHMMSSXS1ANO0";
    fits_write_key(fptr, TSTRING, "IMAGEID", value.toLatin1().data(), "", &status);
    fits_write_comment(fptr, "date(6)-time(6)-observatory(1)-futID(2)-ccdmodel(2)-ccdtype(1)_product level(1)", &status);
    value = "ANDOR iXon3 DU888";
    fits_write_key(fptr, TSTRING, "CAMTYPE", value.toLatin1().data(), "camera type", &status);
    value = "OBJECT";
    fits_write_key(fptr, TSTRING, "IMAGETYP", value.toLatin1().data(), "image type", &status);
    value = "YYYY-MM-DDThh:mm:ss.sss";
    fits_write_key(fptr, TSTRING, "DATE-OBS", value.toLatin1().data(), "universal date", &status);
    value = "YYYY-MM-DDThh:mm:ss.sss";
    fits_write_key(fptr, TSTRING, "D-OBS-LC", value.toLatin1().data(), "local date", &status);
    value = "222";float fvalue = 222;
    fits_write_key(fptr, TFLOAT, "EXPTIME", &fvalue, "exposure time(sec)", &status);
    value = "HH:MM:SS.SSS";
    fits_write_key(fptr, TSTRING, "LST", value.toLatin1().data(), "local sidereal time of starting exposure", &status);
    value = "HH:MM:SS.SSS";
    fits_write_key(fptr, TSTRING, "RA-TEL", value.toLatin1().data(), "RA of telescope", &status);
    value = "DD:MM:SS.SSS";
    fits_write_key(fptr, TSTRING, "DEC-TEL", value.toLatin1().data(), "DEC of telescope", &status);
    value = "2000";
    fits_write_key(fptr, TSTRING, "EQUINOX", value.toLatin1().data(), "", &status);
    value = "S1";
    fits_write_key(fptr, TSTRING, "FUTID", value.toLatin1().data(), "ID of follow up telescope, east(S1), west(S2)", &status);
    value = "filter ";
    fits_write_key(fptr, TSTRING, "FILTER", value.toLatin1().data(), "name of selected filter", &status);
    value = "to be calulated";
    fits_write_key(fptr, TSTRING, "PXLSCLE", value.toLatin1().data(), "pixel scale", &status);
    value = "gain..";
    fits_write_key(fptr, TSTRING, "GAIN", value.toLatin1().data(), "e-/ADU", &status);
    value = "1";
    fits_write_key(fptr, TSTRING, "RDNOISE", value.toLatin1().data(), "read out noise(e-) <1 with EM gain", &status);
    value = "30";
    fits_write_key(fptr, TSTRING, "READRATE", value.toLatin1().data(), "MHz", &status);
    value = "0";
    fits_write_key(fptr, TSTRING, "TEMPSET", value.toLatin1().data(), "set CCD temperature", &status);
    value = "0";
    fits_write_key(fptr, TSTRING, "TEMPACT", value.toLatin1().data(), "actual CCD temperature", &status);
    value = "who";
    fits_write_key(fptr, TSTRING, "OBSERVER", value.toLatin1().data(), "name of the observer", &status);


    fits_close_file(fptr, &status);

    if(status)
        qDebug()<<"stderr:"<<stderr<<" status:"<<status;

}

void AndorUser::StartDs9()
{
    //start ds9
    m_procDs9.setEnvironment(m_procDs9.environment());
    m_argsDs9 << "-title";
    m_argsDs9 << m_ds9Id;
    m_procDs9.start("./bin/ds9", m_argsDs9);
    m_procDs9.waitForStarted();
}

void AndorUser::DisplayImage(QString fileName, QString ds9Id)
{

    //start XPA
    m_argsXpa = "./bin/xpaset "+
            ds9Id+
            " fits "+
            fileName+
            " < "+
            fileName;
    system(m_argsXpa.toLatin1().data());
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
        SetShutter(1, 1, 50, 50); //need to check if dark is possible
    default:
        break;
    }

    //set exposure time
    SetExposureTime(expTime);

    //set path and file name
    QString baseFileName = m_andorCcdParams->imgSavPath+"/"+fileName;

    //acq image

    for(int i=0; i<amount; i++)
    {
        StartAcquisition();

        m_andorCcdParams->isAcquiring=true;
        WaitForAcquisition();
        m_andorCcdParams->isAcquiring=false;

        QString surfix;
        surfix.sprintf("%06d", i+1);
        QString fitsFileName=baseFileName
                             +"_"
                             +surfix;

        SaveAsFITS(fitsFileName.toLatin1().data(), 0);
        qDebug()<<"fits file name: "<<fitsFileName;

        WriteFitsKeys(fitsFileName);

        DisplayImage(fitsFileName, m_ds9Id);


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

void AndorUser::UserGetConnect(bool *connect)
{
    *connect=m_andorCcdParams->connected;
//    qDebug()<<"m_andorCcdParams->connected"<<m_andorCcdParams->connected;
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
