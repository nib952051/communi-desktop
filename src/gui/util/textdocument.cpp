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

#include "textdocument.h"
#include <QAbstractTextDocumentLayout>
#include <QTextCursor>
#include <QTextBlock>
#include <QPainter>
#include <qmath.h>

static int delay = 1000;

TextDocument::TextDocument(QObject* parent) : QTextDocument(parent)
{
    d.note = -1;
    d.dirty = -1;
    d.active = false;
}

int TextDocument::totalCount() const
{
    return blockCount() + d.buffer.count();
}

bool TextDocument::isActive() const
{
    return d.active;
}

void TextDocument::setActive(bool active)
{
    d.active = active;
    if (active && d.dirty)
        flushBuffer();
}

int TextDocument::note() const
{
    return d.note;
}

void TextDocument::setNote(int note)
{
    if (d.note != note) {
        removeMarker(d.note);
        if (note != -1)
            addMarker(note);
        d.note = note;
    }
}

void TextDocument::addMarker(int block)
{
    if (block == -1)
        block = totalCount() - 1;
    d.markers.append(block);
    updateBlock(block);
}

void TextDocument::removeMarker(int block)
{
    if (d.markers.removeOne(block))
        updateBlock(block);
}

void TextDocument::addHighlight(int block)
{
    if (block == -1)
        block = totalCount() - 1;
    d.highlights.append(block);
    updateBlock(block);
}

void TextDocument::removeHighlight(int block)
{
    if (d.highlights.removeOne(block))
        updateBlock(block);
}

void TextDocument::append(const QString& text, bool highlight)
{
    if (!text.isEmpty()) {
        if (highlight)
            d.highlights.append(totalCount() - 1);
        if (d.active || d.dirty == 0) {
            QTextCursor cursor(this);
            cursor.beginEditBlock();
            appendLine(cursor, text);
            cursor.endEditBlock();
        } else {
            if (d.dirty <= 0) {
                d.dirty = startTimer(delay);
                delay += 1000;
            }
            d.buffer += text;
        }
    }
}

void TextDocument::drawMarkers(QPainter* painter, const QRect& bounds)
{
    if (!d.markers.isEmpty()) {
        QAbstractTextDocumentLayout* layout = documentLayout();
        foreach (int marker, d.markers) {
            QTextBlock block = findBlockByNumber(marker);
            if (block.isValid()) {
                QRect br = layout->blockBoundingRect(block).toAlignedRect();
                if (bounds.intersects(br))
                    painter->drawLine(br.topLeft(), br.topRight());
            }
        }
    }
}

void TextDocument::drawHighlights(QPainter* painter, const QRect& bounds)
{
    if (!d.highlights.isEmpty()) {
        int margin = qCeil(documentMargin());
        QAbstractTextDocumentLayout* layout = documentLayout();
        foreach (int highlight, d.highlights) {
            QTextBlock block = findBlockByNumber(highlight);
            if (block.isValid()) {
                QRect br = layout->blockBoundingRect(block).toAlignedRect();
                if (bounds.intersects(br))
                    painter->drawRect(br.adjusted(-margin, 0, margin, 0));
            }
        }
    }
}

void TextDocument::updateBlock(int number)
{
    if (d.active) {
        QTextBlock block = findBlockByNumber(number);
        if (block.isValid())
            QMetaObject::invokeMethod(documentLayout(), "updateBlock", Q_ARG(QTextBlock, block));
    }
}

void TextDocument::timerEvent(QTimerEvent* event)
{
    QTextDocument::timerEvent(event);
    if (event->timerId() == d.dirty) {
        delay -= 1000;
        flushBuffer();
    }
}

void TextDocument::flushBuffer()
{
    if (!d.buffer.isEmpty()) {
        QTextCursor cursor(this);
        cursor.beginEditBlock();
        foreach (const QString& line, d.buffer)
            appendLine(cursor, line);
        cursor.endEditBlock();
    }

    if (d.dirty > 0) {
        killTimer(d.dirty);
        d.dirty = 0;
    }
}

void TextDocument::appendLine(QTextCursor& cursor, const QString& line)
{
    cursor.movePosition(QTextCursor::End);
    if (!isEmpty())
        cursor.insertBlock();

#if QT_VERSION >= 0x040800
    QTextBlockFormat format = cursor.blockFormat();
    format.setLineHeight(120, QTextBlockFormat::ProportionalHeight);
    cursor.setBlockFormat(format);
#endif // QT_VERSION

    cursor.insertHtml(line);
}
