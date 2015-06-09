#include "imudata.h"
#include <QDebug>
#include <QtMath>

#include <pigpio.h>

ImuData::ImuData(QObject *parent)
    : QObject(parent)
    , imu_(nullptr)
    , pressure_(nullptr)
    , serialport_(nullptr)
    , positionsource_(nullptr)
    , m_servoPosition(0)
{
}

bool ImuData::initialize()
{
    RTIMUSettings *settings = new RTIMUSettings("RTIMULib");
    imu_ = RTIMU::createIMU(settings);
//    pressure_ = RTPressure::createPressure(settings);

    if(imu_==nullptr || imu_->IMUType() == RTIMU_TYPE_NULL) {
        return false;
    }

    imu_->IMUInit();

    imu_->setSlerpPower(0.02);
    imu_->setGyroEnable(true);
    imu_->setAccelEnable(true);
    imu_->setCompassEnable(true);

    if(pressure_)
        pressure_->pressureInit();

    connect(&poll_timer_, &QTimer::timeout, this, &ImuData::pollImu);
    poll_timer_.start(imu_->IMUGetPollInterval());


    // GPS setup
    serialport_ = new QSerialPort(this);
    serialport_->setBaudRate(QSerialPort::Baud9600);
    serialport_->setPortName("/dev/ttyAMA0");
    if(!serialport_->open(QIODevice::ReadOnly)) {
        return false;
    }

    positionsource_ = new QNmeaPositionInfoSource(QNmeaPositionInfoSource::RealTimeMode, this);
    positionsource_->setDevice(serialport_);
    connect(positionsource_, &QNmeaPositionInfoSource::positionUpdated, this, &ImuData::handleGpsPositionUpdated);
    positionsource_->startUpdates();

    connect(this, &ImuData::servoPositionChanged, this, &ImuData::handleServoPositionChanged);

    return true;
}

void ImuData::pollImu()
{
    while(imu_->IMURead()) {
        RTIMU_DATA imuData = imu_->getIMUData();
        if(pressure_)
            pressure_->pressureRead(imuData);

        QVector3D p;
        p.setX(imuData.fusionPose.x()*180.0/M_PI);
        p.setY(imuData.fusionPose.y()*180.0/M_PI);
        p.setZ(imuData.fusionPose.z()*180.0/M_PI);
        setPose(p);
    }
}

void ImuData::handleGpsPositionUpdated(const QGeoPositionInfo &update)
{
//    qWarning() << "Got new position here " << update;
    setGeoCoordinate(update.coordinate());
}

void ImuData::handleServoPositionChanged(int servoPosition)
{
    gpioServo(25, servoPosition);
}

