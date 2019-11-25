/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-01-24
 * Description : a progress bar used to display file access
 *               progress or a text in status bar.
 *
 * Copyright (C) 2007-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes.

#include <qlayout.h>
#include <qwidget.h>
#include <qpushbutton.h>

// KDE includes.

#include <ksqueezedtextlabel.h>
#include <kprogress.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kcursor.h>

// Local includes.

#include "statusprogressbar.h"
#include "statusprogressbar.moc"

namespace Digikam
{

class StatusProgressBarPriv
{

public:

    enum WidgetStackEnum
    {
        TextLabel=0,
        ProgressBar
    };

    StatusProgressBarPriv()
    {
        textLabel      = 0;
        progressBar    = 0;
        progressWidget = 0;
        cancelButton   = 0;
    }


    QWidget            *progressWidget;

    QPushButton        *cancelButton;

    KSqueezedTextLabel *textLabel;

    KProgress          *progressBar;
};

StatusProgressBar::StatusProgressBar(QWidget *parent)
                 : QWidgetStack(parent, 0, Qt::WDestructiveClose)
{
    d = new StatusProgressBarPriv;
    setFocusPolicy(QWidget::NoFocus);

    d->textLabel      = new KSqueezedTextLabel(this);
    d->progressWidget = new QWidget(this);
    QHBoxLayout *hBox = new QHBoxLayout(d->progressWidget);
    d->progressBar    = new KProgress(d->progressWidget);
    setProgressTotalSteps(100);
    d->cancelButton = new QPushButton(d->progressWidget);
    d->cancelButton->setFocusPolicy(QWidget::NoFocus);
    d->cancelButton->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) );
    d->cancelButton->setPixmap(SmallIcon("cancel"));

    // Parent widget will probably have the wait cursor set.
    // Set arrow cursor to indicate the button can be clicked
    d->cancelButton->setCursor(KCursor::arrowCursor());

    hBox->addWidget(d->progressBar);
    hBox->addWidget(d->cancelButton);

    addWidget(d->textLabel, StatusProgressBarPriv::TextLabel);
    addWidget(d->progressWidget, StatusProgressBarPriv::ProgressBar);

    connect( d->cancelButton, SIGNAL( clicked() ),
             this, SIGNAL( signalCancelButtonPressed() ) );

    progressBarMode(TextMode);
}

StatusProgressBar::~StatusProgressBar()
{
    delete d;
}

void StatusProgressBar::setText(const QString& text)
{
    d->textLabel->setText(text);
}

void StatusProgressBar::setAlignment(int a)
{
    d->textLabel->setAlignment(a);
}

int StatusProgressBar::progressValue()
{
    return d->progressBar->progress();
}

void StatusProgressBar::setProgressValue(int v)
{
    d->progressBar->setProgress(v);
}

void StatusProgressBar::setProgressTotalSteps(int v)
{
    d->progressBar->setTotalSteps(v);
}

int StatusProgressBar::progressTotalSteps()
{
    return d->progressBar->totalSteps();
}

void StatusProgressBar::setProgressText(const QString& text)
{
    d->progressBar->setFormat( text + QString ("%p%") );
    d->progressBar->update();
}

void StatusProgressBar::progressBarMode(int mode, const QString& text)
{
    if ( mode == TextMode)
    {
        raiseWidget(StatusProgressBarPriv::TextLabel);
        setProgressValue(0);
        setText( text );
    }
    else if ( mode == ProgressBarMode )
    {
        d->cancelButton->hide();
        raiseWidget(StatusProgressBarPriv::ProgressBar);
        setProgressText( text );
    }
    else  // CancelProgressBarMode
    {
        d->cancelButton->show();
        raiseWidget(StatusProgressBarPriv::ProgressBar);
        setProgressText( text );
    }
}

}  // namespace Digikam
