#ifndef APPSAPI_H
#define APPSAPI_H

#include <QDBusMetaType>
#include <QDebug>
#include <QDBusObjectPath>
#include <QSettings>
#include <QUuid>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "apibase.h"
#include "application.h"
#include "signalhandler.h"

#define OXIDE_SETTINGS_VERSION 1

#define appsAPI AppsAPI::singleton()

class AppsAPI : public APIBase {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", OXIDE_APPS_INTERFACE)
    Q_PROPERTY(int state READ state) // This needs to be here for the XML to generate the other properties :(
    Q_PROPERTY(QDBusObjectPath startupApplication READ startupApplication WRITE setStartupApplication)
    Q_PROPERTY(QVariantMap applications READ getApplications)
    Q_PROPERTY(QDBusObjectPath currentApplication READ currentApplication)
    Q_PROPERTY(QVariantMap runningApplications READ runningApplications)
    Q_PROPERTY(QVariantMap pausedApplications READ pausedApplications)
public:
    static AppsAPI* singleton(AppsAPI* self = nullptr){
        static AppsAPI* instance;
        if(self != nullptr){
            instance = self;
        }
        return instance;
    }
    AppsAPI(QObject* parent);
    ~AppsAPI() {
        m_stopping = true;
        writeApplications();
        settings.sync();
        for(auto app : applications){
            app->stop();
        }
        for(auto app : applications){
            app->waitForFinished();
            app->deleteLater();
        }
        applications.clear();
    }
    void startup();
    int state() { return 0; } // Ignore this, it's a kludge to get the xml to generate

    enum ApplicationType { Foreground, Background, Backgroundable};
    Q_ENUM(ApplicationType)

    void setEnabled(bool enabled){
        qDebug() << "Apps API" << enabled;
        for(auto app : applications){
            if(enabled){
                app->registerPath();
            }else{
                app->unregisterPath();
            }
        }
    }

    Q_INVOKABLE QDBusObjectPath registerApplication(QVariantMap properties){
        QString name = properties.value("name", "").toString();
        QString bin = properties.value("bin", "").toString();
        int type = properties.value("type", Foreground).toInt();
        if(type < Foreground || type > Background || name.isEmpty() || bin.isEmpty() || !QFile::exists(bin)){
            return QDBusObjectPath("/");
        }
        if(applications.contains(name)){
            return applications[name]->qPath();
        }
        auto path = QDBusObjectPath(getPath(name));
        auto app = new Application(path, reinterpret_cast<QObject*>(this));
        auto displayName = properties.value("displayName", name).toString();
        app->setConfig(properties);
        applications.insert(name, app);
        writeApplications();
        app->registerPath();
        emit applicationRegistered(path);
        return path;
    }
    Q_INVOKABLE bool unregisterApplication(QDBusObjectPath path){
        auto app = getApplication(path);
        if(app == nullptr){
            return true;
        }
        if(app->systemApp()){
            return false;
        }
        unregisterApplication(app);
        return true;
    }

    Q_INVOKABLE void reload(){
        readApplications();
        writeApplications();
    }

    QDBusObjectPath startupApplication(){
        return m_startupApplication;
    }
    void setStartupApplication(QDBusObjectPath path){
        if(getApplication(path) != nullptr){
            m_startupApplication = path;
            settings.setValue("startupApplication", path.path());
        }
    }

    QVariantMap getApplications(){
        QVariantMap result;
        for(auto app : applications){
            result.insert(app->name(), QVariant::fromValue(app->qPath()));
        }
        return result;
    }

    QDBusObjectPath currentApplication(){
        for(auto app : applications){
            if(app->state() == Application::InForeground){
                return app->qPath();
            }
        }
        return QDBusObjectPath("/");
    }
    QVariantMap runningApplications(){
        QVariantMap result;
        for(auto app : applications){
            auto state = app->state();
            if(state == Application::InForeground || state == Application::InBackground){
                result.insert(app->name(), QVariant::fromValue(app->qPath()));
            }
        }
        return result;
    }
    QVariantMap pausedApplications(){
        QVariantMap result;
        for(auto app : applications){
            auto state = app->state();
            if(state == Application::Paused){
                result.insert(app->name(), QVariant::fromValue(app->qPath()));
            }
        }
        return result;
    }

    void unregisterApplication(Application* app){
        auto name = app->name();
        if(applications.contains(name)){
            applications.remove(name);
            emit applicationUnregistered(app->qPath());
            app->deleteLater();
            writeApplications();
        }
    }
    void pauseAll(){
        for(auto app : applications){
            app->pause(false);
        }
    }
    void resumeIfNone(){
        if(m_stopping){
            return;
        }
        for(auto app : applications){
            if(app->state() == Application::InForeground){
                return;
            }
        }
        auto app = getApplication(m_startupApplication);
        if(app != nullptr){
            app->launch();
        }
    }
    Application* getApplication(QDBusObjectPath path){
        for(auto app : applications){
            if(app->path() == path.path()){
                return app;
            }
        }
        return nullptr;
    }
    Q_INVOKABLE QDBusObjectPath getApplicationPath(QString name){
        auto app = getApplication(name);
        if(app == nullptr){
            return QDBusObjectPath("/");
        }
        return app->qPath();
    }
    Application* getApplication(QString name){
        if(applications.contains(name)){
            return applications[name];
        }
        return nullptr;
    }
    void connectSignals(Application* app, int signal){
        switch(signal){
            case 1:
                connect(signalHandler, &SignalHandler::sigUsr1, app, &Application::sigUsr1);
            break;
            case 2:
                connect(signalHandler, &SignalHandler::sigUsr2, app, &Application::sigUsr2);
            break;
        }
    }
    void disconnectSignals(Application* app, int signal){
        switch(signal){
            case 1:
                disconnect(signalHandler, &SignalHandler::sigUsr1, app, &Application::sigUsr1);
            break;
            case 2:
                disconnect(signalHandler, &SignalHandler::sigUsr2, app, &Application::sigUsr2);
            break;
        }
    }
signals:
    void applicationRegistered(QDBusObjectPath);
    void applicationLaunched(QDBusObjectPath);
    void applicationUnregistered(QDBusObjectPath);
    void applicationPaused(QDBusObjectPath);
    void applicationResumed(QDBusObjectPath);
    void applicationSignaled(QDBusObjectPath);
    void applicationExited(QDBusObjectPath, int);

public slots:
    void leftHeld(){
        auto currentApplication = getApplication(this->currentApplication());
        if(currentApplication->state() != Application::Inactive && currentApplication->path() == m_startupApplication.path()){
            qDebug() << "Already at startup application";
            return;
        }
        auto app = getApplication(m_startupApplication);
        if(app != nullptr){
            app->launch();
        }
    }
    void homeHeld(){
        auto app = getApplication("codes.eeems.erode");
        if(app == nullptr){
            qDebug() << "Unable to find process manager";
            return;
        }
        if(app->state() == Application::InForeground){
            qDebug() << "Process manager already running";
            return;
        }
        app->launch();
    }

private:
    bool m_stopping;
    bool m_enabled;
    QMap<QString, Application*> applications;
    QSettings settings;
    QDBusObjectPath m_startupApplication;
    bool m_sleeping;
    Application* resumeApp = nullptr;
    QString getPath(QString name){
        static const QUuid NS = QUuid::fromString(QLatin1String("{d736a9e1-10a9-4258-9634-4b0fa91189d5}"));
        return QString(OXIDE_SERVICE_PATH) + "/apps/" + QUuid::createUuidV5(NS, name).toString(QUuid::Id128);
    }
    void writeApplications(){
        auto apps = applications.values();
        int size = apps.size();
        settings.beginWriteArray("applications", size);
        for(int i = 0; i < size; ++i){
            settings.setArrayIndex(i);
            auto app = apps[i];
            auto config = app->getConfig();
            for(auto key : config.keys()){
                settings.setValue(key, config[key]);
            }
        }
        settings.endArray();
    }
    void readApplications(){
        settings.sync();
        if(!applications.empty()){
            int size = settings.beginReadArray("applications");
            QStringList names;
            for(int i = 0; i < size; ++i){
                settings.setArrayIndex(i);
                names << settings.value("name").toString();
            }
            settings.endArray();
            for(auto name : applications.keys()){
                auto app = applications[name];
                if(!names.contains(name) && !app->systemApp()){
                    app->unregister();
                }
            }
        }
        int size = settings.beginReadArray("applications");
        for(int i = 0; i < size; ++i){
            settings.setArrayIndex(i);
            auto name = settings.value("name").toString();
            auto displayName = settings.value("displayName", name).toString();
            auto type = settings.value("type", Foreground).toInt();
            auto bin = settings.value("bin").toString();
            if(type < Foreground || type > Background || name.isEmpty() || bin.isEmpty()){
                continue;
            }
            QVariantMap properties {
                {"name", name},
                {"displayName", displayName},
                {"description", settings.value("description", displayName).toString()},
                {"bin", bin},
                {"type", type},
                {"flags", settings.value("flags", QStringList()).toStringList()},
                {"icon", settings.value("icon", "").toString()},
                {"onPause", settings.value("onPause", "").toString()},
                {"onResume", settings.value("onResume", "").toString()},
                {"onStop", settings.value("onStop", "").toString()},
                {"environment", settings.value("environment", QVariantMap()).toMap()},
                {"workingDirectory", settings.value("workingDirectory", "").toString()}
            };
            if(settings.contains("user")){
                properties.insert("user", settings.value("user", "").toString());
            }
            if(settings.contains("group")){
                properties.insert("group", settings.value("group", "").toString());
            }
            if(applications.contains(name)){
                applications[name]->setConfig(properties);
                writeApplications();
            }else{
                registerApplication(properties);
            }
        }
        settings.endArray();
        QDir dir("/opt/usr/share/applications/");
        dir.setNameFilters(QStringList() << "*.oxide");
        QMap<QString, QJsonObject> apps;
        for(auto entry : dir.entryInfoList()){
            QFile file(entry.filePath());
            if(!file.open(QIODevice::ReadOnly)){
                continue;
            }
            auto data = file.readAll();
            auto app = QJsonDocument::fromJson(data).object();
            auto name = entry.completeBaseName();
            app["name"] = name;
            apps.insert(name, app);
        }
        for(auto application : applications.values()){
            auto name = application->name();
            if(!apps.contains(name)){
                continue;
            }
            if(!application->systemApp()){
                application->unregister();
                continue;
            }
            apps.remove(name);
        }
        for(auto app : apps){
            auto name = app["name"].toString();
            int type = Foreground;
            QString typeString = app.contains("type") ? app["type"].toString().toLower() : "";
            if(typeString == "background"){
                type = Background;
            }else if(typeString == "backgroundable"){
                type = Backgroundable;
            }else if(!typeString.isEmpty() && typeString != "foreground"){
                qDebug() << "Invalid type string:" << typeString;
            }
            auto bin = app["bin"].toString();
            if(!QFile::exists(bin)){
                qDebug() << "Can't find application binary:" << bin;
                continue;
            }
            auto flags = QStringList() << "system";
            if(app.contains("flags")){
                for(auto flag : app["flags"].toArray()){
                    auto value = flag.toString();
                    if(!value.isEmpty() && value != "system"){
                        flags << value;
                    }
                }
            }
            QVariantMap properties {
                {"name", name},
                {"bin", bin},
                {"type", type},
                {"flags", flags}
            };
            if(app.contains("displayName")){
                properties.insert("displayName", app["displayName"].toString());
            }
            if(app.contains("description")){
                properties.insert("description", app["description"].toString());
            }
            if(app.contains("icon")){
                properties.insert("icon", app["icon"].toString());
            }
            if(app.contains("user")){
                properties.insert("user", app["user"].toString());
            }
            if(app.contains("group")){
                properties.insert("group", app["group"].toString());
            }
            if(app.contains("workingDirectory")){
                properties.insert("workingDirectory", app["workingDirectory"].toString());
            }
            if(app.contains("events")){
                auto events = app["evnets"].toObject();
                for(auto event : events.keys()){
                    if(event == "stop"){
                        properties.insert("onStop", events[event].toString());
                    }else if(event == "pause"){
                        properties.insert("pause", events[event].toString());
                    }else if(event == "resume"){
                        properties.insert("resume", events[event].toString());
                    }
                }
            }
            if(app.contains("environment")){
                QVariantMap envMap;
                auto environment = app["environment"].toObject();
                for(auto key : environment.keys()){
                    envMap.insert(key, environment[key].toString());
                }
            }
            if(applications.contains(name)){
                applications[name]->setConfig(properties);
                writeApplications();
            }else{
                registerApplication(properties);
            }
        }
    }
    static void migrate(QSettings* settings, int fromVersion){
        Q_UNUSED(settings)
        Q_UNUSED(fromVersion)
        // In the future migrate changes to settings between versions
    }
};
#endif // APPSAPI_H
