#include "MailManager.h"
#include <QProcessEnvironment>
#include <QVariant>
#include "MailService.h"
#include "Storage/FileSystemPaths.h"
#include "Storage/FileSystemLoader.h"
#include <QDir>
#include <QDirIterator>

bool MailManager::_initialized = false;
QString MailManager::_emailTemplatePath = "";

MailManager::MailManager(QObject *parent)
    : QObject{parent}
{
    if(!_initialized)
    {
        _emailTemplatePath = FileSystemPaths::instance()->getConfigPath()+"/email/templates/";
        QDir dir(_emailTemplatePath);
        if (!dir.exists())
            dir.mkpath(_emailTemplatePath);
        if(dir.isEmpty())
        {
            QDirIterator it(":/templates");
            while(it.hasNext())
            {
                it.next();
                QFile::copy(it.filePath(), _emailTemplatePath + it.fileName());
                qInfo()<<"Copy template from : "<< it.filePath() << "to" <<  it.fileName();
            }
        }
        _initialized = true;
    }
}


bool MailManager::sendMailFromTemplate(QString receiver, const QString &templateName, const QVariantMap &data)
{
    // try to open template from filesystem
    QFile file(_emailTemplatePath+templateName);
    if( file.open(QFile::ReadOnly))
    {
        QString templateString = file.readAll();
        file.close();
        qDebug() <<"Open template file from filesystem to send eMail to"<< receiver;
        ParsedTemplate mailContent = parseTemplate(templateString, data);
        return MailService::instance()->sendMail(receiver, mailContent.subject, mailContent.message);
    }

    // try fallback template
    QFile qrcFile(":/templates/"+templateName);
    if(qrcFile.open(QFile::ReadOnly))
    {
        QString templateString = qrcFile.readAll();
        file.close();
        qDebug() <<"Open template file from resources to send eMail to"<< receiver;
        ParsedTemplate mailContent = parseTemplate(templateString, data);
        return MailService::instance()->sendMail(receiver, mailContent.subject, mailContent.message);
    }
    return false;
}

MailManager::ParsedTemplate MailManager::parseTemplate(QString templateString, QVariantMap data)
{
    QString url = QProcessEnvironment::systemEnvironment().value("FRRONTEND_HOST", "");

    if(url.isEmpty())
        url = QProcessEnvironment::systemEnvironment().value("VIRTUAL_HOST", "").replace(".io",".in");

    data["2log_url"] = url;

    QMapIterator<QString, QVariant> dataIt(data);
    while(dataIt.hasNext())
    {
        dataIt.next();
        QString key = "$"+dataIt.key().toUpper();
        QString value;

        if(key == "$TOTAL" || key == "$BALANCE")
        {
            value = QLocale().toCurrencyString((float) dataIt.value().toInt() / 100, "€");
        }
        else
        {
            value  = dataIt.value().toString();
        }
        templateString = templateString.replace(key, value);
    }

    ParsedTemplate content;
    QStringList lines = templateString.split("\n");
    if(lines.count() > 2 && lines[1].isEmpty())
    {
        QString subject = lines[0];
        content.subject = subject;
        lines.removeFirst();
        lines.removeFirst();
    }

    templateString = lines.join("\n").trimmed();
    content.message = templateString;
    return content;
}





