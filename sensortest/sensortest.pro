TEMPLATE = app

QT += qml quick positioning serialport
CONFIG += c++11

SOURCES += main.cpp \
    imudata.cpp \
    mydaemon.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

include(../ext/RTIMULib/RTIMULib/RTIMULib.pri)
include(../ext/pigpio.pri)
include(../ext/gnublin-api.pri)
include(../ext/mcp3008spi.pri)

target.path = /home/pi

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    imudata.h \
    mydaemon.h

