#include "mydaemon.h"
#include <QCoreApplication>
#include <QDebug>

#include <unistd.h>

int MyDaemon::sighupFd[2];
int MyDaemon::sigtermFd[2];
int MyDaemon::sigintFd[2];

MyDaemon::MyDaemon(QObject *parent)
        : QObject(parent)
{
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sighupFd))
       qFatal("Couldn't create HUP socketpair");

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigtermFd))
       qFatal("Couldn't create TERM socketpair");

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigintFd))
       qFatal("Couldn't create INT socketpair");

    snHup = new QSocketNotifier(sighupFd[1], QSocketNotifier::Read, this);
    connect(snHup, SIGNAL(activated(int)), this, SLOT(handleSigHup()));

    snTerm = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read, this);
    connect(snTerm, SIGNAL(activated(int)), this, SLOT(handleSigTerm()));

    snInt = new QSocketNotifier(sigintFd[1], QSocketNotifier::Read, this);
    connect(snInt, SIGNAL(activated(int)), this, SLOT(handleSigInt()));

}

MyDaemon::~MyDaemon()
{
    // ...
}

void MyDaemon::hupSignalHandler(int)
{
    char a = 1;
    ::write(sighupFd[0], &a, sizeof(a));
}

void MyDaemon::termSignalHandler(int)
{
    char a = 1;
    ::write(sigtermFd[0], &a, sizeof(a));
}

void MyDaemon::intSignalHandler(int)
{
    char a = 1;
    ::write(sigintFd[0], &a, sizeof(a));
}

void MyDaemon::handleSigTerm()
{
    snTerm->setEnabled(false);
    char tmp;
    ::read(sigtermFd[1], &tmp, sizeof(tmp));

    qWarning() << "Sig term here";
    QCoreApplication::instance()->quit();

    snTerm->setEnabled(true);
}

void MyDaemon::handleSigHup()
{
    snHup->setEnabled(false);
    char tmp;
    ::read(sighupFd[1], &tmp, sizeof(tmp));

    qWarning() << "Sig hup here";
    QCoreApplication::instance()->quit();

    snHup->setEnabled(true);
}

void MyDaemon::handleSigInt()
{
    snInt->setEnabled(false);
    char tmp;
    ::read(sigintFd[1], &tmp, sizeof(tmp));

    qWarning() << "Sig int here";
    QCoreApplication::instance()->quit();

    snInt->setEnabled(true);
}
