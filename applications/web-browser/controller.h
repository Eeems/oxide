#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QImage>
#include <QQuickItem>
#include <QSettings>
#include <QTimer>

#include <epframebuffer.h>

#include "dbussettings.h"
#include <QKeyEvent>
#include <QMouseEvent>

#define MOLD_SETTINGS_VERSION 1

enum State { Normal, PowerSaving };
enum BatteryState { BatteryUnknown, BatteryCharging, BatteryDischarging, BatteryNotPresent };
enum ChargerState { ChargerUnknown, ChargerConnected, ChargerNotConnected, ChargerNotPresent };
enum WifiState { WifiUnknown, WifiOff, WifiDisconnected, WifiOffline, WifiOnline};

class Controller : public QObject {
    Q_OBJECT
public:
    Controller(QObject* parent)
    : QObject(parent), settings(this), applications() {
        settings.sync();
        auto version = settings.value("version", 0).toInt();
        if(version < MOLD_SETTINGS_VERSION){
            migrate(&settings, version);
        }
    }
    ~Controller(){}

    Q_INVOKABLE void startup(){
        qDebug() << "Running controller startup";
        QTimer::singleShot(10, [this]{
            setState("loaded");
        });
    }
    QString state() {
        if(!getStateControllerUI()){
            return "loading";
        }
        return stateControllerUI->property("state").toString();
    }
    void setState(QString state){
        if(!getStateControllerUI()){
            throw "Unable to find state controller";
        }
        stateControllerUI->setProperty("state", state);
    }

    void setRoot(QObject* root){ this->root = root; }
signals:
    void aboutToQuit();
    void keyEvent(QKeyEvent* ke);
    void mouseEvent(QMouseEvent* me);
    void touchEvent(QTouchEvent* te);
    void tabletEvent(QTabletEvent* te);

private:
    QSettings settings;
    QObject* root = nullptr;
    QObject* stateControllerUI = nullptr;
    QList<QObject*> applications;

    QObject* getStateControllerUI(){
        stateControllerUI = root->findChild<QObject*>("stateController");
        return stateControllerUI;
    }
    static void migrate(QSettings* settings, int fromVersion){
        if(fromVersion != 0){
            throw "Unknown settings version";
        }
        // In the future migrate changes to settings between versions
        settings->setValue("version", MOLD_SETTINGS_VERSION);
        settings->sync();
    }
};

#endif // CONTROLLER_H
