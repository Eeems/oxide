#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QImage>
#include <QQuickItem>

#include <epframebuffer.h>

#include "dbussettings.h"

#include "dbusservice_interface.h"
#include "screenapi_interface.h"
#include "screenshot_interface.h"

#include "screenshotlist.h"

using namespace codes::eeems::oxide1;

#define ANXIETY_SETTINGS_VERSION 1

enum State { Normal, PowerSaving };
enum BatteryState { BatteryUnknown, BatteryCharging, BatteryDischarging, BatteryNotPresent };
enum ChargerState { ChargerUnknown, ChargerConnected, ChargerNotConnected, ChargerNotPresent };
enum WifiState { WifiUnknown, WifiOff, WifiDisconnected, WifiOffline, WifiOnline};

class Controller : public QObject {
    Q_OBJECT
    Q_PROPERTY(ScreenshotList* screenshots MEMBER screenshots READ getScreenshots NOTIFY screenshotsChanged)
    Q_PROPERTY(int columns READ columns WRITE setColumns NOTIFY columnsChanged)
public:
    Controller(QObject* parent)
    : QObject(parent), settings(this), applications() {
        screenshots = new ScreenshotList();
        auto bus = QDBusConnection::systemBus();
        qDebug() << "Waiting for tarnish to start up...";
        while(!bus.interface()->registeredServiceNames().value().contains(OXIDE_SERVICE)){
            struct timespec args{
                .tv_sec = 1,
                .tv_nsec = 0,
            }, res;
            nanosleep(&args, &res);
        }
        api = new General(OXIDE_SERVICE, OXIDE_SERVICE_PATH, bus, this);

        qDebug() << "Requesting screen API...";
        QDBusObjectPath path = api->requestAPI("screen");
        if(path.path() == "/"){
            qDebug() << "Unable to get screen API";
            throw "";
        }
        screenApi = new Screen(OXIDE_SERVICE, path.path(), bus, this);
        connect(screenApi, &Screen::screenshotAdded, this, &Controller::screenshotAdded);
        connect(screenApi, &Screen::screenshotModified, this, &Controller::screenshotModified);
        connect(screenApi, &Screen::screenshotRemoved, this, &Controller::screenshotRemoved);

        for(auto path : screenApi->screenshots()){
            screenshots->append(new Screenshot(OXIDE_SERVICE, path.path(), bus, this));
        }

        settings.sync();
        auto version = settings.value("version", 0).toInt();
        if(version < ANXIETY_SETTINGS_VERSION){
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
    int columns() { return settings.value("columns", 3).toInt(); }
    void setColumns(int columns){
        settings.setValue("columns", columns);
        emit columnsChanged(columns);
    }
    ScreenshotList* getScreenshots(){ return screenshots; }

    void setRoot(QObject* root){ this->root = root; }

signals:
    void screenshotsChanged(ScreenshotList*);
    void columnsChanged(int);

private slots:
    void screenshotAdded(const QDBusObjectPath& path){
        screenshots->append(new Screenshot(OXIDE_SERVICE, path.path(), QDBusConnection::systemBus(), this));
        emit screenshotsChanged(screenshots);
    }
    void screenshotModified(const QDBusObjectPath& path){
        Q_UNUSED(path);
        emit screenshotsChanged(screenshots);
    }
    void screenshotRemoved(const QDBusObjectPath& path){
        screenshots->remove(path);
        emit screenshotsChanged(screenshots);
    }

private:
    QSettings settings;
    ScreenshotList* screenshots;
    General* api;
    Screen* screenApi;
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
        settings->setValue("version", ANXIETY_SETTINGS_VERSION);
        settings->sync();
    }
};

#endif // CONTROLLER_H
