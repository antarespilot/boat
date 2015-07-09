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
    , m_ad0(0.0)
    , m_ad1(0.0)
    , m_ad2(0.0)
    , m_ad3(0.0)
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

    // SPI AD converter setup
    connect(&ad_timer_, &QTimer::timeout, this, &ImuData::pollAd);
    ad_timer_.start(15);

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

void ImuData::pollAd()
{
    setAd0((qreal)readAdChannel(0)/1024.0);
    setAd1((qreal)readAdChannel(1)/1024.0);

    int val = readAdChannel(2);
    setAd2((qreal)val/1024.0);

    val = readAdChannel(3);
    setAd3((qreal)val/1024.0);


    /**  Brilliant stuff here
     * https://tkkrlab.nl/wiki/Arduino_37_sensors
     * */
}

int ImuData::readAdChannel(int channel)
{
    int a2dval = 0;
    unsigned char data[3];

    data[0] = 1;  //  first byte transmitted -> start bit
    data[1] = 0b10000000 |( ((channel & 7) << 4)); // second byte transmitted -> (SGL/DIF = 1, D2=D1=D0=0)
    data[2] = 0; // third byte transmitted....don't care

    ad_.spiWriteRead(data, sizeof(data) );

    a2dval = 0;
    a2dval = (data[1]<< 8) & 0b1100000000; //merge data[1] & data[2] to get result
    a2dval |=  (data[2] & 0xff);

    return a2dval;
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





#if 0

// Use this stuff

/*
This code is provided under the BSD license.
Copyright (c) 2014, Emlid Limited. All rights reserved.
Written by Mikhail Avkhimenia (mikhail.avkhimenia@emlid.com)
twitter.com/emlidtech || www.emlid.com || info@emlid.com
Application: PPM to PWM decoder for Raspberry Pi with Navio.
Requres pigpio library, please install it first - http://abyz.co.uk/rpi/pigpio/
To run this app navigate to the directory containing it and run following commands:
make
sudo ./PPM
*/

#include <pigpio.h>
#include <stdio.h>
#include <unistd.h>

#include <Navio/gpio.h>
#include "Navio/PCA9685.h"

//================================ Options =====================================

unsigned int samplingRate      = 1;      // 1 microsecond (can be 1,2,4,5,10)
unsigned int ppmInputGpio      = 4;      // PPM input on Navio's 2.54 header
unsigned int ppmSyncLength     = 4000;   // Length of PPM sync pause
unsigned int ppmChannelsNumber = 8;      // Number of channels packed in PPM
unsigned int servoFrequency    = 50;     // Servo control frequency
bool verboseOutputEnabled      = true;   // Output channels values to console

//============================ Objects & data ==================================

PCA9685 *pwm;
float channels[8];

//============================== PPM decoder ===================================

unsigned int currentChannel = 0;
unsigned int previousTick;
unsigned int deltaTime;

void ppmOnEdge(int gpio, int level, uint32_t tick)
{
    if (level == 0) {
        deltaTime = tick - previousTick;
        previousTick = tick;

        if (deltaTime >= ppmSyncLength) { // Sync
            currentChannel = 0;

            // RC output
            for (int i = 0; i < ppmChannelsNumber; i++)
                pwm->setPWMuS(i + 3, channels[i]); // 1st Navio RC output is 3

            // Console output
            if (verboseOutputEnabled) {
                printf("\n");
                for (int i = 0; i < ppmChannelsNumber; i++)
                    printf("%4.f ", channels[i]);
            }
        }
        else
            if (currentChannel < ppmChannelsNumber)
                channels[currentChannel++] = deltaTime;
    }
}

//================================== Main ======================================

using namespace Navio;

int main(int argc, char *argv[])
{
    static const uint8_t outputEnablePin = RPI_GPIO_27;

    Pin pin(outputEnablePin);

    if (pin.init()) {
        pin.setMode(Pin::GpioModeOutput);
        pin.write(0); /* drive Output Enable low */
    } else {
        fprintf(stderr, "Output Enable not set. Are you root?");
    }

    // Servo controller setup

    pwm = new PCA9685();
    pwm->initialize();
    pwm->setFrequency(servoFrequency);

    // GPIO setup

    gpioCfgClock(samplingRate, PI_DEFAULT_CLK_PERIPHERAL, 0); /* last parameter is deprecated now */
    gpioInitialise();
    previousTick = gpioTick();
    gpioSetAlertFunc(ppmInputGpio, ppmOnEdge);

    // Infinite sleep - all action is now happening in ppmOnEdge() function

    while(1)
        sleep(10);
    return 0;
}

#endif
