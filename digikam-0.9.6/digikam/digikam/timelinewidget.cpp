/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-12-08
 * Description : a widget to display date and time statistics of pictures
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

// C++ includes.

#include <cmath>

// Qt includes.

#include <qpainter.h>
#include <qpixmap.h>
#include <qpen.h>

// KDE includes.

#include <kcursor.h>
#include <klocale.h>
#include <kglobal.h>
#include <kcalendarsystem.h>
#include <kglobalsettings.h>

// Local includes.

#include "ddebug.h"
#include "themeengine.h"
#include "timelinewidget.h"
#include "timelinewidget.moc"

namespace Digikam
{

class TimeLineWidgetPriv
{

public :

    typedef QPair<int, int> YearRefPair;                         // Year + a reference association (Month or week or day)
    typedef QPair<int, TimeLineWidget::SelectionMode> StatPair;  // Statistic value + selection status.

public:

    TimeLineWidgetPriv()
    {
        validMouseEvent = false;
        selMouseEvent   = false;
        maxCountByDay   = 1;
        maxCountByWeek  = 1;
        maxCountByMonth = 1;
        maxCountByYear  = 1;
        topMargin       = 3;
        bottomMargin    = 20;
        barWidth        = 20;
        startPos        = 96;
        nbItems         = 10;
        timeUnit        = TimeLineWidget::Month;
        scaleMode       = TimeLineWidget::LinScale;
        calendar        = KGlobal::locale()->calendar();
    }

    bool                         validMouseEvent;   // Current mouse enter event is valid to set cursor position or selection.
    bool                         selMouseEvent;     // Current mouse enter event is about to make a selection.

    int                          maxCountByDay;
    int                          maxCountByWeek;
    int                          maxCountByMonth;
    int                          maxCountByYear;
    int                          topMargin; 
    int                          bottomMargin; 
    int                          barWidth; 
    int                          nbItems;
    int                          startPos;

    QDateTime                    refDateTime;       // Reference date-time used to draw histogram from middle of widget.
    QDateTime                    cursorDateTime;    // Current date-time used to draw focus cursor.
    QDateTime                    minDateTime;       // Higher date on histogram.
    QDateTime                    maxDateTime;       // Lower date on histogram.
    QDateTime                    selStartDateTime;
    QDateTime                    selMinDateTime;    // Lower date available on histogram.
    QDateTime                    selMaxDateTime;    // Higher date available on histogram.

    QPixmap                      pixmap;            // Used for widget double buffering.

    QMap<YearRefPair, StatPair>  dayStatMap;        // Store Days count statistics.
    QMap<YearRefPair, StatPair>  weekStatMap;       // Store Weeks count statistics.
    QMap<YearRefPair, StatPair>  monthStatMap;      // Store Month count statistics.
    QMap<int,         StatPair>  yearStatMap;       // Store Years count statistics.

    const KCalendarSystem       *calendar;

    TimeLineWidget::TimeUnit     timeUnit;
    TimeLineWidget::ScaleMode    scaleMode;
};

TimeLineWidget::TimeLineWidget(QWidget *parent)
              : QWidget(parent, 0, Qt::WDestructiveClose)
{
    d = new TimeLineWidgetPriv;
    setBackgroundMode(Qt::NoBackground);
    setMouseTracking(true);
    setMinimumWidth(256);
    setMinimumHeight(192);

    QDateTime ref = QDateTime::currentDateTime();   
    setCursorDateTime(ref);   
    setRefDateTime(ref);

    connect(ThemeEngine::instance(), SIGNAL(signalThemeChanged()),
            this, SLOT(slotThemeChanged()));
}

TimeLineWidget::~TimeLineWidget()
{
    delete d;
}

void TimeLineWidget::setTimeUnit(TimeUnit timeUnit)
{
    d->timeUnit = timeUnit;
    setCursorDateTime(cursorDateTime());
    setRefDateTime(cursorDateTime());
}

TimeLineWidget::TimeUnit TimeLineWidget::timeUnit() const
{
    return d->timeUnit;
}

void TimeLineWidget::setScaleMode(ScaleMode scaleMode)
{
    d->scaleMode = scaleMode;
    updatePixmap();
    update();
}

TimeLineWidget::ScaleMode TimeLineWidget::scaleMode() const
{
    return d->scaleMode;
}

int TimeLineWidget::totalIndex()
{
    if (d->minDateTime.isNull() || d->maxDateTime.isNull())
        return 0;

    int        i = 0;
    QDateTime dt = d->minDateTime;

    do
    {
        dt = nextDateTime(dt);
        i++;
    }
    while(dt < d->maxDateTime);

    return i;
}

int TimeLineWidget::indexForDateTime(const QDateTime& date)
{
    if (d->minDateTime.isNull() || d->maxDateTime.isNull() || date.isNull())
        return 0;

    int        i = 0;
    QDateTime dt = d->minDateTime;

    do
    {
        dt = nextDateTime(dt);
        i++;
    }
    while(dt < date);

    return i;
}

int TimeLineWidget::indexForRefDateTime()
{
    return (indexForDateTime(d->refDateTime));
}

int TimeLineWidget::indexForCursorDateTime()
{
    return (indexForDateTime(d->cursorDateTime));
}

void TimeLineWidget::setCurrentIndex(int index)
{
    if (d->minDateTime.isNull() || d->maxDateTime.isNull())
        return;

    int        i = 0;
    QDateTime dt = d->minDateTime;

    do
    {
        dt = nextDateTime(dt);
        i++;
    }
    while(i <= index);

    setRefDateTime(dt);
}

void TimeLineWidget::setCursorDateTime(const QDateTime& dateTime)
{
    QDateTime dt = dateTime;
    dt.setTime(QTime(0, 0, 0, 0));

    switch(d->timeUnit)
    {
        case Week:
        {
            // Go to the first day of week.
            int weekYear = 0;
            int weekNb   = d->calendar->weekNumber(dt.date(), &weekYear);
            dt           = firstDayOfWeek(weekYear, weekNb);
            break;
        }
        case Month:
        {
            // Go to the first day of month.
            dt.setDate(QDate(dt.date().year(), dt.date().month(), 1));
            break;
        }
        case Year:
        {
            // Go to the first day of year.
            dt.setDate(QDate(dt.date().year(), 1, 1));
            break;
        }
        default:
            break;
    }

    if (d->cursorDateTime == dt)
        return;

    d->cursorDateTime = dt;

    emit signalCursorPositionChanged();
}

QDateTime TimeLineWidget::cursorDateTime() const
{
    return d->cursorDateTime;
}

int TimeLineWidget::cursorInfo(QString& infoDate)
{
    SelectionMode selected;
    QDateTime dt = cursorDateTime();

    switch(d->timeUnit)
    {
        case Day:
        {
            infoDate = KGlobal::locale()->formatDate(dt.date());
            break;
        }
        case Week:
        {
            infoDate = i18n("Week #%1 - %2 %3")
                       .arg(d->calendar->weekNumber(dt.date()))
                       .arg(d->calendar->monthName(dt.date()))
                       .arg(d->calendar->yearString(dt.date(), false));
            break;
        }
        case Month:
        {
            infoDate = QString("%1 %2")
                       .arg(d->calendar->monthName(dt.date()))
                       .arg(d->calendar->yearString(dt.date(), false));
            break;
        }
        case Year:
        {
            infoDate = d->calendar->yearString(dt.date(), false);
            break;
        }
    }

    return statForDateTime(dt, selected);
}

void TimeLineWidget::setRefDateTime(const QDateTime& dateTime)
{
    QDateTime dt = dateTime;
    dt.setTime(QTime(0, 0, 0, 0));

    switch(d->timeUnit)
    {
        case Week:
        {
            // Go to the first day of week.
            int dayWeekOffset = (-1) * (d->calendar->dayOfWeek(dt.date()) - 1);
            dt = dt.addDays(dayWeekOffset);
            break;
        }
        case Month:
        {
            // Go to the first day of month.
            dt.setDate(QDate(dt.date().year(), dt.date().month(), 1));
            break;
        }
        case Year:
        {
            // Go to the first day of year.
            dt.setDate(QDate(dt.date().year(), 1, 1));
            break;
        }
        default:
            break;
    }

    d->refDateTime = dt;
    updatePixmap();
    update();
    emit signalRefDateTimeChanged();
}

void TimeLineWidget::slotResetSelection()
{
    resetSelection();
    updatePixmap();
    update();
}

void TimeLineWidget::resetSelection()
{
    QMap<TimeLineWidgetPriv::YearRefPair, TimeLineWidgetPriv::StatPair>::iterator it;

    for (it = d->dayStatMap.begin() ; it != d->dayStatMap.end(); ++it)
        it.data().second = Unselected;

    for (it = d->weekStatMap.begin() ; it != d->weekStatMap.end(); ++it)
        it.data().second = Unselected;

    for (it = d->monthStatMap.begin() ; it != d->monthStatMap.end(); ++it)
        it.data().second = Unselected;

    QMap<int, TimeLineWidgetPriv::StatPair>::iterator it2;

    for (it2 = d->yearStatMap.begin() ; it2 != d->yearStatMap.end(); ++it2)
        it2.data().second = Unselected;
}

void TimeLineWidget::setSelectedDateRange(const DateRangeList& list)
{
    if (list.isEmpty())
        return;

    resetSelection();

    QDateTime start, end, dt;
    DateRangeList::const_iterator it;

    for (it = list.begin() ; it != list.end(); ++it)
    {
        start = (*it).first;
        end   = (*it).second;
        if (end > start)
        {
            dt = start;
            do
            {
                setDateTimeSelected(dt, Selected);
                dt = dt.addDays(1);
            }
            while (dt < end);
        }
    }

    updatePixmap();
    update();
}

DateRangeList TimeLineWidget::selectedDateRange(int& totalCount)
{
    // We will parse all selections done on Days stats map.

    DateRangeList list;
    totalCount = 0;
    QMap<TimeLineWidgetPriv::YearRefPair, TimeLineWidgetPriv::StatPair>::iterator it3;
    QDateTime sdt, edt;
    QDate     date;

    for (it3 = d->dayStatMap.begin() ; it3 != d->dayStatMap.end(); ++it3)
    {
        if (it3.data().second == Selected)
        {
            date = QDate(it3.key().first, 1, 1);
            date = date.addDays(it3.key().second-1);
            sdt  = QDateTime(date);
            edt  = sdt.addDays(1); 
            list.append(DateRange(sdt, edt));
            totalCount += it3.data().first;
        }
    }

    DateRangeList::iterator it, it2;

    /*
    for (it = list.begin() ; it != list.end(); ++it)
        DDebug() << (*it).first.date().toString(Qt::ISODate) << " :: " 
                 << (*it).second.date().toString(Qt::ISODate) << endl;

    DDebug() << "Total Count of Items = " << totalCount << endl;
    */

    // Group contiguous date ranges to optimize query on database.

    DateRangeList list2;
    QDateTime     first, second, first2, second2;

    for (it = list.begin() ; it != list.end(); ++it)
    { 
        first  = (*it).first;
        second = (*it).second;
        it2 = it;
        do
        {
            ++it2;
            if (it2 != list.end())
            {
                first2  = (*it2).first;
                second2 = (*it2).second;
                
                if (first2 == second)
                {
                    second = second2;
                    ++it;
                }
                else 
                    break;
            }
        }
        while(it2 != list.end());

        list2.append(DateRange(first, second));
    }

    /*
    for (it = list2.begin() ; it != list2.end(); ++it)
        DDebug() << (*it).first.date().toString(Qt::ISODate) << " :: " 
                 << (*it).second.date().toString(Qt::ISODate) << endl;
    */

    return list2;
}

void TimeLineWidget::slotDatesMap(const QMap<QDateTime, int>& datesStatMap)
{
    // Clear all counts in all stats maps before to update it. Do not clear selections.

    QMap<int, TimeLineWidgetPriv::StatPair>::iterator it_iP;
    for ( it_iP = d->yearStatMap.begin() ; it_iP != d->yearStatMap.end(); ++it_iP )
        it_iP.data().first = 0;

    QMap<TimeLineWidgetPriv::YearRefPair, TimeLineWidgetPriv::StatPair>::iterator it_YP;
    for ( it_YP = d->monthStatMap.begin() ; it_YP != d->monthStatMap.end(); ++it_YP )
        it_YP.data().first = 0;

    for ( it_YP = d->weekStatMap.begin() ; it_YP != d->weekStatMap.end(); ++it_YP )
        it_YP.data().first = 0;

    for ( it_YP = d->dayStatMap.begin() ; it_YP != d->dayStatMap.end(); ++it_YP )
        it_YP.data().first = 0;

    // Parse all new Date stamp and store histogram stats relevant in maps.

    int count;
    QMap<QDateTime, int>::const_iterator it;
    if (datesStatMap.isEmpty())
    {
        d->minDateTime = QDateTime();
        d->maxDateTime = QDateTime();
    }
    else
    {
        d->minDateTime = datesStatMap.begin().key();
        d->maxDateTime = datesStatMap.begin().key();
    }

    for ( it = datesStatMap.begin(); it != datesStatMap.end(); ++it )
    {
        if (it.key() > d->maxDateTime)
            d->maxDateTime = it.key();

        if (it.key() < d->minDateTime)
            d->minDateTime = it.key();

        int year        = it.key().date().year();
        int month       = it.key().date().month();
        int day         = d->calendar->dayOfYear(it.key().date());
        int yearForWeek = year;  // Used with week shared between 2 years decade (Dec/Jan).
        int week        = d->calendar->weekNumber(it.key().date(), &yearForWeek);

        // Stats Years values.

        it_iP = d->yearStatMap.find(year);
        if ( it_iP == d->yearStatMap.end() )
        {
            count = it.data();
            d->yearStatMap.insert( year, TimeLineWidgetPriv::StatPair(count, Unselected) );
        }
        else
        {
            count = it_iP.data().first + it.data();
            d->yearStatMap.replace( year, TimeLineWidgetPriv::StatPair(count, it_iP.data().second) );
        }

        if (d->maxCountByYear < count) 
            d->maxCountByYear = count;

        // Stats Months values.

        it_YP = d->monthStatMap.find(TimeLineWidgetPriv::YearRefPair(year, month));
        if ( it_YP == d->monthStatMap.end() )
        {
            count = it.data();
            d->monthStatMap.insert( TimeLineWidgetPriv::YearRefPair(year, month), 
                                    TimeLineWidgetPriv::StatPair(count, Unselected) );
        }
        else
        {
            count = it_YP.data().first + it.data();
            d->monthStatMap.replace( TimeLineWidgetPriv::YearRefPair(year, month), 
                                     TimeLineWidgetPriv::StatPair(count, it_YP.data().second) );
        }

        if (d->maxCountByMonth < count) 
            d->maxCountByMonth = count;

        // Stats Weeks values.

        it_YP = d->weekStatMap.find(TimeLineWidgetPriv::YearRefPair(yearForWeek, week));
        if ( it_YP == d->weekStatMap.end() )
        {
            count = it.data();
            d->weekStatMap.insert( TimeLineWidgetPriv::YearRefPair(yearForWeek, week), 
                                   TimeLineWidgetPriv::StatPair(count, Unselected) );
        }
        else
        {
            count = it_YP.data().first + it.data();
            d->weekStatMap.replace( TimeLineWidgetPriv::YearRefPair(yearForWeek, week), 
                                    TimeLineWidgetPriv::StatPair(count, it_YP.data().second) );
        }

        if (d->maxCountByWeek < count) 
            d->maxCountByWeek = count;

        // Stats Days values.

        it_YP = d->dayStatMap.find(TimeLineWidgetPriv::YearRefPair(year, day));
        if ( it_YP == d->dayStatMap.end() )
        {
            count = it.data();
            d->dayStatMap.insert( TimeLineWidgetPriv::YearRefPair(year, day), 
                                  TimeLineWidgetPriv::StatPair(count, Unselected) );
        }
        else
        {
            count = it_YP.data().first + it.data();
            d->dayStatMap.replace( TimeLineWidgetPriv::YearRefPair(year, day), 
                                   TimeLineWidgetPriv::StatPair(count, it_YP.data().second) );
        }

        if (d->maxCountByDay < count) 
            d->maxCountByDay = count;
    }

    if (!datesStatMap.isEmpty())
    {
        d->maxDateTime.setTime(QTime(0, 0, 0, 0));
        d->minDateTime.setTime(QTime(0, 0, 0, 0));
    }
    else
    {
        d->maxDateTime = d->refDateTime;
        d->minDateTime = d->refDateTime;
    }

    updatePixmap();
    update();
    emit signalDateMapChanged();
}

void TimeLineWidget::updatePixmap()
{
    // Drawing background and image.
    d->pixmap = QPixmap(size());
    d->pixmap.fill(palette().active().background());

    QPainter p(&d->pixmap);

    d->bottomMargin = (int)(p.fontMetrics().height()*1.5); 
    d->barWidth     = p.fontMetrics().width("00"); 
    d->nbItems      = (int)((width() / 2.0) / (float)d->barWidth);
    d->startPos     = (int)((width() / 2.0) - ((float)(d->barWidth) / 2.0));
    int dim         = height() - d->bottomMargin - d->topMargin;
    QDateTime ref   = d->refDateTime;
    ref.setTime(QTime(0, 0, 0, 0));
    double        max, logVal;
    int           val, top;
    SelectionMode sel;
    QRect         focusRect, selRect, barRect;
    QBrush        selBrush;
    QColor        dateColor, subDateColor;

    // Date histogram drawing is divided in 2 parts. The current date-time 
    // is placed on the center of the view and all dates on right are computed,
    // and in second time, all dates on the left.

    // Draw all dates on the right of ref. date-time.

    for (int i = 0 ; i < d->nbItems ; i++)
    {
        val = statForDateTime(ref, sel);
        max = (double)maxCount();

        if (d->scaleMode == TimeLineWidget::LogScale)
        {
            if (max > 0.0) max = log(max);
            else           max = 1.0;

            if (val <= 0) logVal = 0;
            else          logVal = log(val);

            top = lround(dim + d->topMargin - ((logVal * dim) / max));

            if (top < 0) val = 0;
        }
        else
        {
            top = lround(dim + d->topMargin - ((val * dim) / max));
        }

        barRect.setTop(top);
        barRect.setLeft(d->startPos + i*d->barWidth);
        barRect.setBottom(height() - d->bottomMargin);
        barRect.setRight(d->startPos + (i+1)*d->barWidth);

        if (ref == d->cursorDateTime)
            focusRect = barRect;

        if (ref > d->maxDateTime)
            dateColor = palette().active().mid();
        else 
            dateColor = palette().active().foreground(); 

        p.setPen(palette().active().foreground());
        p.fillRect(barRect, QBrush(ThemeEngine::instance()->textSpecialRegColor()));
        p.drawRect(barRect);
        p.drawLine(barRect.right(), barRect.bottom(), barRect.right(), barRect.bottom()+3);
        p.drawLine(barRect.left(),  barRect.bottom(), barRect.left(),  barRect.bottom()+3);

        if (val) 
        {
            if (sel)
                subDateColor = palette().active().highlightedText();
            else
                subDateColor = palette().active().foreground();
        }
        else 
            subDateColor = palette().active().mid();

        if (sel == Selected || sel == FuzzySelection)
        {
            selBrush.setColor(ThemeEngine::instance()->thumbSelColor());
            selBrush.setStyle(QBrush::SolidPattern);
            if (sel == FuzzySelection)
                selBrush.setStyle(QBrush::Dense4Pattern);

            selRect.setTop(height() - d->bottomMargin + 1);
            selRect.setLeft(d->startPos + i*d->barWidth);
            selRect.setBottom(height() - d->bottomMargin/2);
            selRect.setRight(d->startPos + (i+1)*d->barWidth);
            p.fillRect(selRect, selBrush);
        }

        switch(d->timeUnit)
        {
            case Day:
            {
                {
                    p.save();
                    QFont fnt = p.font();
                    fnt.setPointSize(fnt.pointSize()-4);
                    p.setFont(fnt);
                    p.setPen(subDateColor);
                    QString txt = QString(d->calendar->weekDayName(ref.date(), true)[0]);
                    QRect br    = p.fontMetrics().boundingRect(0, 0, width(), height(), 0, txt); 
                    p.drawText(barRect.left() + ((barRect.width()-br.width())/2),
                               barRect.bottom()+br.height(), txt);
                    p.restore();
                }

                if (d->calendar->dayOfWeek(ref.date()) == 1)
                {
                    p.setPen(dateColor);
                    p.drawLine(barRect.left(), barRect.bottom(), 
                               barRect.left(), barRect.bottom()+d->bottomMargin/2);
                    QString txt = KGlobal::locale()->formatDate(ref.date(), true);
                    QRect br    = p.fontMetrics().boundingRect(0, 0, width(), height(), 0, txt); 
                    p.drawText(barRect.left()-br.width()/2, barRect.bottom() + d->bottomMargin, txt);
                }
                break;
            }
            case Week:
            {
                int week = d->calendar->weekNumber(ref.date());
                {
                    p.save();
                    QFont fnt = p.font();
                    fnt.setPointSize(fnt.pointSize()-4);
                    p.setFont(fnt);
                    p.setPen(subDateColor);
                    QString txt = QString::number(week);
                    QRect br    = p.fontMetrics().boundingRect(0, 0, width(), height(), 0, txt); 
                    p.drawText(barRect.left() + ((barRect.width()-br.width())/2),
                               barRect.bottom()+br.height(), txt);
                    p.restore();
                }

                p.setPen(dateColor);
                if (week == 1 || week == 10 || week == 20 || week == 30 || week == 40 || week == 50)
                {
                    p.drawLine(barRect.left(), barRect.bottom(), 
                               barRect.left(), barRect.bottom()+d->bottomMargin/2);
                    QString txt = KGlobal::locale()->formatDate(ref.date(), true);
                    QRect br    = p.fontMetrics().boundingRect(0, 0, width(), height(), 0, txt); 
                    if (week != 50)
                        p.drawText(barRect.left()-br.width()/2, barRect.bottom() + d->bottomMargin, txt);
                }
                else if (week == 6 || week == 16 || week == 26 || week == 36 || week == 46)
                {
                    p.drawLine(barRect.left(), barRect.bottom(), 
                               barRect.left(), barRect.bottom()+d->bottomMargin/4);
                }
                break;
            }
            case Month:
            {
                {
                    p.save();
                    QFont fnt = p.font();
                    fnt.setPointSize(fnt.pointSize()-4);
                    p.setFont(fnt);
                    p.setPen(subDateColor) ;
                    QString txt = QString(d->calendar->monthName(ref.date(), true)[0]);
                    QRect br    = p.fontMetrics().boundingRect(0, 0, width(), height(), 0, txt); 
                    p.drawText(barRect.left() + ((barRect.width()-br.width())/2),
                               barRect.bottom()+br.height(), txt);
                    p.restore();
                }

                p.setPen(dateColor);
                if (ref.date().month() == 1)
                {
                    p.drawLine(barRect.left(), barRect.bottom(), 
                               barRect.left(), barRect.bottom()+d->bottomMargin/2);
                    QString txt = QString::number(ref.date().year());
                    QRect br    = p.fontMetrics().boundingRect(0, 0, width(), height(), 0, txt); 
                    p.drawText(barRect.left()-br.width()/2, barRect.bottom() + d->bottomMargin, txt);
                }
                else if (ref.date().month() == 7)
                {
                    p.drawLine(barRect.left(), barRect.bottom(), 
                               barRect.left(), barRect.bottom()+d->bottomMargin/4);
                }
                break;
            }
            case Year:
            {
                p.setPen(dateColor);
                if (ref.date().year() % 10 == 0)
                {
                    p.drawLine(barRect.left(), barRect.bottom(), 
                               barRect.left(), barRect.bottom()+d->bottomMargin/2);
                    QString txt = QString::number(ref.date().year());
                    QRect br    = p.fontMetrics().boundingRect(0, 0, width(), height(), 0, txt); 
                    p.drawText(barRect.left()-br.width()/2, barRect.bottom() + d->bottomMargin, txt);
                }
                else if (ref.date().year() % 5 == 0)
                    p.drawLine(barRect.left(), barRect.bottom(), 
                               barRect.left(), barRect.bottom()+d->bottomMargin/4);
                break;
            }
        }

        ref = nextDateTime(ref);
    }

    // Draw all dates on the left of ref. date-time.

    ref = d->refDateTime;
    ref.setTime(QTime(0, 0, 0, 0));
    ref = prevDateTime(ref);

    for (int i = 0 ; i < d->nbItems-1 ; i++)
    {
        val = statForDateTime(ref, sel);
        max = (double)maxCount();

        if (d->scaleMode == TimeLineWidget::LogScale)
        {
            if (max > 0.0) max = log(max);
            else           max = 1.0;

            if (val <= 0) logVal = 0;
            else          logVal = log(val);

            top = lround(dim + d->topMargin - ((logVal * dim) / max));

            if (top < 0) val = 0;
        }
        else
        {
            top = lround(dim + d->topMargin - ((val * dim) / max));
        }

        barRect.setTop(top);
        barRect.setRight(d->startPos - i*d->barWidth);
        barRect.setBottom(height() - d->bottomMargin);
        barRect.setLeft(d->startPos - (i+1)*d->barWidth);

        if (ref == d->cursorDateTime)
            focusRect = barRect;

        if (ref < d->minDateTime)
            dateColor = palette().active().mid();
        else 
            dateColor = palette().active().foreground(); 

        p.setPen(palette().active().foreground());
        p.fillRect(barRect, QBrush(ThemeEngine::instance()->textSpecialRegColor()));
        p.drawRect(barRect);
        p.drawLine(barRect.right(), barRect.bottom(), barRect.right(), barRect.bottom()+3);
        p.drawLine(barRect.left(),  barRect.bottom(), barRect.left(),  barRect.bottom()+3);

        if (val) 
        {
            if (sel)
                subDateColor = palette().active().highlightedText();
            else
                subDateColor = palette().active().foreground();
        }
        else 
            subDateColor = palette().active().mid();

        if (sel == Selected || sel == FuzzySelection)
        {
            selBrush.setColor(ThemeEngine::instance()->thumbSelColor());
            selBrush.setStyle(QBrush::SolidPattern);
            if (sel == FuzzySelection)
                selBrush.setStyle(QBrush::Dense4Pattern);

            selRect.setTop(height() - d->bottomMargin + 1);
            selRect.setLeft(d->startPos - (i+1)*d->barWidth);
            selRect.setBottom(height() - d->bottomMargin/2);
            selRect.setRight(d->startPos - i*d->barWidth);
            p.fillRect(selRect, selBrush);
        }

        switch(d->timeUnit)
        {
            case Day:
            {
                {
                    p.save();
                    QFont fnt = p.font();
                    fnt.setPointSize(fnt.pointSize()-4);
                    p.setFont(fnt);
                    p.setPen(subDateColor) ;
                    QString txt = QString(d->calendar->weekDayName(ref.date(), true)[0]);
                    QRect br    = p.fontMetrics().boundingRect(0, 0, width(), height(), 0, txt); 
                    p.drawText(barRect.left() + ((barRect.width()-br.width())/2),
                               barRect.bottom()+br.height(), txt);
                    p.restore();
                }

                if (d->calendar->dayOfWeek(ref.date()) == 1)
                {
                    p.setPen(dateColor);
                    p.drawLine(barRect.left(), barRect.bottom(),
                               barRect.left(), barRect.bottom()+d->bottomMargin/2);
                    QString txt = KGlobal::locale()->formatDate(ref.date(), true);
                    QRect br    = p.fontMetrics().boundingRect(0, 0, width(), height(), 0, txt); 
                    p.drawText(barRect.left()-br.width()/2, barRect.bottom() + d->bottomMargin, txt);
                }
                break;
            }
            case Week:
            {
                int week = d->calendar->weekNumber(ref.date());
                {
                    p.save();
                    QFont fnt = p.font();
                    fnt.setPointSize(fnt.pointSize()-4);
                    p.setFont(fnt);
                    p.setPen(subDateColor) ;
                    QString txt = QString::number(week);
                    QRect br    = p.fontMetrics().boundingRect(0, 0, width(), height(), 0, txt); 
                    p.drawText(barRect.left() + ((barRect.width()-br.width())/2),
                               barRect.bottom()+br.height(), txt);
                    p.restore();
                }

                p.setPen(dateColor);
                if (week == 1 || week == 10 || week == 20 || week == 30 || week == 40 || week == 50)
                {
                    p.drawLine(barRect.left(), barRect.bottom(), 
                               barRect.left(), barRect.bottom()+d->bottomMargin/2);
                    QString txt = KGlobal::locale()->formatDate(ref.date(), true);
                    QRect br    = p.fontMetrics().boundingRect(0, 0, width(), height(), 0, txt); 
                    if (week != 50)
                        p.drawText(barRect.left()-br.width()/2, barRect.bottom() + d->bottomMargin, txt);
                }
                else if (week == 6 || week == 16 || week == 26 || week == 36 || week == 46)
                {
                    p.drawLine(barRect.left(), barRect.bottom(), 
                               barRect.left(), barRect.bottom()+d->bottomMargin/4);
                }
                break;
            }
            case Month:
            {
                {
                    p.save();
                    QFont fnt = p.font();
                    fnt.setPointSize(fnt.pointSize()-4);
                    p.setFont(fnt);
                    p.setPen(subDateColor) ;
                    QString txt = QString(d->calendar->monthName(ref.date(), true)[0]);
                    QRect br    = p.fontMetrics().boundingRect(0, 0, width(), height(), 0, txt); 
                    p.drawText(barRect.left() + ((barRect.width()-br.width())/2),
                               barRect.bottom()+br.height(), txt);
                    p.restore();
                }

                p.setPen(dateColor);
                if (ref.date().month() == 1)
                {
                    p.drawLine(barRect.left(), barRect.bottom(), 
                               barRect.left(), barRect.bottom()+d->bottomMargin/2);
                    QString txt = QString::number(ref.date().year());
                    QRect br    = p.fontMetrics().boundingRect(0, 0, width(), height(), 0, txt); 
                    p.drawText(barRect.left()-br.width()/2, barRect.bottom() + d->bottomMargin, txt);
                }
                else if (ref.date().month() == 7)
                {
                    p.drawLine(barRect.right(), barRect.bottom(), 
                               barRect.right(), barRect.bottom()+d->bottomMargin/4);
                }
                break;
            }
            case Year:
            {
                p.setPen(dateColor);
                if (ref.date().year() % 10 == 0)
                {
                    p.drawLine(barRect.left(), barRect.bottom(), 
                               barRect.left(), barRect.bottom()+d->bottomMargin/2);
                    QString txt = QString::number(ref.date().year());
                    QRect br    = p.fontMetrics().boundingRect(0, 0, width(), height(), 0, txt); 
                    p.drawText(barRect.left()-br.width()/2, barRect.bottom() + d->bottomMargin, txt);
                }
                else if (ref.date().year() % 5 == 0)
                    p.drawLine(barRect.right(), barRect.bottom(), 
                               barRect.right(), barRect.bottom()+d->bottomMargin/4);
                break;
            }
        }

        ref = prevDateTime(ref);
    }

    // Draw cursor rectangle over current date-time.
    if (focusRect.isValid())
    {
        focusRect.setTop(d->topMargin);
        QPoint p1(focusRect.left(), height() - d->bottomMargin);
        QPoint p2(focusRect.right(), height() - d->bottomMargin);
        focusRect.setBottom(focusRect.bottom() + d->bottomMargin/2);

        p.setPen(palette().active().shadow());
        p.drawLine(p1.x(), p1.y()+1, p2.x(), p2.y()+1);
        p.drawRect(focusRect);

        focusRect.addCoords(-1,-1, 1, 1);
        p.setPen(ThemeEngine::instance()->textSelColor());
        p.drawRect(focusRect);
        p.drawLine(p1.x()-1, p1.y(), p2.x()+1, p2.y());

        focusRect.addCoords(-1,-1, 1, 1);
        p.drawRect(focusRect);
        p.drawLine(p1.x()-1, p1.y()-1, p2.x()+1, p2.y()-1);

        focusRect.addCoords(-1,-1, 1, 1);
        p.setPen(palette().active().shadow());
        p.drawRect(focusRect);
        p.drawLine(p1.x(), p1.y()-2, p2.x(), p2.y()-2);
    }
    p.end();
}

QDateTime TimeLineWidget::prevDateTime(const QDateTime& dt)
{
    QDateTime prev;
    switch(d->timeUnit)
    {
        case Day:
        {
            prev = dt.addDays(-1);
            break;
        }
        case Week:
        {
            prev = dt.addDays(-7);
            break;
        }
        case Month:
        {
            prev = dt.addMonths(-1);
            break;
        }
        case Year:
        {
            prev = dt.addYears(-1);
            break;
        }
    }
    return prev;
}

QDateTime TimeLineWidget::nextDateTime(const QDateTime& dt)
{
    QDateTime next;
    switch(d->timeUnit)
    {
        case Day:
        {
            next = dt.addDays(1);
            break;
        }
        case Week:
        {
            next = dt.addDays(7);
            break;
        }
        case Month:
        {
            next = dt.addMonths(1);
            break;
        }
        case Year:
        {
            next = dt.addYears(1);
            break;
        }
    }
    return next;
}

int TimeLineWidget::maxCount()
{
    int max = 1;
    switch(d->timeUnit)
    {
        case Day:
        {
            max = d->maxCountByDay;
            break;
        }
        case Week:
        {
            max = d->maxCountByWeek;
            break;
        }
        case Month:
        {
            max = d->maxCountByMonth;
            break;
        }
        case Year:
        {
            max = d->maxCountByYear;
            break;
        }
    }
    return max;
}

int TimeLineWidget::statForDateTime(const QDateTime& dt, SelectionMode& selected)
{
    int count = 0;
    int year  = dt.date().year();
    int month = dt.date().month();
    int day   = d->calendar->dayOfYear(dt.date());
    int yearForWeek;  // Used with week shared between 2 years decade (Dec/Jan).
    int week  = d->calendar->weekNumber(dt.date(), &yearForWeek);
    selected  = Unselected;

    switch(d->timeUnit)
    {
        case Day:
        {
            QMap<TimeLineWidgetPriv::YearRefPair, TimeLineWidgetPriv::StatPair>::iterator it =
                d->dayStatMap.find(TimeLineWidgetPriv::YearRefPair(year, day));
            if ( it != d->dayStatMap.end() )
            {
                count    = it.data().first;
                selected = it.data().second;
            }
            break;
        }
        case Week:
        {
            QMap<TimeLineWidgetPriv::YearRefPair, TimeLineWidgetPriv::StatPair>::iterator it =
                d->weekStatMap.find(TimeLineWidgetPriv::YearRefPair(yearForWeek, week));
            if ( it != d->weekStatMap.end() )
            {
                count    = it.data().first;
                selected = it.data().second;
            }
            break;
        }
        case Month:
        {
            QMap<TimeLineWidgetPriv::YearRefPair, TimeLineWidgetPriv::StatPair>::iterator it =
                d->monthStatMap.find(TimeLineWidgetPriv::YearRefPair(year, month));
            if ( it != d->monthStatMap.end() )
            {
                count    = it.data().first;
                selected = it.data().second;
            }
            break;
        }
        case Year:
        {
            QMap<int, TimeLineWidgetPriv::StatPair>::iterator it = d->yearStatMap.find(year);
            if ( it != d->yearStatMap.end() )
            {
                count    = it.data().first;
                selected = it.data().second;
            }
            break;
        }
    }

    return count;
}

void TimeLineWidget::setDateTimeSelected(const QDateTime& dt, SelectionMode selected)
{
    int year  = dt.date().year();
    int month = dt.date().month();
    int yearForWeek;  // Used with week shared between 2 years decade (Dec/Jan).
    int week  = d->calendar->weekNumber(dt.date(), &yearForWeek);

    QDateTime dts, dte;

    switch(d->timeUnit)
    {
        case Day:
        {
            dts = dt;
            dte = dts.addDays(1);
            setDaysRangeSelection(dts, dte, selected);
            break;
        }
        case Week:
        {
            dts = firstDayOfWeek(yearForWeek, week);
            dte = dts.addDays(7);
            setDaysRangeSelection(dts, dte, selected);
            updateWeekSelection(dts, dte);
            break;
        }
        case Month:
        {
            dts = QDateTime(QDate(year, month, 1));
            dte = dts.addDays(d->calendar->daysInMonth(dts.date()));
            setDaysRangeSelection(dts, dte, selected);
            updateMonthSelection(dts, dte);
            break;
        }
        case Year:
        {
            dts = QDateTime(QDate(year, 1, 1));
            dte = dts.addDays(d->calendar->daysInYear(dts.date()));
            setDaysRangeSelection(dts, dte, selected);
            updateYearSelection(dts, dte);
            break;
        }
    }
}

void TimeLineWidget::updateWeekSelection(const QDateTime dts, const QDateTime dte)
{
    QMap<TimeLineWidgetPriv::YearRefPair, TimeLineWidgetPriv::StatPair>::iterator it;
    QDateTime dtsWeek, dteWeek, dt;
    int week;
    int yearForWeek;  // Used with week shared between 2 years decade (Dec/Jan).
    dt = dts;
    do
    {
        yearForWeek = dt.date().year();
        week = d->calendar->weekNumber(dt.date(), &yearForWeek);

        dtsWeek = firstDayOfWeek(yearForWeek, week);
        dteWeek = dtsWeek.addDays(7);
        it  = d->weekStatMap.find(TimeLineWidgetPriv::YearRefPair(yearForWeek, week));
        if ( it != d->weekStatMap.end() )
            it.data().second = checkSelectionForDaysRange(dtsWeek, dteWeek);

        dt = dt.addDays(7);
    }
    while (dt <= dte);
}

void TimeLineWidget::updateMonthSelection(const QDateTime dts, const QDateTime dte)
{
    QMap<TimeLineWidgetPriv::YearRefPair, TimeLineWidgetPriv::StatPair>::iterator it;
    QDateTime dtsMonth, dteMonth, dt;
    int       year, month;
    dt = dts;
    do
    {
        year  = dt.date().year();
        month = dt.date().month();

        dtsMonth = QDateTime(QDate(year, month, 1));
        dteMonth = dtsMonth.addDays(d->calendar->daysInMonth(dtsMonth.date()));
        it  = d->monthStatMap.find(TimeLineWidgetPriv::YearRefPair(year, month));
        if ( it != d->monthStatMap.end() )
            it.data().second = checkSelectionForDaysRange(dtsMonth, dteMonth);

        dt = dteMonth;
    }
    while (dt <= dte);
}

void TimeLineWidget::updateYearSelection(const QDateTime dts, const QDateTime dte)
{
    QMap<int, TimeLineWidgetPriv::StatPair>::iterator it;
    QDateTime dtsYear, dteYear, dt;
    int       year;
    dt = dts;
    do
    {
        year = dt.date().year();

        dtsYear = QDateTime(QDate(year, 1, 1));
        dteYear = dtsYear.addDays(d->calendar->daysInYear(dtsYear.date()));
        it = d->yearStatMap.find(year);
        if ( it != d->yearStatMap.end() )
            it.data().second = checkSelectionForDaysRange(dtsYear, dteYear);

        dt = dteYear;
    }
    while (dt <= dte);
}

void TimeLineWidget::updateAllSelection()
{
    QMap<TimeLineWidgetPriv::YearRefPair, TimeLineWidgetPriv::StatPair>::iterator it;
    QDateTime dts, dte;
    QDate     date;

    for (it = d->dayStatMap.begin() ; it != d->dayStatMap.end(); ++it)
    {
        if (it.data().second == Selected)
        {
            date = QDate(it.key().first, 1, 1);
            date = date.addDays(it.key().second-1);
            dts  = QDateTime(date);
            dte  = dts.addDays(1); 
            updateWeekSelection(dts, dte);
            updateMonthSelection(dts, dte);
            updateYearSelection(dts, dte);
        }
    }
}

void TimeLineWidget::setDaysRangeSelection(const QDateTime dts, const QDateTime dte, SelectionMode selected)
{
    int year, day;
    QDateTime dt = dts;
    QMap<TimeLineWidgetPriv::YearRefPair, TimeLineWidgetPriv::StatPair>::iterator it;

    do
    {
        year = dt.date().year();
        day  = d->calendar->dayOfYear(dt.date());
        it = d->dayStatMap.find(TimeLineWidgetPriv::YearRefPair(year, day));
        if ( it != d->dayStatMap.end() )
            it.data().second = selected;

        dt = dt.addDays(1);
    }
    while(dt < dte);
}

TimeLineWidget::SelectionMode TimeLineWidget::checkSelectionForDaysRange(const QDateTime dts, const QDateTime dte)
{
    int year, day;
    int items         = 0;
    int itemsFuz      = 0;
    int itemsSel      = 0;
    QDateTime dt      = dts;
    QMap<TimeLineWidgetPriv::YearRefPair, TimeLineWidgetPriv::StatPair>::iterator it;

    do
    {
        year = dt.date().year();
        day  = d->calendar->dayOfYear(dt.date());

        it = d->dayStatMap.find(TimeLineWidgetPriv::YearRefPair(year, day));
        if ( it != d->dayStatMap.end() )
        {
            items++;

            if (it.data().second != Unselected)
            {
                if (it.data().second == FuzzySelection)
                    itemsFuz++;
                else
                    itemsSel++;
            }
        }
        dt = dt.addDays(1);
    }
    while (dt < dte);

    if (items == 0)
        return Unselected;

    if (itemsFuz == 0 && itemsSel == 0)
        return Unselected;

    if (itemsFuz > 0)
        return FuzzySelection;

    if (items > itemsSel)
        return FuzzySelection;

    return Selected;
}

void TimeLineWidget::paintEvent(QPaintEvent*)
{
    bitBlt(this, 0, 0, &d->pixmap);
}

void TimeLineWidget::resizeEvent(QResizeEvent*)
{
    updatePixmap();
}

void TimeLineWidget::slotBackward()
{
    QDateTime ref = d->refDateTime;

    switch(d->timeUnit)
    {
        case Day:
        {
            for (int i = 0; i < 7; i++)
                ref = prevDateTime(ref);
            break;
        }
        case Week:
        {
            for (int i = 0; i < 4; i++)
                ref = prevDateTime(ref);
            break;
        }
        case Month:
        {
            for (int i = 0; i < 12; i++)
                ref = prevDateTime(ref);
            break;
        }
        case Year:
        {
            for (int i = 0; i < 5; i++)
                ref = prevDateTime(ref);
            break;
        }
    }

    if (ref < d->minDateTime)
        ref = d->minDateTime;

    setRefDateTime(ref);
}

void TimeLineWidget::slotPrevious()
{
    if (d->refDateTime <= d->minDateTime)
        return;
    QDateTime ref = prevDateTime(d->refDateTime);
    setRefDateTime(ref);
}

void TimeLineWidget::slotNext()
{
    if (d->refDateTime >= d->maxDateTime)
        return;
    QDateTime ref = nextDateTime(d->refDateTime);
    setRefDateTime(ref);
}

void TimeLineWidget::slotForward()
{
    QDateTime ref = d->refDateTime;

    switch(d->timeUnit)
    {
        case Day:
        {
            for (int i = 0; i < 7; i++)
                ref = nextDateTime(ref);
            break;
        }
        case Week:
        {
            for (int i = 0; i < 4; i++)
                ref = nextDateTime(ref);
            break;
        }
        case Month:
        {
            for (int i = 0; i < 12; i++)
                ref = nextDateTime(ref);
            break;
        }
        case Year:
        {
            for (int i = 0; i < 5; i++)
                ref = nextDateTime(ref);
            break;
        }
    }

    if (ref > d->maxDateTime)
        ref = d->maxDateTime;

    setRefDateTime(ref);
}

void TimeLineWidget::wheelEvent(QWheelEvent* e)
{
    if (e->delta() < 0)
    {
        if (e->state() & Qt::ShiftButton)
            slotForward();
        else
            slotNext();
    }

    if (e->delta() > 0)
    {
        if (e->state() & Qt::ShiftButton)
            slotBackward();
        else
            slotPrevious();
    }
}

void TimeLineWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        QPoint pt(e->x(), e->y());

        bool ctrlPressed    = e->state() & Qt::ControlButton;
        QDateTime ref       = dateTimeForPoint(pt, d->selMouseEvent);
        d->selStartDateTime = QDateTime();
        if (d->selMouseEvent) 
        {
            if (!ctrlPressed)
                resetSelection();

            d->selStartDateTime = ref;
            d->selMinDateTime   = ref;
            d->selMaxDateTime   = ref;
            setDateTimeSelected(ref, Selected);
        }

        if (!ref.isNull())
            setCursorDateTime(ref);

        d->validMouseEvent = true;
        updatePixmap();
        update();
    }
}

void TimeLineWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (d->validMouseEvent == true)
    {
        QPoint pt(e->x(), e->y());

        bool sel;
        QDateTime selEndDateTime = dateTimeForPoint(pt, sel);
        setCursorDateTime(selEndDateTime);

        // Clamp start and end date-time of current contiguous selection.

        if (!selEndDateTime.isNull() && !d->selStartDateTime.isNull())
        {
            if (selEndDateTime > d->selStartDateTime && 
                selEndDateTime > d->selMaxDateTime)
            {
                d->selMaxDateTime = selEndDateTime;
            }
            else if (selEndDateTime < d->selStartDateTime && 
                     selEndDateTime < d->selMinDateTime)
            {
                d->selMinDateTime = selEndDateTime;
            }

            QDateTime dt = d->selMinDateTime;
            do
            {
                setDateTimeSelected(dt, Unselected);
                dt = nextDateTime(dt);
            }
            while(dt <= d->selMaxDateTime);
        }

        // Now perform selections on Date Maps.

        if (d->selMouseEvent)
        {
            if (!d->selStartDateTime.isNull() && !selEndDateTime.isNull())
            {
                QDateTime dt = d->selStartDateTime;
                if (selEndDateTime > d->selStartDateTime)
                {
                    do
                    {
                        setDateTimeSelected(dt, Selected);
                        dt = nextDateTime(dt);
                    }
                    while(dt <= selEndDateTime);
                }
                else
                {
                    do
                    {
                        setDateTimeSelected(dt, Selected);
                        dt = prevDateTime(dt);
                    }
                    while(dt >= selEndDateTime);
                }
            }
        }

        updatePixmap();
        update();
    }
}

void TimeLineWidget::mouseReleaseEvent(QMouseEvent*)
{
    d->validMouseEvent = false;

    // Only dispatch changes about selection when user release mouse selection
    // to prevent multiple queries on database.
    if (d->selMouseEvent)
    {
        updateAllSelection();
        emit signalSelectionChanged();
    }

    d->selMouseEvent = false;
}

QDateTime TimeLineWidget::dateTimeForPoint(const QPoint& pt, bool &isOnSelectionArea)
{
    QRect barRect, selRect;
    isOnSelectionArea = false;

    // Check on the right of reference date.

    QDateTime ref = d->refDateTime;
    ref.setTime(QTime(0, 0, 0, 0));

    QRect deskRect = KGlobalSettings::desktopGeometry(this);
    int items = deskRect.width() / d->barWidth;

    for (int i = 0 ; i < items ; i++)
    {
        barRect.setTop(0);
        barRect.setLeft(d->startPos + i*d->barWidth);
        barRect.setBottom(height() - d->bottomMargin + 1);
        barRect.setRight(d->startPos + (i+1)*d->barWidth);

        selRect.setTop(height() - d->bottomMargin + 1);
        selRect.setLeft(d->startPos + i*d->barWidth);
        selRect.setBottom(height());
        selRect.setRight(d->startPos + (i+1)*d->barWidth);

        if (selRect.contains(pt))
            isOnSelectionArea = true;

        if (barRect.contains(pt) || selRect.contains(pt))
        {
            if (i >= d->nbItems)
            {
                // Point is outside visible widget area. We scrolling widget contents.
                slotNext();
            }

            return ref;
        }

        ref = nextDateTime(ref);
    }

    // Check on the left of reference date.

    ref = d->refDateTime;
    ref.setTime(QTime(0, 0, 0, 0));
    ref = prevDateTime(ref);

    for (int i = 0 ; i < items ; i++)
    {
        barRect.setTop(0);
        barRect.setRight(d->startPos - i*d->barWidth);
        barRect.setBottom(height() - d->bottomMargin + 1);
        barRect.setLeft(d->startPos - (i+1)*d->barWidth);

        selRect.setTop(height() - d->bottomMargin + 1);
        selRect.setLeft(d->startPos - (i+1)*d->barWidth);
        selRect.setBottom(height());
        selRect.setRight(d->startPos - i*d->barWidth);

        if (selRect.contains(pt))
            isOnSelectionArea = true;

        if (barRect.contains(pt) || selRect.contains(pt))
        {
            if (i >= d->nbItems-1)
            {
                // Point is outside visible widget area. We scrolling widget contents.
                slotPrevious();
            }

            return ref;
        }

        ref = prevDateTime(ref);
    }

    return QDateTime();
}

QDateTime TimeLineWidget::firstDayOfWeek(int year, int weekNumber)
{
    // Search the first day of first week of year.
    // We start to scan from 1st december of year-1 because 
    // first week of year OR last week of year-1 can be shared 
    // between year-1 and year.
    QDateTime d1(QDate(year-1, 12, 1));
    QDateTime dt = d1;
    int weekYear = 0;
    int weekNum  = 0;

    do
    {
        dt      = dt.addDays(1);
        weekNum = d->calendar->weekNumber(dt.date(), &weekYear);
    }
    while(weekNum != 1 && weekYear != year);

    dt = dt.addDays((weekNumber-1)*7);

/*
    DDebug() << "Year= " << year << " Week= " << weekNumber 
             << " 1st day= " << dt << endl;
*/

    return dt;
}

void TimeLineWidget::slotThemeChanged()
{
    updatePixmap();
    update();
}

}  // NameSpace Digikam
