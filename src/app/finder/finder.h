/*
* Copyright (C) 2008-2014 The Communi Project
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

#ifndef FINDER_H
#define FINDER_H

#include <QSet>
#include <QObject>
#include <QShortcut>

class ChatPage;
class BufferView;
class AbstractFinder;

class Finder : public QObject
{
    Q_OBJECT

public:
    explicit Finder(ChatPage* page);

public slots:
    void searchTree();
    void cancelTreeSearch();

    void searchList(BufferView* view = 0);
    void cancelListSearch(BufferView* view = 0);

    void searchBrowser(BufferView* view = 0);
    void cancelBrowserSearch(BufferView* view = 0);

private slots:
    void startSearch(AbstractFinder* input);
    void finderDestroyed(AbstractFinder* input);

private:
    struct Private {
        ChatPage* page;
        QSet<AbstractFinder*> finders;
        QShortcut* nextShortcut;
        QShortcut* prevShortcut;
        QShortcut* cancelShortcut;
    } d;
};

#endif // FINDER_H
