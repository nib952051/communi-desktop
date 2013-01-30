/*
* Copyright (C) 2008-2013 J-P Nurmi <jpnurmi@gmail.com>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#ifndef MULTISESSIONTABWIDGET_H
#define MULTISESSIONTABWIDGET_H

#include <QStackedWidget>
#include "settings.h"

class Session;
class IrcMessage;
class MessageView;
class SessionTabWidget;

class MultiSessionTabWidget : public QStackedWidget
{
    Q_OBJECT

public:
    MultiSessionTabWidget(QWidget* parent = 0);

    QList<Session*> sessions() const;
    SessionTabWidget* sessionWidget(Session* session) const;

    QByteArray saveSplitter() const;

public slots:
    void addSession(Session* session);
    void removeSession(Session* session);

    void restoreSplitter(const QByteArray& state);
    void applySettings(const Settings& settings);

signals:
    void sessionAdded(Session* session);
    void sessionRemoved(Session* session);

    void alerted(MessageView* view, IrcMessage* message);
    void highlighted(MessageView* view, IrcMessage* message);

    void splitterChanged(const QByteArray& state);

    void newTabRequested();

private:
    struct MainTabWidgetData {
        Settings settings;
    } d;
};

#endif // MULTISESSIONTABWIDGET_H
