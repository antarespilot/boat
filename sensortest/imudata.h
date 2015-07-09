#ifndef IMUDATA_H
#define IMUDATA_H

#include <QObject>
#include <QTimer>
#include <QVector3D>
#include <QNmeaPositionInfoSource>
#include <QSerialPort>

#include "RTIMULib.h"
#include <mcp3008Spi.h>

class ImuData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVector3D pose READ pose WRITE setPose NOTIFY poseChanged)
    Q_PROPERTY(QGeoCoordinate geoCoordinate READ geoCoordinate WRITE setGeoCoordinate NOTIFY geoCoordinateChanged)
    Q_PROPERTY(int servoPosition READ servoPosition WRITE setServoPosition NOTIFY servoPositionChanged)

    Q_PROPERTY(qreal ad0 READ ad0 WRITE setAd0 NOTIFY ad0Changed)
    Q_PROPERTY(qreal ad1 READ ad1 WRITE setAd1 NOTIFY ad1Changed)
    Q_PROPERTY(qreal ad2 READ ad2 WRITE setAd2 NOTIFY ad2Changed)
    Q_PROPERTY(qreal ad3 READ ad3 WRITE setAd3 NOTIFY ad3Changed)

public:

    explicit ImuData(QObject *parent = 0);
    bool initialize();

    QVector3D pose() const  {   return m_pose;  }

    QGeoCoordinate geoCoordinate() const
    {
        return m_geoCoordinate;
    }

    int servoPosition() const   {   return m_servoPosition; }
    qreal ad0() const   {   return m_ad0;   }
    qreal ad1() const   {   return m_ad1;   }
    qreal ad2() const   {   return m_ad2;   }
    qreal ad3() const   {   return m_ad3;   }

public slots:

    void setPose(QVector3D pose)
    {
        if (m_pose == pose)
            return;
        m_pose = pose;
        emit poseChanged(pose);
    }

    void setGeoCoordinate(QGeoCoordinate geoCoordinate)
    {
        if (m_geoCoordinate == geoCoordinate)
            return;

        m_geoCoordinate = geoCoordinate;
        emit geoCoordinateChanged(geoCoordinate);
    }

    void setServoPosition(int servoPosition)
    {
        if (m_servoPosition == servoPosition)
            return;

        m_servoPosition = servoPosition;
        emit servoPositionChanged(servoPosition);
    }

    void setAd0(qreal ad0)
    {
        if (m_ad0 == ad0)
            return;

        m_ad0 = ad0;
        emit ad0Changed(ad0);
    }

    void setAd1(qreal ad1)
    {
        if (m_ad1 == ad1)
            return;

        m_ad1 = ad1;
        emit ad1Changed(ad1);
    }

    void setAd2(qreal ad2)
    {
        if (m_ad2 == ad2)
            return;

        m_ad2 = ad2;
        emit ad2Changed(ad2);
    }

    void setAd3(qreal ad3)
    {
        if (m_ad3 == ad3)
            return;

        m_ad3 = ad3;
        emit ad3Changed(ad3);
    }

signals:

    void poseChanged(QVector3D pose);
    void geoCoordinateChanged(QGeoCoordinate geoCoordinate);
    void servoPositionChanged(int servoPosition);

    void ad0Changed(qreal ad0);

    void ad1Changed(qreal ad1);

    void ad2Changed(qreal ad2);

    void ad3Changed(qreal ad3);

private:

    void pollImu();

    void pollAd();
    int readAdChannel(int channel);

    void handleGpsPositionUpdated(const QGeoPositionInfo & update);
    void handleServoPositionChanged(int servoPosition);

    QTimer poll_timer_;

    RTIMU* imu_;
    RTPressure* pressure_;

    QVector3D m_pose;

    QSerialPort* serialport_;
    QNmeaPositionInfoSource* positionsource_;

    QGeoCoordinate m_geoCoordinate;
    int m_servoPosition;

    mcp3008Spi ad_;
    QTimer ad_timer_;
    qreal m_ad0;
    qreal m_ad1;
    qreal m_ad2;
    qreal m_ad3;
};

#endif // IMUDATA_H
