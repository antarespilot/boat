#ifndef MYDAEMON_H
#define MYDAEMON_H

#include <QObject>
#include <QSocketNotifier>

#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>

class MyDaemon : public QObject
{
    Q_OBJECT

public:

    MyDaemon(QObject *parent = 0);
    virtual ~MyDaemon();

    // Unix signal handlers.
    static void hupSignalHandler(int unused);
    static void termSignalHandler(int unused);
    static void intSignalHandler(int unused);

public slots:

    // Qt signal handlers.
    void handleSigHup();
    void handleSigTerm();
    void handleSigInt();

private:

    static int sighupFd[2];
    static int sigtermFd[2];
    static int sigintFd[2];

    QSocketNotifier *snHup;
    QSocketNotifier *snTerm;
    QSocketNotifier *snInt;
};

#endif // MYDAEMON_H
