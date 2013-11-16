/*
* Copyright (C) 2008-2013 The Communi Project
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

#include "titlebar.h"
#include "messageformatter.h"
#include <IrcTextFormat>
#include <QActionEvent>
#include <IrcCommand>
#include <IrcChannel>
#include <QAction>
#include <QEvent>
#include <QMenu>

TitleBar::TitleBar(QWidget* parent) : QLabel(parent)
{
    d.buffer = 0;
    d.formatter = new MessageFormatter(this);

    d.closeButton = new QToolButton(this);
    d.closeButton->setObjectName("close");
    d.closeButton->setVisible(false);
    d.closeButton->adjustSize();

    d.menuButton = new QToolButton(this);
    d.menuButton->setObjectName("menu");
    d.menuButton->setPopupMode(QToolButton::InstantPopup);
    d.menuButton->setMenu(new QMenu(d.menuButton));
    d.menuButton->setVisible(false);
    d.menuButton->adjustSize();

    setWordWrap(true);
    setAttribute(Qt::WA_Hover);
    setOpenExternalLinks(true);
    setTextFormat(Qt::RichText);
}

IrcBuffer* TitleBar::buffer() const
{
    return d.buffer;
}

void TitleBar::setBuffer(IrcBuffer* buffer)
{
    if (d.buffer != buffer) {
        if (d.buffer) {
            IrcChannel* channel = qobject_cast<IrcChannel*>(d.buffer);
            if (channel) {
                disconnect(channel, SIGNAL(destroyed(IrcChannel*)), this, SLOT(cleanup()));
                disconnect(channel, SIGNAL(topicChanged(QString)), this, SLOT(refresh()));
                disconnect(channel, SIGNAL(modeChanged(QString)), this, SLOT(refresh()));
            } else {
                disconnect(d.buffer, SIGNAL(destroyed(IrcBuffer*)), this, SLOT(cleanup()));
            }
            disconnect(d.buffer, SIGNAL(titleChanged(QString)), this, SLOT(refresh()));
        }
        d.buffer = buffer;
        if (d.buffer) {
            IrcChannel* channel = qobject_cast<IrcChannel*>(d.buffer);
            if (channel) {
                connect(channel, SIGNAL(destroyed(IrcChannel*)), this, SLOT(cleanup()));
                connect(channel, SIGNAL(topicChanged(QString)), this, SLOT(refresh()));
                connect(channel, SIGNAL(modeChanged(QString)), this, SLOT(refresh()));
            } else {
                connect(d.buffer, SIGNAL(destroyed(IrcBuffer*)), this, SLOT(cleanup()));
            }
            connect(d.buffer, SIGNAL(titleChanged(QString)), this, SLOT(refresh()));
        }
        refresh();
    }
}

QString TitleBar::topic() const
{
    IrcChannel* channel = qobject_cast<IrcChannel*>(d.buffer);
    if (channel)
        return channel->topic();
    return QString();
}

void TitleBar::setTopic(const QString& topic)
{
    IrcChannel* channel = qobject_cast<IrcChannel*>(d.buffer);
    if (channel) {
        if (channel->topic() != topic)
            channel->sendCommand(IrcCommand::createTopic(channel->title(), topic));
    }
}

bool TitleBar::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::Enter:
        d.closeButton->setVisible(true);
        d.menuButton->setVisible(!d.menuButton->menu()->actions().isEmpty());
        break;
    case QEvent::Leave:
        d.closeButton->setVisible(false);
        d.menuButton->setVisible(false);
        break;
    case QEvent::ActionAdded:
        d.menuButton->menu()->addAction(static_cast<QActionEvent*>(event)->action());
        break;
    case QEvent::ActionRemoved:
        d.menuButton->menu()->removeAction(static_cast<QActionEvent*>(event)->action());
        break;
    default:
        break;
    }
    return QLabel::event(event);
}

void TitleBar::resizeEvent(QResizeEvent* event)
{
    QLabel::resizeEvent(event);
    QRect r = d.closeButton->rect();
    r.moveTopRight(rect().topRight());
    d.closeButton->setGeometry(r);
    r.moveTopRight(d.closeButton->geometry().topLeft() - QPoint(1, 0));
    d.menuButton->setGeometry(r);
}

void TitleBar::cleanup()
{
    d.buffer = 0;
    refresh();
}

void TitleBar::refresh()
{
    QString title = d.buffer ? d.buffer->title() : QString();
    IrcChannel* channel = qobject_cast<IrcChannel*>(d.buffer);
    QString topic = channel ? channel->topic() : QString();
    if (!topic.isEmpty())
        title = tr("%1: %2").arg(title, d.formatter->formatHtml(topic));
    setText(title);
}