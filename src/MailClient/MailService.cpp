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

Q_GLOBAL_STATIC(MailService, mailService);

MailService::MailService(QObject *parent) : QObject(parent)
{
    _host = QProcessEnvironment::systemEnvironment().value("MAIL_HOST", "");
    _user = QProcessEnvironment::systemEnvironment().value("MAIL_USER", "");
    _pass = QProcessEnvironment::systemEnvironment().value("MAIL_PASS", "");
    _addr = QProcessEnvironment::systemEnvironment().value("MAIL_ADDR", _user);
    _port = QProcessEnvironment::systemEnvironment().value("MAIL_PORT", "25").toInt();

    if(_host.isEmpty() || _user.isEmpty() || _pass.isEmpty())
        return;

    _init = true;
}

MailService *MailService::instance()
{
    return mailService;
}

bool MailService::sendMail(QString receiver, QString subject, QString content)
{
    if(!_init)
        return false;

    // object deletes itself
    _smtp = new Smtp(_user, _pass, _host, _port);
    _smtp->sendMail(_addr, receiver, subject, content);
    return true;
}

bool MailService::sendWelcomeMail(QVariantMap data)
{
    QString url = QProcessEnvironment::systemEnvironment().value("FRRONTEND_HOST", "");

    if(url.isEmpty())
        url = QProcessEnvironment::systemEnvironment().value("VIRTUAL_HOST", "").replace(".io",".in");

    QString userName = data["userName"].toString();
    QString login = data["login"].toString();
    QString password = data["password"].toString();
    QString receiver = data["receiver"].toString();

      QString message =
    "Hallo "+userName+",\r\n\r\n"
    "Willkommen bei 2log! Dir wurde soeben ein Administrations-Konto eingerichtet.\r\n"
    "Du kannst dich mit folgenden Zugangsdaten unter "+url +" einloggen.\r\n\r\n"
    "Login:\t\t"+login+"\r\n"
    "Passwort:\t"+password+"\r\n\r\n"
    "Dein 2log Team";
    return sendMail(receiver, "Wilkommen bei 2log!", message);
}

bool MailService::sendResetPasswordMail(QVariantMap data)
{
    QString url = QProcessEnvironment::systemEnvironment().value("FRRONTEND_HOST", "");

    if(url.isEmpty())
        url = QProcessEnvironment::systemEnvironment().value("VIRTUAL_HOST", "").replace(".io",".in");

    QString userName = data["userName"].toString();
    QString login = data["login"].toString();
    QString password = data["password"].toString();
    QString receiver = data["receiver"].toString();

      QString message =
    "Hallo "+userName+",\r\n\r\n"
    "Willkommen bei 2log! Dir wurde soeben ein neues Password zugewiesen.\r\n"
    "Du kannst dich mit folgenden Zugangsdaten unter "+url +" einloggen.\r\n\r\n"
    "Login:\t\t"+login+"\r\n"
    "Passwort:\t"+password+"\r\n\r\n"
    "Dein 2log Team";
    return sendMail(receiver, "Dein neues 2log Passwort", message);
}


