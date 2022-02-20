#ifndef MAILMANAGER_H
#define MAILMANAGER_H

#include <QObject>

class MailManager : public QObject
{
    Q_OBJECT

    struct ParsedTemplate
    {
        QString message;
        QString subject;
    };

public:
    explicit MailManager(QObject *parent = nullptr);

public slots:
    bool sendMailFromTemplate(QString receiver, const QString& templateName, const QVariantMap& data);

private:
    ParsedTemplate parseTemplate(const QString templateString, QVariantMap data);
    static QString _emailTemplatePath;
    static bool _initialized;

signals:

};

#endif // MAILMANAGER_H
