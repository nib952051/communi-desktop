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

#include "treehighlighter.h"
#include "sharedtimer.h"
#include "treewidget.h"
#include "treeitem.h"
#include "treerole.h"
#include <IrcConnection>
#include <IrcMessage>
#include <IrcBuffer>

TreeHighlighter::TreeHighlighter(TreeWidget* tree) : QObject(tree)
{
    d.tree = tree;
    d.blink = false;

    connect(tree, SIGNAL(bufferAdded(IrcBuffer*)), this, SLOT(onBufferAdded(IrcBuffer*)));
    connect(tree, SIGNAL(bufferRemoved(IrcBuffer*)), this, SLOT(onBufferRemoved(IrcBuffer*)));
    connect(tree, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onItemExpanded(QTreeWidgetItem*)));
    connect(tree, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onItemCollapsed(QTreeWidgetItem*)));
    connect(tree, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onItemChanged(QTreeWidgetItem*,int)));
    connect(tree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
}

void TreeHighlighter::onBufferAdded(IrcBuffer* buffer)
{
    connect(buffer, SIGNAL(activeChanged(bool)), this, SLOT(onBufferChanged()));
    connect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));

    TreeItem* item = d.tree->bufferItem(buffer);
    connect(item, SIGNAL(destroyed(TreeItem*)), this, SLOT(onItemDestroyed(TreeItem*)));
    colorizeItem(item);
}

void TreeHighlighter::onBufferRemoved(IrcBuffer* buffer)
{
    d.items.remove(d.tree->bufferItem(buffer));
    disconnect(buffer, SIGNAL(activeChanged(bool)), this, SLOT(onBufferChanged()));
    disconnect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));
}

void TreeHighlighter::onMessageReceived(IrcMessage* message)
{
    if (message->type() == IrcMessage::Private || message->type() == IrcMessage::Notice) {
        IrcBuffer* buffer = qobject_cast<IrcBuffer*>(sender());
        TreeItem* item = d.tree->bufferItem(buffer);
        if (item && item != d.tree->currentItem()) {
            if (message->property("private").toBool() ||
                    message->property("content").toString().contains(message->connection()->nickName(), Qt::CaseInsensitive)) {
                highlightItem(item);
            }
        }
    }
}

void TreeHighlighter::onItemExpanded(QTreeWidgetItem* item)
{
    static_cast<TreeItem*>(item)->refresh();
}

void TreeHighlighter::onItemCollapsed(QTreeWidgetItem* item)
{
    static_cast<TreeItem*>(item)->refresh();
}

void TreeHighlighter::onItemChanged(QTreeWidgetItem* item, int column)
{
    if (column == 0 && d.items.contains(item)) {
        if (!item->data(0, TreeRole::Highlight).toBool())
            unhighlightItem(item);
    }
}

void TreeHighlighter::onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    unhighlightItem(previous);
    unhighlightItem(current);
}

void TreeHighlighter::highlightItem(QTreeWidgetItem* item)
{
    if (item && !d.items.contains(item)) {
        if (d.items.isEmpty())
            SharedTimer::instance()->registerReceiver(this, "blinkItems");
        d.items.insert(item);
        item->setData(0, TreeRole::Highlight, true);
        colorizeItem(item);
    }
}

void TreeHighlighter::unhighlightItem(QTreeWidgetItem* item)
{
    if (item && d.items.contains(item)) {
        d.items.remove(item);
        if (d.items.isEmpty())
            SharedTimer::instance()->unregisterReceiver(this, "blinkItems");
        item->setData(0, TreeRole::Highlight, false);
        colorizeItem(item);
    }
}

void TreeHighlighter::colorizeItem(QTreeWidgetItem* item)
{
    TreeItem* ti = static_cast<TreeItem*>(item);
    if (ti) {
        const bool hilite = d.blink && d.items.contains(item);
        if (hilite) {
            item->setData(0, Qt::ForegroundRole, QColor("#ff4040"));
            item->setData(1, Qt::BackgroundRole, QColor("#ff4040").lighter(125)); // TODO
        } else {
            const IrcBuffer* buffer = ti->buffer();
            const QColor fg = d.tree->palette().color(QPalette::Disabled, QPalette::Text);
            item->setData(0, Qt::ForegroundRole, buffer->isActive() ? QVariant() : fg);
            item->setData(1, Qt::BackgroundRole, QVariant());
        }

        TreeItem* pi = ti->parentItem();
        if (pi) {
            const IrcBuffer* buffer = pi->buffer();
            const QColor fg = d.tree->palette().color(QPalette::Disabled, QPalette::Text);
            pi->setData(0, Qt::ForegroundRole, hilite && !pi->isExpanded() ? QColor("#ff4040") : buffer->isActive() ? QVariant() : fg); // TODO
        }
    }
}

void TreeHighlighter::onItemDestroyed(TreeItem* item)
{
    d.items.remove(item);
}

void TreeHighlighter::onBufferChanged()
{
    IrcBuffer* buffer = qobject_cast<IrcBuffer*>(sender());
    colorizeItem(d.tree->bufferItem(buffer));
}

void TreeHighlighter::blinkItems()
{
    foreach (QTreeWidgetItem* item, d.items)
        colorizeItem(item);
    d.blink = !d.blink;
}