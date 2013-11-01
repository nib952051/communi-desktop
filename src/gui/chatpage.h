/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QSplitter>

class TreeView;
class SplitView;
class IrcBuffer;
class IrcConnection;

class ChatPage : public QSplitter
{
    Q_OBJECT

public:
    ChatPage(QWidget* parent = 0);
    ~ChatPage();

    IrcBuffer* currentBuffer() const;

    QList<IrcConnection*> connections() const;
    void addConnection(IrcConnection* connection);
    void removeConnection(IrcConnection* connection);

signals:
    void currentBufferChanged(IrcBuffer* buffer);

private:
    struct Private {
        TreeView* treeView;
        SplitView* splitView;
        QList<IrcConnection*> connections;
    } d;
};

#endif // CHATPAGE_H