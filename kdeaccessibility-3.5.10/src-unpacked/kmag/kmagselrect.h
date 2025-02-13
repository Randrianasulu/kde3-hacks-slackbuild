/***************************************************************************
                          kmagselrect.h  -  description
                             -------------------
    begin                : Mon Feb 12 23:45:41 EST 2001
    copyright            : (C) 2001-2003 by Sarang Lakare
    email                : sarang#users.sf.net
    copyright            : (C) 2003-2004 by Olaf Schmidt
    email                : ojschmidt@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KMAGSELRECT_H
#define KMAGSELRECT_H

#include <stdlib.h>

// Qt includes
#include <qrect.h>
#include <qwidget.h>
#include <qlabel.h>

class KMagSelWinCorner : public QLabel
{
    Q_OBJECT

public:

    KMagSelWinCorner ( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );

    virtual ~KMagSelWinCorner();

signals:

    void startResizing ();
    void resized ( QPoint offset );

protected:

    QPoint oldPos;

    virtual void mousePressEvent ( QMouseEvent * e );
    virtual void mouseReleaseEvent ( QMouseEvent * e );
    virtual void mouseMoveEvent ( QMouseEvent * e );
};

class KMagSelWin : public QWidget
{
    Q_OBJECT

public:

    KMagSelWin ( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );

    virtual ~KMagSelWin();

    void setSelRect ( QRect selRect );
    QRect getSelRect ();

public slots:

    void startResizing ();
    void titleMoved ( QPoint offset );
    void topLeftResized ( QPoint offset );
    void topRightResized ( QPoint offset );
    void bottomLeftResized ( QPoint offset );
    void bottomRightResized ( QPoint offset );

signals:

    void resized();

protected:

    QRect oldSelRect;

    KMagSelWinCorner *titleBar;
    KMagSelWinCorner *topLeftCorner;
    KMagSelWinCorner *topRightCorner;
    KMagSelWinCorner *bottomLeftCorner;
    KMagSelWinCorner *bottomRightCorner;
};

/**
 * This class stores the selected rectangular area for grabbing. It also displays the
 * rectangular area on demand.
 *
 * @author Original : Michael Forster
 * @author Current : Sarang Lakare
 */
class KMagSelRect : public QObject, public QRect
{
    Q_OBJECT

public:
    KMagSelRect(QWidget *parent=0);
    KMagSelRect(const QPoint &topLeft, const QPoint &bottomRight,
      QWidget *parent=0);
    KMagSelRect(const QPoint &topLeft, const QSize &size,
      QWidget *parent=0);
    KMagSelRect(int left, int top, int width, int height,
      QWidget *selWindowParent=0);

    virtual ~KMagSelRect();

    WId winId();

    bool visible();

    /// Makes the rectangle always visible
    void alwaysVisible(bool visible=true);

    /// Returns true if always visible is set
    bool getAlwaysVisible() const {
      return (m_alwaysVisible);
    };

public slots:

    void show();
    void hide();
    void update();

    void selWinResized();

protected:

    void init(QWidget *);

    QWidget *selWindowParent;
    KMagSelWin *selectionwindow;
    bool m_alwaysVisible;

};

void setTitleColors (QColor title, QColor text, QColor titleBtn);
void setFrameSize (int size);

#endif // KMAGSELRECT_H
