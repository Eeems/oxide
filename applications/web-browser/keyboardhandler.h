#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <Qt>
#include <QQmlApplicationEngine>
#include <QChar>
#include <QKeyEvent>
#include <QCoreApplication>
#include <QDebug>
#include <QQuickItem>

class KeyboardHandler : public QObject {
    Q_OBJECT
public:
    explicit KeyboardHandler(QObject *parent = 0) : QObject(parent) {}
   QQuickItem* root;
    Q_INVOKABLE void charPress(const QChar &character, const Qt::KeyboardModifiers &modifier){
        if(character != '\x0'){
            int key = character.unicode();
            auto targets = focusObjects();
            if(targets.empty()){
                qDebug() << "No target";
                return;
            }
            qDebug() << targets;
            for(auto target : targets){
                QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, key, modifier, QString(character));
                QCoreApplication::postEvent(target, event);
            }
            qDebug() << "Char pressed: " << QKeySequence(key + modifier).toString();
        }else{
            qDebug() << "Invalid char";
            keyPress(Qt::Key_unknown, modifier, QString());
        }
    }
    Q_INVOKABLE void keyPress(const Qt::Key &key, const Qt::KeyboardModifiers &modifier, const QString &text){
        auto targets = focusObjects();
        if(targets.empty()){
            qDebug() << "No target";
            return;
        }
        qDebug() << targets;
        for(auto target : targets){
            QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, key, modifier, text);
            QCoreApplication::postEvent(target, event);
        }
        QKeySequence sequence;
        switch(key){
            case Qt::Key_Shift:
            case Qt::Key_Alt:
            case Qt::Key_AltGr:
            case Qt::Key_Control:
            case Qt::Key_Meta:
                sequence = QKeySequence(modifier);
                break;
            default:
                sequence = QKeySequence(key + modifier);
                break;
        }
        qDebug() << "Key pressed: " << sequence.toString() << " text: " << text;
    }
    Q_INVOKABLE void stringPress(const QString &text, const Qt::KeyboardModifiers &modifier){
        auto targets = focusObjects();
        if(targets.empty()){
            qDebug() << "No target";
            return;
        }
        qDebug() << targets;
        for(auto target : targets){
            QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, Qt::Key_unknown, modifier, text);
            QCoreApplication::postEvent(target, event);
            QVariant readOnly = target->property("readOnly");
            QVariant length = target->property("length");
            if(target->property("text").isValid() && length.isValid() && readOnly.isValid() && !readOnly.toBool()){
                QMetaObject::invokeMethod(target, "insert", Q_ARG(int, length.toInt()), Q_ARG(QString, text));
            }
        }
        qDebug() << "String pressed: " << QKeySequence(text + modifier).toString();
    }
    QList<QQuickItem*> focusObjects(){
        QList<QQuickItem*> focusItems;
        for(auto widget : root->findChildren<QQuickItem*>()){
            if(widget->hasActiveFocus()){
                focusItems.append(widget);
            }
        }
        return focusItems;
    }
};

#endif // KEYBOARD_H
