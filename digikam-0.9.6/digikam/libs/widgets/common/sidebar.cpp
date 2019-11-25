/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-03-22
 * Description : a widget to manage sidebar in gui.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern.ahrens@kdemail.net>
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>  
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

/** @file sidebar.cpp */

// Qt includes.

#include <qsplitter.h>
#include <qwidgetstack.h>
#include <qdatastream.h>
#include <qtimer.h>

// KDE includes.

#include <kapplication.h>
#include <kconfig.h>
#include <kdeversion.h>
#include <kiconloader.h>

// Local includes.

#include "ddebug.h"
#include "sidebar.h"
#include "sidebar.moc"

namespace Digikam
{

class SidebarPriv
{
public:

    SidebarPriv()
    {
        minimizedDefault = false;
        minimized        = false;
        isMinimized      = false;

        tabs             = 0;
        activeTab        = -1;
        minSize          = 0;
        maxSize          = 0;
        dragSwitchId     = -1;

        stack            = 0;
        splitter         = 0;
        dragSwitchTimer  = 0;
    }

    bool           minimizedDefault;
    bool           minimized;
    bool           isMinimized;      // Backup of minimized status (used with Fullscreen)

    int            tabs;
    int            activeTab;
    int            minSize;
    int            maxSize;
    int            dragSwitchId;

    QWidgetStack  *stack;
    QSplitter     *splitter;
    QSize          bigSize;
    QTimer        *dragSwitchTimer; 

    Sidebar::Side  side;
};

Sidebar::Sidebar(QWidget *parent, const char *name, Side side, bool minimizedDefault)
       : KMultiTabBar(KMultiTabBar::Vertical, parent, name)
{
    d = new SidebarPriv;
    d->minimizedDefault = minimizedDefault;
    d->side             = side;
    d->dragSwitchTimer  = new QTimer(this);

    connect(d->dragSwitchTimer, SIGNAL(timeout()),
            this, SLOT(slotDragSwitchTimer()));
}

Sidebar::~Sidebar()
{
    saveViewState();
    delete d;
}

void Sidebar::updateMinimumWidth()
{
    int width = 0;
    for (int i = 0; i < d->tabs; i++)
    {
        QWidget *w = d->stack->widget(i);
        if (w && w->width() > width)
            width = w->width();
    }
    d->stack->setMinimumWidth(width);
}

void Sidebar::setSplitter(QSplitter *sp)
{
#if KDE_IS_VERSION(3,3,0)
    setStyle(KMultiTabBar::VSNET);
#else
    setStyle(KMultiTabBar::KDEV3);
#endif

    d->splitter = sp;
    d->stack    = new QWidgetStack(d->splitter);

    if(d->side == Left)
        setPosition(KMultiTabBar::Left);
    else
        setPosition(KMultiTabBar::Right);
}

QSplitter* Sidebar::splitter() const
{
    return d->splitter;
}

void Sidebar::loadViewState()
{
    KConfig *config = kapp->config();
    config->setGroup(QString("%1").arg(name()));

    int tab        = config->readNumEntry("ActiveTab", 0);
    bool minimized = config->readBoolEntry("Minimized", d->minimizedDefault);

    // validate
    if(tab >= d->tabs || tab < 0)
        tab = 0;

    if (minimized)
    {
        d->activeTab = tab;
        //setTab(d->activeTab, true);
        d->stack->raiseWidget(d->activeTab);
        emit signalChangedTab(d->stack->visibleWidget());
    }
    else
    {
        d->activeTab = -1;
    }

    clicked(tab);
}

void Sidebar::saveViewState()
{
    KConfig *config = kapp->config();
    config->setGroup(QString("%1").arg(name()));
    config->writeEntry("ActiveTab", d->activeTab);
    config->writeEntry("Minimized", d->minimized);
    config->sync();
}

void Sidebar::backup()
{
    d->isMinimized = d->minimized;

    if (!d->isMinimized) 
        shrink();

    KMultiTabBar::hide();
}

void Sidebar::restore()
{
    if (!d->isMinimized) 
        expand();

    KMultiTabBar::show();
}

void Sidebar::appendTab(QWidget *w, const QPixmap &pic, const QString &title)
{
    w->reparent(d->stack, QPoint(0, 0));
    KMultiTabBar::appendTab(pic, d->tabs, title);
    d->stack->addWidget(w, d->tabs);

    tab(d->tabs)->setAcceptDrops(true);
    tab(d->tabs)->installEventFilter(this);

    connect(tab(d->tabs), SIGNAL(clicked(int)),
            this, SLOT(clicked(int)));

    d->tabs++;
}

void Sidebar::deleteTab(QWidget *w)
{
    int tab = d->stack->id(w);
    if(tab < 0)
        return;

    if(tab == d->activeTab)
        d->activeTab = -1;

    d->stack->removeWidget(d->stack->widget(tab));
    removeTab(tab);
    d->tabs--;
    updateMinimumWidth();

    //TODO show another widget
}

void Sidebar::clicked(int tab)
{
    if(tab >= d->tabs || tab < 0)
        return;

    if(tab == d->activeTab)
    {
        d->stack->isHidden() ? expand() : shrink();
    }
    else
    {
        if(d->activeTab >= 0)
            setTab(d->activeTab, false);

        d->activeTab = tab;
        setTab(d->activeTab, true);
        d->stack->raiseWidget(d->activeTab);

        if(d->minimized)
            expand();

        emit signalChangedTab(d->stack->visibleWidget());
    }
}

void Sidebar::setActiveTab(QWidget *w)
{
    int tab = d->stack->id(w);
    if(tab < 0)
        return;

    if(d->activeTab >= 0)
        setTab(d->activeTab, false);

    d->activeTab = tab;
    setTab(d->activeTab, true);
    d->stack->raiseWidget(d->activeTab);

    if(d->minimized)
        expand();

    emit signalChangedTab(d->stack->visibleWidget());
}

QWidget* Sidebar::getActiveTab()
{
    return d->stack->visibleWidget();
}

void Sidebar::shrink()
{
    d->minimized = true;
    d->bigSize   = size();
    d->minSize   = minimumWidth();
    d->maxSize   = maximumWidth();

    d->stack->hide();

    KMultiTabBarTab* tab = tabs()->first();
    if (tab)
        setFixedWidth(tab->width());
    else
        setFixedWidth(width());

    emit signalViewChanged();
}

void Sidebar::expand()
{
    d->minimized = false;
    d->stack->show();
    resize(d->bigSize);
    setMinimumWidth(d->minSize);
    setMaximumWidth(d->maxSize);
    emit signalViewChanged();
}

bool Sidebar::isExpanded()
{
    return !d->minimized; 
}

bool Sidebar::eventFilter(QObject *obj, QEvent *ev)
{
    QPtrList<KMultiTabBarTab>* pTabs = tabs();

    for (QPtrListIterator<KMultiTabBarTab> it(*pTabs); it.current(); ++it)
    {
        if ( obj == *it )
        {
            if ( ev->type() == QEvent::DragEnter)
            {
                QDragEnterEvent *e = static_cast<QDragEnterEvent *>(ev);
                enterEvent(e);
                e->accept(true);
                return false;
            }
            else if (ev->type() == QEvent::DragMove)
            {
                if (!d->dragSwitchTimer->isActive())
                {
                    d->dragSwitchTimer->start(800, true);
                    d->dragSwitchId = (*it)->id();
                }
                return false;
            }
            else if (ev->type() == QEvent::DragLeave)
            {
                d->dragSwitchTimer->stop();
                QDragLeaveEvent *e = static_cast<QDragLeaveEvent *>(ev);
                leaveEvent(e);
                return false;
            }
            else if (ev->type() == QEvent::Drop)
            {
                d->dragSwitchTimer->stop();
                QDropEvent *e = static_cast<QDropEvent *>(ev);
                leaveEvent(e);
                return false;
            }
            else
            {
                return false;
            }
        }
    }

    // Else, pass the event on to the parent class
    return KMultiTabBar::eventFilter(obj, ev);
}

void Sidebar::slotDragSwitchTimer()
{
    clicked(d->dragSwitchId);
}

}  // namespace Digikam
