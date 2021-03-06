/*   2log.io
 *   Copyright (C) 2021 - 2log.io | mail@2log.io,  mail@friedemann-metzger.de
 *   
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Affero General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Affero General Public License for more details.
 *
 *   You should have received a copy of the GNU Affero General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "MailService.h"
#include <QProcessEnvironment>
#include <QtConcurrent>

Q_GLOBAL_STATIC(MailService, mailService);

MailService::MailService(QObject *parent) : QObject(parent)
{
    _host = QProcessEnvironment::systemEnvironment().value("MAIL_HOST", "");
    _user = QProcessEnvironment::systemEnvironment().value("MAIL_USER", "");
    _pass = QProcessEnvironment::systemEnvironment().value("MAIL_PASS", "");
    _addr = QProcessEnvironment::systemEnvironment().value("MAIL_ADDR", "");
    _sender = QProcessEnvironment::systemEnvironment().value("MAIL_SENDER", "");

    QString type = QProcessEnvironment::systemEnvironment().value("MAIL_CONNECTION_TYPE", "SSL");

    if(type == "SSL" || type.isEmpty())
    {
        _port = QProcessEnvironment::systemEnvironment().value("MAIL_PORT", "465").toUInt();
        _connType = SmtpClient::TlsConnection;
    }
    else if(type == "TLS")
    {
        _port = QProcessEnvironment::systemEnvironment().value("MAIL_PORT", "587").toUInt();
        _connType = SmtpClient::TlsConnection;
    }
    else if (type == "TCP")
    {
        _port = QProcessEnvironment::systemEnvironment().value("MAIL_PORT", "25").toUInt();
        _connType = SmtpClient::TcpConnection;
    }

    if(_host.isEmpty()) {
        qWarning()<< "eMail is not configured. Set the apropriate environment variables.";
        return;
    }

    qDebug()<<"Mail-Port:"<<_port ;
    qDebug()<<"Connection-Type:"<<type;

    _init = true;
}

MailService *MailService::instance()
{
    return mailService;
}

bool MailService::sendMail(QString receiver, QString subject, QString content)
{
    if(!_init)
    {
        qDebug() << "No smtp configuration. Can't send eMail.";
        return false;
    }
    QtConcurrent::run([=]()
    {
        SmtpClient smtp(_host, _port, _connType);
        if(!_user.isEmpty())
            smtp.setUser(_user);
        if(!_pass.isEmpty())
            smtp.setPassword(_pass);

        MimeMessage message;
        EmailAddress sender(_addr, _sender);
        message.setSender(&sender);
        EmailAddress to(receiver);
        message.addRecipient(&to);
        message.setSubject(subject);

        MimeText text;

        text.setText(content);
        message.addPart(&text);

        if (!smtp.connectToHost()) {
            qDebug() << "Sending eMail: Failed to connect to host!" << endl;
        }

        if (!smtp.login() && !_user.isEmpty()) {
            qDebug() << "Sending eMail: Failed to login!" << endl;
        }

        if (!smtp.sendMail(message)) {
            qDebug() << "Sending eMail: Failed to send mail!" << endl;
        }
        else
        {
            qDebug() << "Sending eMail: Succeeded" << endl;
        }
        smtp.quit();});

    return true;
}
