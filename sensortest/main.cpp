#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QtQml>

#include "imudata.h"

#include <pigpio.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    ImuData imudata;
    if(!imudata.initialize()) {
        qCritical() << "Failed to initialize IMUs";
        exit(1);
    }

    // Try out the pigpio stuff..
    if(gpioInitialise() < 0) {
        qWarning() << "pig init failed!?!?!?";
        exit(1);
    } else {
        qDebug() << "pigpio initialized!";

//        gpioSetPWMfrequency(25, 400);
//        gpioSetPWMrange(25, 2500);
        gpioServo(25, 1500);
    }

    // Set up Qt Quick
    qmlRegisterType<ImuData>("Boat",1,0,"ImuData");
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("imudata", &imudata);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();

    gpioServo(25, 0);
}
