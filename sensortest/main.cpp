#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QtQml>

#include "imudata.h"
#include <pigpio.h>

#include "mydaemon.h"

static int setup_unix_signal_handlers()
{
    struct sigaction hup, term, sigint;

    hup.sa_handler = MyDaemon::hupSignalHandler;
    sigemptyset(&hup.sa_mask);
    hup.sa_flags = 0;
    hup.sa_flags |= SA_RESTART;

    if (sigaction(SIGHUP, &hup, 0) > 0)
       return 1;

    term.sa_handler = MyDaemon::termSignalHandler;
    sigemptyset(&term.sa_mask);
    term.sa_flags |= SA_RESTART;

    if (sigaction(SIGTERM, &term, 0) > 0)
       return 2;

    sigint.sa_handler = MyDaemon::intSignalHandler;
    sigemptyset(&sigint.sa_mask);
    sigint.sa_flags |= SA_RESTART;

    if (sigaction(SIGTERM, &sigint, 0) > 0)
       return 3;

    return 0;
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    setup_unix_signal_handlers();
    MyDaemon mine;

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
