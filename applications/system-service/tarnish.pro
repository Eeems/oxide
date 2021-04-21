QT += dbus

CONFIG += c++17
CONFIG += console
CONFIG -= app_bundle
CONFIG += precompile_header_c

QMAKE_CFLAGS += -std=c99

SOURCES += \
    apibase.cpp \
    application.cpp \
    appsapi.cpp \
    bss.cpp \
    buttonhandler.cpp \
    event_device.cpp \
    network.cpp \
    notification.cpp \
    screenshot.cpp \
    sysobject.cpp \
    systemapi.cpp \
    wlan.cpp \
    wpa_supplicant.cpp \
    main.cpp \
    ../../shared/devicesettings.cpp

TARGET=tarnish

target.path = /opt/bin
INSTALLS += target

configFile.files = ../../assets/etc/dbus-1/system.d/*
configFile.path =  /etc/dbus-1/system.d/
INSTALLS += configFile

service.files = ../../assets/etc/systemd/system/tarnish.service
service.path = /etc/systemd/system/
INSTALLS += service

applications.files = ../../assets/opt/usr/share/applications/*
applications.path = /opt/usr/share/applications/
INSTALLS += applications

system(qdbusxml2cpp -N -p wpa_supplicant.h:wpa_supplicant.cpp fi.w1.wpa_supplicant1.xml)

DBUS_INTERFACES += org.freedesktop.login1.xml

INCLUDEPATH += $$PWD/../../docker-toolchain/qtcreator/files/libraries
INCLUDEPATH += ../../shared
DEPENDPATH += $$PWD/../../docker-toolchain/qtcreator/files/libraries

HEADERS += \
    apibase.h \
    application.h \
    appsapi.h \
    bss.h \
    buttonhandler.h \
    dbusservice.h \
    dbussettings.h \
    digitizerhandler.h \
    event_device.h \
    fifohandler.h \
    mxcfb.h \
    network.h \
    notification.h \
    notificationapi.h \
    powerapi.h \
    screenapi.h \
    screenshot.h \
    supplicant.h \
    sysobject.h \
    systemapi.h \
    wifiapi.h \
    wlan.h \
    wpa_supplicant.h \
    ../../shared/devicesettings.h \
    ../../shared/signalhandler.h

linux-oe-g++ {
    LIBS += -lqsgepaper
    LIBS += -lpng16
    LIBS += -lsystemd
    LIBS += -lz
}

QMAKE_POST_LINK += sh $$_PRO_FILE_PWD_/generate_xml.sh

DISTFILES += \
    ../../assets/opt/usr/share/applications/codes.eeems.anxiety.oxide \
    ../../assets/opt/usr/share/applications/codes.eeems.corrupt.oxide \
    ../../assets/opt/usr/share/applications/codes.eeems.mold.oxide \
    fi.w1.wpa_supplicant1.xml \
    generate_xml.sh \
    org.freedesktop.login1.xml

RESOURCES +=
