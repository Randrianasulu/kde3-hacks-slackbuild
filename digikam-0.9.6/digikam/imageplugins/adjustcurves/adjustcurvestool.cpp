/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-12-01
 * Description : image histogram adjust curves.
 *
 * Copyright (C) 2004-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <qlayout.h>
#include <qcolor.h>
#include <qgroupbox.h>
#include <qhgroupbox.h>
#include <qvgroupbox.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qwhatsthis.h>
#include <qtooltip.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qframe.h>
#include <qtimer.h>
#include <qhbuttongroup.h>
#include <qpixmap.h>

// KDE includes.

#include <kconfig.h>
#include <kcursor.h>
#include <klocale.h>
#include <knuminput.h>
#include <kmessagebox.h>
#include <kselect.h>
#include <kfiledialog.h>
#include <kglobalsettings.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kapplication.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>
#include <kpushbutton.h>

// Local includes.

#include "daboutdata.h"
#include "ddebug.h"
#include "dimg.h"
#include "imageiface.h"
#include "imagewidget.h"
#include "imagehistogram.h"
#include "imagecurves.h"
#include "editortoolsettings.h"
#include "histogramwidget.h"
#include "curveswidget.h"
#include "colorgradientwidget.h"
#include "dimgimagefilters.h"
#include "adjustcurvestool.h"
#include "adjustcurvestool.moc"

using namespace Digikam;

namespace DigikamAdjustCurvesImagesPlugin
{

AdjustCurvesTool::AdjustCurvesTool(QObject* parent)
                : EditorTool(parent)
{
    m_destinationPreviewData = 0;

    ImageIface iface(0, 0);
    m_originalImage = iface.getOriginalImg();

    m_histoSegments = m_originalImage->sixteenBit() ? 65535 : 255;

    setName("adjustcurves");
    setToolName(i18n("Adjust Curves"));
    setToolIcon(SmallIcon("adjustcurves"));

    // -------------------------------------------------------------

    m_previewWidget = new ImageWidget("adjustcurves Tool", 0,
                                      i18n("<p>This is the image's curve-adjustments preview. "
                                           "You can pick a spot on the image "
                                           "to see the corresponding level in the histogram."));
    setToolView(m_previewWidget);

    // -------------------------------------------------------------

    m_gboxSettings = new EditorToolSettings(EditorToolSettings::Default|
                                            EditorToolSettings::Load|
                                            EditorToolSettings::SaveAs|
                                            EditorToolSettings::Ok|
                                            EditorToolSettings::Cancel);

    QGridLayout* grid = new QGridLayout(m_gboxSettings->plainPage(), 5, 5);

    QLabel *label1 = new QLabel(i18n("Channel:"), m_gboxSettings->plainPage());
    label1->setAlignment ( Qt::AlignRight | Qt::AlignVCenter );
    m_channelCB = new QComboBox( false, m_gboxSettings->plainPage() );
    m_channelCB->insertItem( i18n("Luminosity") );
    m_channelCB->insertItem( i18n("Red") );
    m_channelCB->insertItem( i18n("Green") );
    m_channelCB->insertItem( i18n("Blue") );
    m_channelCB->insertItem( i18n("Alpha") );
    m_channelCB->setCurrentText( i18n("Luminosity") );
    QWhatsThis::add( m_channelCB, i18n("<p>Select the histogram channel to display here:<p>"
                                       "<b>Luminosity</b>: display the image's luminosity values.<p>"
                                       "<b>Red</b>: display the red image-channel values.<p>"
                                       "<b>Green</b>: display the green image-channel values.<p>"
                                       "<b>Blue</b>: display the blue image-channel values.<p>"
                                       "<b>Alpha</b>: display the alpha image-channel values. "
                                       "This channel corresponds to the transparency value and "
                                       "is supported by some image formats, such as PNG or TIF."));

    m_scaleBG = new QHButtonGroup(m_gboxSettings->plainPage());
    QWhatsThis::add( m_scaleBG, i18n("<p>Select the histogram scale here.<p>"
                                     "If the image's maximal counts are small, you can use the linear scale.<p>"
                                     "Logarithmic scale can be used when the maximal counts are big; "
                                     "if it is used, all values (small and large) will be visible on the graph."));

    QPushButton *linHistoButton = new QPushButton( m_scaleBG );
    QToolTip::add( linHistoButton, i18n( "<p>Linear" ) );
    m_scaleBG->insert(linHistoButton, CurvesWidget::LinScaleHistogram);
    KGlobal::dirs()->addResourceType("histogram-lin", KGlobal::dirs()->kde_default("data") + "digikam/data");
    QString directory = KGlobal::dirs()->findResourceDir("histogram-lin", "histogram-lin.png");
    linHistoButton->setPixmap( QPixmap( directory + "histogram-lin.png" ) );
    linHistoButton->setToggleButton(true);

    QPushButton *logHistoButton = new QPushButton( m_scaleBG );
    QToolTip::add( logHistoButton, i18n( "<p>Logarithmic" ) );
    m_scaleBG->insert(logHistoButton, CurvesWidget::LogScaleHistogram);
    KGlobal::dirs()->addResourceType("histogram-log", KGlobal::dirs()->kde_default("data") + "digikam/data");
    directory = KGlobal::dirs()->findResourceDir("histogram-log", "histogram-log.png");
    logHistoButton->setPixmap( QPixmap( directory + "histogram-log.png" ) );
    logHistoButton->setToggleButton(true);

    m_scaleBG->setExclusive(true);
    m_scaleBG->setButton(CurvesWidget::LogScaleHistogram);
    m_scaleBG->setFrameShape(QFrame::NoFrame);
    m_scaleBG->setInsideMargin(0);

    QHBoxLayout* l1 = new QHBoxLayout();
    l1->addWidget(label1);
    l1->addWidget(m_channelCB);
    l1->addStretch(10);
    l1->addWidget(m_scaleBG);

    // -------------------------------------------------------------

    QWidget *curveBox = new QWidget(m_gboxSettings->plainPage());
    QGridLayout* gl   = new QGridLayout(curveBox, 4, 2, 0);

    m_histogramWidget = new HistogramWidget(256, 140, curveBox, false, true, true);
    QWhatsThis::add( m_histogramWidget, i18n("<p>Here you can see the target preview image histogram drawing "
                                             "of the selected image channel. This one is re-computed at any "
                                             "curves settings changes."));

    m_vGradient = new ColorGradientWidget( ColorGradientWidget::Vertical, 10, curveBox );
    m_vGradient->setColors( QColor( "white" ), QColor( "black" ) );

    QLabel *spacev = new QLabel(curveBox);
    spacev->setFixedWidth(1);

    m_curvesWidget = new CurvesWidget(256, 256, m_originalImage->bits(), m_originalImage->width(),
                                                m_originalImage->height(), m_originalImage->sixteenBit(),
                                                curveBox);
    QWhatsThis::add( m_curvesWidget, i18n("<p>This is the curve drawing of the selected channel from "
                                          "original image"));

    QLabel *spaceh = new QLabel(curveBox);
    spaceh->setFixedHeight(1);

    m_hGradient = new ColorGradientWidget( ColorGradientWidget::Horizontal, 10, curveBox );
    m_hGradient->setColors( QColor( "black" ), QColor( "white" ) );

    gl->addMultiCellWidget(m_histogramWidget, 0, 0, 2, 2);
    gl->addMultiCellWidget(m_vGradient,       2, 2, 0, 0);
    gl->addMultiCellWidget(spacev,            2, 2, 1, 1);
    gl->addMultiCellWidget(m_curvesWidget,    2, 2, 2, 2);
    gl->addMultiCellWidget(spaceh,            3, 3, 2, 2);
    gl->addMultiCellWidget(m_hGradient,       4, 4, 2, 2);
    gl->setRowSpacing(1, m_gboxSettings->spacingHint());

    // -------------------------------------------------------------

    m_curveType = new QHButtonGroup(m_gboxSettings->plainPage());
    m_curveFree = new QPushButton(m_curveType);
    m_curveType->insert(m_curveFree, FreeDrawing);
    KGlobal::dirs()->addResourceType("curvefree", KGlobal::dirs()->kde_default("data") +
                                     "digikam/data");
    directory = KGlobal::dirs()->findResourceDir("curvefree", "curvefree.png");
    m_curveFree->setPixmap( QPixmap( directory + "curvefree.png" ) );
    m_curveFree->setToggleButton(true);
    QToolTip::add( m_curveFree, i18n( "Curve free mode" ) );
    QWhatsThis::add( m_curveFree, i18n("<p>With this button, you can draw your curve free-hand with the mouse."));
    m_curveSmooth = new QPushButton(m_curveType);
    m_curveType->insert(m_curveSmooth, SmoothDrawing);
    KGlobal::dirs()->addResourceType("curvemooth", KGlobal::dirs()->kde_default("data") +
                                     "digikam/data");
    directory = KGlobal::dirs()->findResourceDir("curvemooth", "curvemooth.png");
    m_curveSmooth->setPixmap( QPixmap( directory + "curvemooth.png" ) );
    m_curveSmooth->setToggleButton(true);
    QToolTip::add( m_curveSmooth, i18n( "Curve smooth mode" ) );
    QWhatsThis::add( m_curveSmooth, i18n("<p>With this button, you constrains the curve type to a smooth line with tension."));
    m_curveType->setExclusive(true);
    m_curveType->setButton(SmoothDrawing);
    m_curveType->setFrameShape(QFrame::NoFrame);

    // -------------------------------------------------------------

    m_pickerColorButtonGroup = new QHButtonGroup(m_gboxSettings->plainPage());
    m_pickBlack = new QPushButton(m_pickerColorButtonGroup);
    m_pickerColorButtonGroup->insert(m_pickBlack, BlackTonal);
    KGlobal::dirs()->addResourceType("color-picker-black", KGlobal::dirs()->kde_default("data") +
                                     "digikam/data");
    directory = KGlobal::dirs()->findResourceDir("color-picker-black", "color-picker-black.png");
    m_pickBlack->setPixmap( QPixmap( directory + "color-picker-black.png" ) );
    m_pickBlack->setToggleButton(true);
    QToolTip::add( m_pickBlack, i18n( "All channels shadow tone color picker" ) );
    QWhatsThis::add( m_pickBlack, i18n("<p>With this button, you can pick the color from original image used to set <b>Shadow Tone</b> "
                                       "smooth curves point on Red, Green, Blue, and Luminosity channels."));
    m_pickGray  = new QPushButton(m_pickerColorButtonGroup);
    m_pickerColorButtonGroup->insert(m_pickGray, GrayTonal);
    KGlobal::dirs()->addResourceType("color-picker-grey", KGlobal::dirs()->kde_default("data") +
                                     "digikam/data");
    directory = KGlobal::dirs()->findResourceDir("color-picker-grey", "color-picker-grey.png");
    m_pickGray->setPixmap( QPixmap( directory + "color-picker-grey.png" ) );
    m_pickGray->setToggleButton(true);
    QToolTip::add( m_pickGray, i18n( "All channels middle tone color picker" ) );
    QWhatsThis::add( m_pickGray, i18n("<p>With this button, you can pick the color from original image used to set <b>Middle Tone</b> "
                                      "smooth curves point on Red, Green, Blue, and Luminosity channels."));
    m_pickWhite = new QPushButton(m_pickerColorButtonGroup);
    m_pickerColorButtonGroup->insert(m_pickWhite, WhiteTonal);
    KGlobal::dirs()->addResourceType("color-picker-white", KGlobal::dirs()->kde_default("data") +
                                     "digikam/data");
    directory = KGlobal::dirs()->findResourceDir("color-picker-white", "color-picker-white.png");
    m_pickWhite->setPixmap( QPixmap( directory + "color-picker-white.png" ) );
    m_pickWhite->setToggleButton(true);
    QToolTip::add( m_pickWhite, i18n( "All channels highlight tone color picker" ) );
    QWhatsThis::add( m_pickWhite, i18n("<p>With this button, you can pick the color from original image used to set <b>Highlight Tone</b> "
                                       "smooth curves point on Red, Green, Blue, and Luminosity channels."));
    m_pickerColorButtonGroup->setExclusive(true);
    m_pickerColorButtonGroup->setFrameShape(QFrame::NoFrame);

    // -------------------------------------------------------------

    m_resetButton = new QPushButton(i18n("&Reset"), m_gboxSettings->plainPage());
    m_resetButton->setPixmap( SmallIcon("reload_page", 18) );
    QToolTip::add( m_resetButton, i18n( "Reset current channel curves' values." ) );
    QWhatsThis::add( m_resetButton, i18n("<p>If you press this button, all curves' values "
                                         "from the current selected channel "
                                         "will be reset to the default values."));

    QHBoxLayout* l3 = new QHBoxLayout();
    l3->addWidget(m_curveType);
    l3->addWidget(m_pickerColorButtonGroup);
    l3->addWidget(m_resetButton);
    l3->addStretch(10);

    grid->addMultiCellLayout(l1,       0, 0, 1, 5);
    grid->addMultiCellWidget(curveBox, 1, 3, 0, 5);
    grid->addMultiCellLayout(l3,       4, 4, 1, 5);
    grid->setMargin(0);
    grid->setSpacing(m_gboxSettings->spacingHint());
    grid->setRowStretch(5, 10);

    setToolSettings(m_gboxSettings);
    init();

    // -------------------------------------------------------------

    connect(m_curvesWidget, SIGNAL(signalCurvesChanged()),
            this, SLOT(slotTimer()));

    connect(m_previewWidget, SIGNAL(spotPositionChangedFromOriginal(const Digikam::DColor&, const QPoint&)),
            this, SLOT(slotSpotColorChanged(const Digikam::DColor&)));

    connect(m_previewWidget, SIGNAL(spotPositionChangedFromTarget(const Digikam::DColor&, const QPoint&)),
            this, SLOT(slotColorSelectedFromTarget(const Digikam::DColor&)));

    connect(m_previewWidget, SIGNAL(signalResized()),
            this, SLOT(slotEffect()));

    // -------------------------------------------------------------
    // ComboBox slots.

    connect(m_channelCB, SIGNAL(activated(int)),
            this, SLOT(slotChannelChanged(int)));

    connect(m_scaleBG, SIGNAL(released(int)),
            this, SLOT(slotScaleChanged(int)));

    connect(m_curveType, SIGNAL(clicked(int)),
            this, SLOT(slotCurveTypeChanged(int)));

    // -------------------------------------------------------------
    // Buttons slots.

    connect(m_resetButton, SIGNAL(clicked()),
            this, SLOT(slotResetCurrentChannel()));

    connect(m_pickerColorButtonGroup, SIGNAL(released(int)),
            this, SLOT(slotPickerColorButtonActived()));
}

AdjustCurvesTool::~AdjustCurvesTool()
{
    if (m_destinationPreviewData)
       delete [] m_destinationPreviewData;
}

void AdjustCurvesTool::slotPickerColorButtonActived()
{
    // Save previous rendering mode and toggle to original image.
    m_currentPreviewMode = m_previewWidget->getRenderingPreviewMode();
    m_previewWidget->setRenderingPreviewMode(ImageGuideWidget::PreviewOriginalImage);
}

void AdjustCurvesTool::slotSpotColorChanged(const DColor &color)
{
    DColor sc = color;

    if ( m_pickBlack->isOn() )
    {
       // Black tonal curves point.
       m_curvesWidget->curves()->setCurvePoint(ImageHistogram::ValueChannel, 1,
                               QPoint(QMAX(QMAX(sc.red(), sc.green()), sc.blue()), 42*m_histoSegments/256));
       m_curvesWidget->curves()->setCurvePoint(ImageHistogram::RedChannel, 1, QPoint(sc.red(), 42*m_histoSegments/256));
       m_curvesWidget->curves()->setCurvePoint(ImageHistogram::GreenChannel, 1, QPoint(sc.green(), 42*m_histoSegments/256));
       m_curvesWidget->curves()->setCurvePoint(ImageHistogram::BlueChannel, 1, QPoint(sc.blue(), 42*m_histoSegments/256));
       m_pickBlack->setOn(false);
    }
    else if ( m_pickGray->isOn() )
    {
       // Gray tonal curves point.
       m_curvesWidget->curves()->setCurvePoint(ImageHistogram::ValueChannel, 8,
                               QPoint(QMAX(QMAX(sc.red(), sc.green()), sc.blue()), 128*m_histoSegments/256));
       m_curvesWidget->curves()->setCurvePoint(ImageHistogram::RedChannel, 8, QPoint(sc.red(), 128*m_histoSegments/256));
       m_curvesWidget->curves()->setCurvePoint(ImageHistogram::GreenChannel, 8, QPoint(sc.green(), 128*m_histoSegments/256));
       m_curvesWidget->curves()->setCurvePoint(ImageHistogram::BlueChannel, 8, QPoint(sc.blue(), 128*m_histoSegments/256));
       m_pickGray->setOn(false);
    }
    else if ( m_pickWhite->isOn() )
    {
       // White tonal curves point.
       m_curvesWidget->curves()->setCurvePoint(ImageHistogram::ValueChannel, 15,
                               QPoint(QMAX(QMAX(sc.red(), sc.green()), sc.blue()), 213*m_histoSegments/256));
       m_curvesWidget->curves()->setCurvePoint(ImageHistogram::RedChannel, 15, QPoint(sc.red(), 213*m_histoSegments/256));
       m_curvesWidget->curves()->setCurvePoint(ImageHistogram::GreenChannel, 15, QPoint(sc.green(), 213*m_histoSegments/256));
       m_curvesWidget->curves()->setCurvePoint(ImageHistogram::BlueChannel, 15, QPoint(sc.blue(), 213*m_histoSegments/256));
       m_pickWhite->setOn(false);
    }
    else
    {
       m_curvesWidget->setCurveGuide(color);
       return;
    }

    // Calculate Red, green, blue curves.

    for (int i = ImageHistogram::ValueChannel ; i <= ImageHistogram::BlueChannel ; i++)
       m_curvesWidget->curves()->curvesCalculateCurve(i);

    m_curvesWidget->repaint(false);

    // restore previous rendering mode.
    m_previewWidget->setRenderingPreviewMode(m_currentPreviewMode);

    slotEffect();
}

void AdjustCurvesTool::slotColorSelectedFromTarget( const DColor &color )
{
    m_histogramWidget->setHistogramGuideByColor(color);
}

void AdjustCurvesTool::slotResetCurrentChannel()
{
    m_curvesWidget->curves()->curvesChannelReset(m_channelCB->currentItem());

    m_curvesWidget->repaint();
    slotEffect();
    m_histogramWidget->reset();
}

void AdjustCurvesTool::slotEffect()
{
    ImageIface* iface = m_previewWidget->imageIface();
    uchar *orgData    = iface->getPreviewImage();
    int w             = iface->previewWidth();
    int h             = iface->previewHeight();
    bool sb           = iface->previewSixteenBit();

    // Create the new empty destination image data space.
    m_histogramWidget->stopHistogramComputation();

    if (m_destinationPreviewData)
       delete [] m_destinationPreviewData;

    m_destinationPreviewData = new uchar[w*h*(sb ? 8 : 4)];

    // Calculate the LUT to apply on the image.
    m_curvesWidget->curves()->curvesLutSetup(ImageHistogram::AlphaChannel);

    // Apply the lut to the image.
    m_curvesWidget->curves()->curvesLutProcess(orgData, m_destinationPreviewData, w, h);

    iface->putPreviewImage(m_destinationPreviewData);
    m_previewWidget->updatePreview();

    // Update histogram.
    m_histogramWidget->updateData(m_destinationPreviewData, w, h, sb, 0, 0, 0, false);
    delete [] orgData;
}

void AdjustCurvesTool::finalRendering()
{
    kapp->setOverrideCursor( KCursor::waitCursor() );
    ImageIface* iface = m_previewWidget->imageIface();
    uchar *orgData    = iface->getOriginalImage();
    int w             = iface->originalWidth();
    int h             = iface->originalHeight();
    bool sb           = iface->originalSixteenBit();

    // Create the new empty destination image data space.
    uchar* desData = new uchar[w*h*(sb ? 8 : 4)];

    // Calculate the LUT to apply on the image.
    m_curvesWidget->curves()->curvesLutSetup(ImageHistogram::AlphaChannel);

    // Apply the lut to the image.
    m_curvesWidget->curves()->curvesLutProcess(orgData, desData, w, h);

    iface->putOriginalImage(i18n("Adjust Curves"), desData);
    kapp->restoreOverrideCursor();

    delete [] orgData;
    delete [] desData;
}

void AdjustCurvesTool::slotChannelChanged(int channel)
{
    switch(channel)
    {
       case LuminosityChannel:
          m_histogramWidget->m_channelType = HistogramWidget::ValueHistogram;
          m_hGradient->setColors( QColor( "black" ), QColor( "white" ) );
          m_curvesWidget->m_channelType = CurvesWidget::ValueHistogram;
          m_vGradient->setColors( QColor( "white" ), QColor( "black" ) );
          break;

        case RedChannel:
          m_histogramWidget->m_channelType = HistogramWidget::RedChannelHistogram;
          m_hGradient->setColors( QColor( "black" ), QColor( "red" ) );
          m_curvesWidget->m_channelType = CurvesWidget::RedChannelHistogram;
          m_vGradient->setColors( QColor( "red" ), QColor( "black" ) );
          break;

       case GreenChannel:
          m_histogramWidget->m_channelType = HistogramWidget::GreenChannelHistogram;
          m_hGradient->setColors( QColor( "black" ), QColor( "green" ) );
          m_curvesWidget->m_channelType = CurvesWidget::GreenChannelHistogram;
          m_vGradient->setColors( QColor( "green" ), QColor( "black" ) );
          break;

       case BlueChannel:
          m_histogramWidget->m_channelType = HistogramWidget::BlueChannelHistogram;
          m_hGradient->setColors( QColor( "black" ), QColor( "blue" ) );
          m_curvesWidget->m_channelType = CurvesWidget::BlueChannelHistogram;
          m_vGradient->setColors( QColor( "blue" ), QColor( "black" ) );
          break;

       case AlphaChannel:
          m_histogramWidget->m_channelType = HistogramWidget::AlphaChannelHistogram;
          m_hGradient->setColors( QColor( "black" ), QColor( "white" ) );
          m_curvesWidget->m_channelType = CurvesWidget::AlphaChannelHistogram;
          m_vGradient->setColors( QColor( "white" ), QColor( "black" ) );
          break;
    }

    m_curveType->setButton(m_curvesWidget->curves()->getCurveType(channel));

    m_curvesWidget->repaint(false);
    m_histogramWidget->repaint(false);
}

void AdjustCurvesTool::slotScaleChanged(int scale)
{
    m_curvesWidget->m_scaleType    = scale;
    m_histogramWidget->m_scaleType = scale;
    m_histogramWidget->repaint(false);
    m_curvesWidget->repaint(false);
}

void AdjustCurvesTool::slotCurveTypeChanged(int type)
{
    switch(type)
    {
       case SmoothDrawing:
       {
          m_curvesWidget->curves()->setCurveType(m_curvesWidget->m_channelType, ImageCurves::CURVE_SMOOTH);
          m_pickerColorButtonGroup->setEnabled(true);
          break;
       }

       case FreeDrawing:
       {
          m_curvesWidget->curves()->setCurveType(m_curvesWidget->m_channelType, ImageCurves::CURVE_FREE);
          m_pickerColorButtonGroup->setEnabled(false);
          break;
       }
    }

    m_curvesWidget->curveTypeChanged();
}

void AdjustCurvesTool::readSettings()
{
    KConfig* config = kapp->config();
    config->setGroup("adjustcurves Tool");

    m_channelCB->setCurrentItem(config->readNumEntry("Histogram Channel", 0));    // Luminosity.
    m_scaleBG->setButton(config->readNumEntry("Histogram Scale", HistogramWidget::LogScaleHistogram));

    m_curvesWidget->reset();

    for (int i = 0 ; i < 5 ; i++)
    {
        m_curvesWidget->curves()->curvesChannelReset(i);
        m_curvesWidget->curves()->setCurveType(i, (ImageCurves::CurveType)config->readNumEntry(QString("CurveTypeChannel%1").arg(i),
                                                                                        ImageCurves::CURVE_SMOOTH));

        for (int j = 0 ; j < 17 ; j++)
        {
            QPoint disable(-1, -1);
            QPoint p = config->readPointEntry(QString("CurveAjustmentChannel%1Point%2").arg(i).arg(j), &disable);

            if (m_originalImage->sixteenBit() && p.x() != -1)
            {
                p.setX(p.x()*255);
                p.setY(p.y()*255);
            }

            m_curvesWidget->curves()->setCurvePoint(i, j, p);
        }

        m_curvesWidget->curves()->curvesCalculateCurve(i);
    }

    m_histogramWidget->reset();
    slotChannelChanged(m_channelCB->currentItem());
    slotScaleChanged(m_scaleBG->selectedId());
    slotEffect();
}

void AdjustCurvesTool::writeSettings()
{
    KConfig* config = kapp->config();
    config->setGroup("adjustcurves Tool");
    config->writeEntry("Histogram Channel", m_channelCB->currentItem());
    config->writeEntry("Histogram Scale", m_scaleBG->selectedId());

    for (int i = 0 ; i < 5 ; i++)
    {
        config->writeEntry(QString("CurveTypeChannel%1").arg(i), m_curvesWidget->curves()->getCurveType(i));

        for (int j = 0 ; j < 17 ; j++)
        {
            QPoint p = m_curvesWidget->curves()->getCurvePoint(i, j);

            if (m_originalImage->sixteenBit() && p.x() != -1)
            {
                p.setX(p.x()/255);
                p.setY(p.y()/255);
            }

            config->writeEntry(QString("CurveAjustmentChannel%1Point%2").arg(i).arg(j), p);
        }
    }

    m_previewWidget->writeSettings();
    config->sync();
}

void AdjustCurvesTool::slotResetSettings()
{
    for (int channel = 0 ; channel < 5 ; channel++)
       m_curvesWidget->curves()->curvesChannelReset(channel);

    m_curvesWidget->reset();
    slotEffect();
    m_histogramWidget->reset();
}

void AdjustCurvesTool::slotLoadSettings()
{
    KURL loadCurvesFile;

    loadCurvesFile = KFileDialog::getOpenURL(KGlobalSettings::documentPath(),
                                             QString( "*" ), kapp->activeWindow(),
                                             QString( i18n("Select Gimp Curves File to Load")) );
    if( loadCurvesFile.isEmpty() )
       return;

    if ( m_curvesWidget->curves()->loadCurvesFromGimpCurvesFile( loadCurvesFile ) == false )
    {
       KMessageBox::error(kapp->activeWindow(), i18n("Cannot load from the Gimp curves text file."));
       return;
    }

    // Refresh the current curves config.
    slotChannelChanged(m_channelCB->currentItem());
    slotEffect();
}

void AdjustCurvesTool::slotSaveAsSettings()
{
    KURL saveCurvesFile;

    saveCurvesFile = KFileDialog::getSaveURL(KGlobalSettings::documentPath(),
                                             QString( "*" ), kapp->activeWindow(),
                                             QString( i18n("Gimp Curves File to Save")) );
    if( saveCurvesFile.isEmpty() )
       return;

    if ( m_curvesWidget->curves()->saveCurvesToGimpCurvesFile( saveCurvesFile ) == false )
    {
       KMessageBox::error(kapp->activeWindow(), i18n("Cannot save to the Gimp curves text file."));
       return;
    }

    // Refresh the current curves config.
    slotChannelChanged(m_channelCB->currentItem());
}

}  // NameSpace DigikamAdjustCurvesImagesPlugin
