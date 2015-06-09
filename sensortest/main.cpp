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
    } else {
        qDebug() << "pigpio initialized!";
    }


    // Set up Qt Quick

    qmlRegisterType<ImuData>("Boat",1,0,"ImuData");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("imudata", &imudata);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));




    return app.exec();
}
