#ifndef BSS_H
#define BSS_H

#include <QMutableListIterator>

#include "supplicant.h"
#include "network.h"
#include "dbussettings.h"

class BSS : public QObject{
    Q_OBJECT
    Q_CLASSINFO("Version", OXIDE_INTERFACE_VERSION)
    Q_CLASSINFO("D-Bus Interface", OXIDE_BSS_INTERFACE)
    Q_PROPERTY(QString bssid READ bssid)
    Q_PROPERTY(QString ssid READ ssid)
    Q_PROPERTY(bool privacy READ privacy)
    Q_PROPERTY(ushort frequency READ frequency)
    Q_PROPERTY(ushort signal READ signal)
    Q_PROPERTY(QDBusObjectPath network READ network)
    Q_PROPERTY(QStringList key_mgmt READ key_mgmt)
public:
    BSS(QString path, QString bssid, QString ssid, QObject* parent);
    BSS(QString path, IBSS* bss, QObject* parent) : BSS(path, bss->bSSID(), bss->sSID(), parent) {}

    ~BSS(){ unregisterPath(); }
    QString path(){ return m_path; }
    void registerPath(){
        auto bus = QDBusConnection::systemBus();
        bus.unregisterObject(path(), QDBusConnection::UnregisterTree);
        if(bus.registerObject(path(), this, QDBusConnection::ExportAllContents)){
            qDebug() << "Registered" << path() << OXIDE_BSS_INTERFACE;
        }else{
            qDebug() << "Failed to register" << path();
        }
    }
    void unregisterPath(){
        auto bus = QDBusConnection::systemBus();
        if(bus.objectRegisteredAt(path()) != nullptr){
            qDebug() << "Unregistered" << path();
            bus.unregisterObject(path());
        }
    }

    QString bssid(){ return m_bssid; }
    QString ssid(){ return m_ssid; }

    QList<QString> paths(){
        QList<QString> result;
        for(auto bss : bsss){
            result.append(bss->path());
        }
        return result;
    }
    void addBSS(QString path, Interface* interface){
        if(paths().contains(path)){
            return;
        }
        auto bss = new IBSS(WPA_SUPPLICANT_SERVICE, path, QDBusConnection::systemBus(), interface);
        bsss.append(bss);
        QObject::connect(bss, &IBSS::PropertiesChanged, this, &BSS::PropertiesChanged, Qt::QueuedConnection);
    }
    void addBSS(IBSS* bss){
        if(paths().contains(bss->path())){
            return;
        }
        bsss.append(bss);
        QObject::connect(bss, &IBSS::PropertiesChanged, this, &BSS::PropertiesChanged, Qt::QueuedConnection);
    }
    void removeBSS(QString path){
        QMutableListIterator<IBSS*> i(bsss);
        while(i.hasNext()){
            auto bss = i.next();
            if(!bss->isValid() || bss->path() == path){
                i.remove();
                bss->deleteLater();
            }
        }
    }
    bool privacy(){
        for(auto bss : bsss){
            if(bss->privacy()){
                return true;
            }
        }
        return false;
    }
    ushort frequency(){
        if(!bsss.size()){
            return 0;
        }
        return bsss.first()->frequency();
    }
    short signal(){
        if(!bsss.size()){
            return 0;
        }
        return bsss.first()->signal();
    }
    QDBusObjectPath network();
    QStringList key_mgmt(){
        QStringList result;
        if(!bsss.size()){
            return result;
        }
        auto bss = bsss.first();
        result.append(bss->wPA()["KeyMgmt"].value<QStringList>());
        result.append(bss->rSN()["KeyMgmt"].value<QStringList>());
        return result;
    }
    Q_INVOKABLE QDBusObjectPath connect();

signals:
    void removed();
    void propertiesChanged(QVariantMap);

private slots:
    void PropertiesChanged(const QVariantMap& properties){
        emit propertiesChanged(properties);
    }

private:
    QString m_path;
    QList<IBSS*> bsss;
    QString m_bssid;
    QString m_ssid;
};

#endif // BSS_H
