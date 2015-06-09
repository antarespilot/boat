#ifndef IMUDATA_H
#define IMUDATA_H

#include <QObject>
#include <QTimer>
#include <QVector3D>
#include <QNmeaPositionInfoSource>
#include <QSerialPort>

#include "RTIMULib.h"

class ImuData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVector3D pose READ pose WRITE setPose NOTIFY poseChanged)
    Q_PROPERTY(QGeoCoordinate geoCoordinate READ geoCoordinate WRITE setGeoCoordinate NOTIFY geoCoordinateChanged)
    Q_PROPERTY(int servoPosition READ servoPosition WRITE setServoPosition NOTIFY servoPositionChanged)

public:

    explicit ImuData(QObject *parent = 0);
    bool initialize();

    QVector3D pose() const  {   return m_pose;  }

    QGeoCoordinate geoCoordinate() const
    {
        return m_geoCoordinate;
    }

    int servoPosition() const
    {
        return m_servoPosition;
    }

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

signals:

    void poseChanged(QVector3D pose);

    void geoCoordinateChanged(QGeoCoordinate geoCoordinate);

    void servoPositionChanged(int servoPosition);

private:

    void pollImu();
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
};

#endif // IMUDATA_H
