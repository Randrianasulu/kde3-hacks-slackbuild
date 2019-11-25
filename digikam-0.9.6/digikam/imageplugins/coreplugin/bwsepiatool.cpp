/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-12-06
 * Description : Black and White conversion tool.
 *
 * Copyright (C) 2004-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
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

// Qt includes.

#include <qcolor.h>
#include <qcombobox.h>
#include <qfile.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qhbuttongroup.h>
#include <qhgroupbox.h>
#include <qintdict.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qtooltip.h>
#include <qvbox.h>
#include <qvgroupbox.h>
#include <qwhatsthis.h>

// KDE includes.

#include <kapplication.h>
#include <kconfig.h>
#include <kcursor.h>
#include <kfiledialog.h>
#include <kglobalsettings.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <ktabwidget.h>

// LibKDcraw includes.

#include <libkdcraw/rnuminput.h>

// Digikam includes.

#include "bcgmodifier.h"
#include "colorgradientwidget.h"
#include "curveswidget.h"
#include "dimg.h"
#include "dimgimagefilters.h"
#include "editortoolsettings.h"
#include "histogramwidget.h"
#include "imagecurves.h"
#include "imagehistogram.h"
#include "imageiface.h"
#include "imagewidget.h"
#include "listboxpreviewitem.h"

// Local includes.

#include "bwsepiatool.h"
#include "bwsepiatool.moc"

using namespace KDcrawIface;
using namespace Digikam;

namespace DigikamImagesPluginCore
{

class PreviewPixmapFactory : public QObject
{
public:

    PreviewPixmapFactory(BWSepiaTool* bwSepia);

    void invalidate() { m_previewPixmapMap.clear(); }

    const QPixmap* pixmap(int id);

private:

    QPixmap makePixmap(int id);

    QIntDict<QPixmap>    m_previewPixmapMap;
    BWSepiaTool *m_bwSepia;
};

PreviewPixmapFactory::PreviewPixmapFactory(BWSepiaTool* bwSepia)
                    : QObject(bwSepia), m_bwSepia(bwSepia)
{
    m_previewPixmapMap.setAutoDelete(true);
}

const QPixmap* PreviewPixmapFactory::pixmap(int id)
{
    if (m_previewPixmapMap.find(id) == 0)
    {
        QPixmap pix = makePixmap(id);
        m_previewPixmapMap.insert(id, new QPixmap(pix));
    }

    QPixmap* res = m_previewPixmapMap[id];

    return res;
}

QPixmap PreviewPixmapFactory::makePixmap(int id)
{
    return m_bwSepia->getThumbnailForEffect(id);
}

// -----------------------------------------------------------------------------------

class ListBoxBWPreviewItem : public ListBoxPreviewItem
{

public:

    ListBoxBWPreviewItem(QListBox *listbox, const QString &text,
                         PreviewPixmapFactory* factory, int id)
        : ListBoxPreviewItem(listbox, QPixmap(), text)
    {
          m_previewPixmapFactory = factory;
          m_id                   = id;
    };

    virtual const QPixmap* pixmap() const;

private:

    int                   m_id;
    PreviewPixmapFactory* m_previewPixmapFactory;
};

const QPixmap* ListBoxBWPreviewItem::pixmap() const
{
    return m_previewPixmapFactory->pixmap(m_id);
}

// -----------------------------------------------------------------------------------

BWSepiaTool::BWSepiaTool(QObject* parent)
           : EditorTool(parent)
{
    setName("convertbw");
    setToolName(i18n("Black && White"));
    setToolIcon(SmallIcon("bwtonal"));
    setToolHelp("blackandwhitetool.anchor");

    m_destinationPreviewData = 0;

    ImageIface iface(0, 0);
    m_originalImage  = iface.getOriginalImg();
    m_thumbnailImage = m_originalImage->smoothScale(128, 128, QSize::ScaleMin);

    // -------------------------------------------------------------

    m_previewWidget = new ImageWidget("convertbw Tool", 0,
                                      i18n("<p>Here you can see the black and white conversion tool preview. "
                                           "You can pick color on image "
                                           "to see the color level corresponding on histogram."));
    setToolView(m_previewWidget);

    // -------------------------------------------------------------

    EditorToolSettings *gboxSettings = new EditorToolSettings(EditorToolSettings::Default|
                                                              EditorToolSettings::Load|
                                                              EditorToolSettings::SaveAs|
                                                              EditorToolSettings::Ok|
                                                              EditorToolSettings::Cancel);

    QGridLayout* gridSettings = new QGridLayout(gboxSettings->plainPage(), 4, 4);

    QLabel *label1 = new QLabel(i18n("Channel:"), gboxSettings->plainPage());
    label1->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_channelCB = new QComboBox(false, gboxSettings->plainPage());
    m_channelCB->insertItem(i18n("Luminosity"));
    m_channelCB->insertItem(i18n("Red"));
    m_channelCB->insertItem(i18n("Green"));
    m_channelCB->insertItem(i18n("Blue"));
    QWhatsThis::add( m_channelCB, i18n("<p>Select the histogram channel to display here:<p>"
                                       "<b>Luminosity</b>: display the image's luminosity values.<p>"
                                       "<b>Red</b>: display the red image-channel values.<p>"
                                       "<b>Green</b>: display the green image-channel values.<p>"
                                       "<b>Blue</b>: display the blue image-channel values.<p>"));

    m_scaleBG = new QHButtonGroup(gboxSettings->plainPage());
    m_scaleBG->setExclusive(true);
    m_scaleBG->setFrameShape(QFrame::NoFrame);
    m_scaleBG->setInsideMargin(0);
    QWhatsThis::add( m_scaleBG, i18n("<p>Select the histogram scale here.<p>"
                                     "If the image's maximal counts are small, you can use the linear scale.<p>"
                                     "Logarithmic scale can be used when the maximal counts are big; "
                                     "if it is used, all values (small and large) will be visible on the graph."));

    QPushButton *linHistoButton = new QPushButton(m_scaleBG);
    QToolTip::add(linHistoButton, i18n("<p>Linear"));
    m_scaleBG->insert(linHistoButton, HistogramWidget::LinScaleHistogram);
    KGlobal::dirs()->addResourceType("histogram-lin", KGlobal::dirs()->kde_default("data") + "digikam/data");
    QString directory = KGlobal::dirs()->findResourceDir("histogram-lin", "histogram-lin.png");
    linHistoButton->setPixmap(QPixmap(directory + "histogram-lin.png"));
    linHistoButton->setToggleButton(true);

    QPushButton *logHistoButton = new QPushButton(m_scaleBG);
    QToolTip::add(logHistoButton, i18n("<p>Logarithmic"));
    m_scaleBG->insert(logHistoButton, HistogramWidget::LogScaleHistogram);
    KGlobal::dirs()->addResourceType("histogram-log", KGlobal::dirs()->kde_default("data") + "digikam/data");
    directory = KGlobal::dirs()->findResourceDir("histogram-log", "histogram-log.png");
    logHistoButton->setPixmap(QPixmap(directory + "histogram-log.png"));
    logHistoButton->setToggleButton(true);

    QHBoxLayout* l1 = new QHBoxLayout();
    l1->addWidget(label1);
    l1->addWidget(m_channelCB);
    l1->addStretch(10);
    l1->addWidget(m_scaleBG);

    gridSettings->addMultiCellLayout(l1, 0, 0, 0, 4);

    // -------------------------------------------------------------

    QVBox *histoBox   = new QVBox(gboxSettings->plainPage());
    m_histogramWidget = new HistogramWidget(256, 140, histoBox, false, true, true);
    QWhatsThis::add( m_histogramWidget, i18n("<p>Here you can see the target preview image histogram drawing "
                                             "of the selected image channel. This one is re-computed at any "
                                             "settings changes."));
    QLabel *space = new QLabel(histoBox);
    space->setFixedHeight(1);
    m_hGradient = new ColorGradientWidget(ColorGradientWidget::Horizontal, 10, histoBox);
    m_hGradient->setColors(QColor("black"), QColor("white"));

    gridSettings->addMultiCellWidget(histoBox, 1, 2, 0, 4);

    // -------------------------------------------------------------

    m_tab = new KTabWidget(gboxSettings->plainPage());

    m_bwFilm = new QListBox(m_tab);
    m_bwFilm->setColumnMode(1);
    m_bwFilm->setVariableWidth(false);
    m_bwFilm->setVariableHeight(false);
    ListBoxWhatsThis* whatsThis2 = new ListBoxWhatsThis(m_bwFilm);
    m_previewPixmapFactory       = new PreviewPixmapFactory(this);

    int type = BWGeneric;

    ListBoxBWPreviewItem *item = new ListBoxBWPreviewItem(m_bwFilm, i18n("Generic"), m_previewPixmapFactory, type);
    whatsThis2->add( item, i18n("<b>Generic</b>:"
                                "<p>Simulate a generic black and white film</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwFilm, i18n("Agfa 200X"), m_previewPixmapFactory, type);
    whatsThis2->add( item, i18n("<b>Agfa 200X</b>:"
                                "<p>Simulate the Agfa 200X black and white film at 200 ISO</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwFilm, i18n("Agfa Pan 25"), m_previewPixmapFactory, type);
    whatsThis2->add( item, i18n("<b>Agfa Pan 25</b>:"
                                "<p>Simulate the Agfa Pan black and white film at 25 ISO</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwFilm, i18n("Agfa Pan 100"), m_previewPixmapFactory, type);
    whatsThis2->add( item, i18n("<b>Agfa Pan 100</b>:"
                                "<p>Simulate the Agfa Pan black and white film at 100 ISO</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwFilm, i18n("Agfa Pan 400"), m_previewPixmapFactory, type);
    whatsThis2->add( item, i18n("<b>Agfa Pan 400</b>:"
                                "<p>Simulate the Agfa Pan black and white film at 400 ISO</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwFilm, i18n("Ilford Delta 100"), m_previewPixmapFactory, type);
    whatsThis2->add( item, i18n("<b>Ilford Delta 100</b>:"
                                "<p>Simulate the Ilford Delta black and white film at 100 ISO</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwFilm, i18n("Ilford Delta 400"), m_previewPixmapFactory, type);
    whatsThis2->add( item, i18n("<b>Ilford Delta 400</b>:"
                                "<p>Simulate the Ilford Delta black and white film at 400 ISO</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwFilm, i18n("Ilford Delta 400 Pro 3200"), m_previewPixmapFactory, type);
    whatsThis2->add( item, i18n("<b>Ilford Delta 400 Pro 3200</b>:"
                                "<p>Simulate the Ilford Delta 400 Pro black and white film at 3200 ISO</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwFilm, i18n("Ilford FP4 Plus"), m_previewPixmapFactory, type);
    whatsThis2->add( item, i18n("<b>Ilford FP4 Plus</b>:"
                                "<p>Simulate the Ilford FP4 Plus black and white film at 125 ISO</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwFilm, i18n("Ilford HP5 Plus"), m_previewPixmapFactory, type);
    whatsThis2->add( item, i18n("<b>Ilford HP5 Plus</b>:"
                                "<p>Simulate the Ilford HP5 Plus black and white film at 400 ISO</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwFilm, i18n("Ilford PanF Plus"), m_previewPixmapFactory, type);
    whatsThis2->add( item, i18n("<b>Ilford PanF Plus</b>:"
                                "<p>Simulate the Ilford PanF Plus black and white film at 50 ISO</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwFilm, i18n("Ilford XP2 Super"), m_previewPixmapFactory, type);
    whatsThis2->add( item, i18n("<b>Ilford XP2 Super</b>:"
                                "<p>Simulate the Ilford XP2 Super black and white film at 400 ISO</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwFilm, i18n("Kodak Tmax 100"), m_previewPixmapFactory, type);
    whatsThis2->add( item, i18n("<b>Kodak Tmax 100</b>:"
                                "<p>Simulate the Kodak Tmax black and white film at 100 ISO</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwFilm, i18n("Kodak Tmax 400"), m_previewPixmapFactory, type);
    whatsThis2->add( item, i18n("<b>Kodak Tmax 400</b>:"
                                "<p>Simulate the Kodak Tmax black and white film at 400 ISO</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwFilm, i18n("Kodak TriX"), m_previewPixmapFactory, type);
    whatsThis2->add( item, i18n("<b>Kodak TriX</b>:"
                                "<p>Simulate the Kodak TriX black and white film at 400 ISO</p>"));

    // -------------------------------------------------------------

    QVBox *vbox = new QVBox(m_tab);

    m_bwFilters = new QListBox(vbox);
    m_bwFilters->setColumnMode(1);
    m_bwFilters->setVariableWidth(false);
    m_bwFilters->setVariableHeight(false);
    ListBoxWhatsThis* whatsThis = new ListBoxWhatsThis(m_bwFilters);

    type = BWNoFilter;

    item = new ListBoxBWPreviewItem(m_bwFilters,
                                     i18n("No Lens Filter"), m_previewPixmapFactory, type);
    whatsThis->add( item, i18n("<b>No Lens Filter</b>:"
                               "<p>Do not apply a lens filter when rendering the image.</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwFilters, i18n("Green Filter"), m_previewPixmapFactory, type);
    whatsThis->add( item, i18n("<b>Black & White with Green Filter</b>:"
                               "<p>Simulate black and white film exposure using a green filter. "
                               "This is usefule for all scenic shoots, especially portraits "
                               "photographed against the sky.</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwFilters, i18n("Orange Filter"), m_previewPixmapFactory, type);
    whatsThis->add( item, i18n("<b>Black & White with Orange Filter</b>:"
                               "<p>Simulate black and white film exposure using an orange filter. "
                               "This will enhance landscapes, marine scenes and aerial "
                               "photography.</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwFilters, i18n("Red Filter"), m_previewPixmapFactory, type);
    whatsThis->add( item, i18n("<b>Black & White with Red Filter</b>:"
                               "<p>Simulate black and white film exposure using a red filter. "
                               "This creates dramatic sky effects, and simulates moonlight scenes "
                               "in the daytime.</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwFilters, i18n("Yellow Filter"), m_previewPixmapFactory, type);
    whatsThis->add( item, i18n("<b>Black & White with Yellow Filter</b>:"
                               "<p>Simulate black and white film exposure using a yellow filter. "
                               "This has the most natural tonal correction, and improves contrast. Ideal for "
                               "landscapes.</p>"));

    m_strengthInput = new RIntNumInput(vbox);
    m_strengthInput->input()->setLabel(i18n("Strength:"), AlignLeft | AlignVCenter);
    m_strengthInput->setRange(1, 5, 1);
    m_strengthInput->setDefaultValue(1);
    QWhatsThis::add(m_strengthInput, i18n("<p>Here, set the strength adjustment of the lens filter."));

    // -------------------------------------------------------------

    m_bwTone = new QListBox(m_tab);
    m_bwTone->setColumnMode(1);
    m_bwTone->setVariableWidth(false);
    m_bwTone->setVariableHeight(false);
    ListBoxWhatsThis* whatsThis3 = new ListBoxWhatsThis(m_bwTone);

    type = BWNoTone;

    item = new ListBoxBWPreviewItem(m_bwTone, i18n("No Tone Filter"), m_previewPixmapFactory, type);
    whatsThis3->add( item, i18n("<b>No Tone Filter</b>:"
                                "<p>Do not apply a tone filter to the image.</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwTone, i18n("Sepia Tone"), m_previewPixmapFactory, type);
    whatsThis3->add( item, i18n("<b>Black & White with Sepia Tone</b>:"
                                "<p>Gives a warm highlight and mid-tone while adding a bit of coolness to "
                                "the shadows - very similar to the process of bleaching a print and "
                                "re-developing in a sepia toner.</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwTone, i18n("Brown Tone"), m_previewPixmapFactory, type);
    whatsThis3->add( item, i18n("<b>Black & White with Brown Tone</b>:"
                                "<p>This filter is more neutral than the Sepia Tone "
                                "filter.</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwTone, i18n("Cold Tone"), m_previewPixmapFactory, type);
    whatsThis3->add( item, i18n("<b>Black & White with Cold Tone</b>:"
                                "<p>Start subtle and replicates printing on a cold tone black and white "
                                "paper such as a bromide enlarging "
                                "paper.</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwTone, i18n("Selenium Tone"), m_previewPixmapFactory, type);
    whatsThis3->add( item, i18n("<b>Black & White with Selenium Tone</b>:"
                                "<p>This effect replicates traditional selenium chemical toning done "
                                "in the darkroom.</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwTone, i18n("Platinum Tone"), m_previewPixmapFactory, type);
    whatsThis3->add( item, i18n("<b>Black & White with Platinum Tone</b>:"
                                "<p>This effect replicates traditional platinum chemical toning done "
                                "in the darkroom.</p>"));

    ++type;
    item = new ListBoxBWPreviewItem(m_bwTone, i18n("Green Tone"), m_previewPixmapFactory, type);
    whatsThis3->add( item, i18n("<b>Black & White with greenish tint</b>:"
                                "<p>This effect is also known as Verdante.</p>"));

    // -------------------------------------------------------------

    QWidget *curveBox     = new QWidget( m_tab );
    QGridLayout *gridTab2 = new QGridLayout(curveBox, 5, 2, 0);

    ColorGradientWidget* vGradient = new ColorGradientWidget(
                                                  ColorGradientWidget::Vertical,
                                                  10, curveBox);
    vGradient->setColors(QColor("white"), QColor("black"));

    QLabel *spacev = new QLabel(curveBox);
    spacev->setFixedWidth(1);

    m_curvesWidget = new CurvesWidget(256, 256, m_originalImage->bits(), m_originalImage->width(),
                                               m_originalImage->height(), m_originalImage->sixteenBit(),
                                               curveBox);
    QWhatsThis::add( m_curvesWidget, i18n("<p>This is the curve adjustment of the image luminosity"));

    QLabel *spaceh = new QLabel(curveBox);
    spaceh->setFixedHeight(1);

    ColorGradientWidget *hGradient = new ColorGradientWidget(
                                                  ColorGradientWidget::Horizontal,
                                                  10, curveBox);
    hGradient->setColors(QColor("black"), QColor("white"));

    m_cInput = new RIntNumInput(curveBox);
    m_cInput->input()->setLabel(i18n("Contrast:"), AlignLeft | AlignVCenter);
    m_cInput->setRange(-100, 100, 1);
    m_cInput->setDefaultValue(0);
    QWhatsThis::add( m_cInput, i18n("<p>Set here the contrast adjustment of the image."));

    gridTab2->addMultiCellWidget(vGradient,      0, 0, 0, 0);
    gridTab2->addMultiCellWidget(spacev,         0, 0, 1, 1);
    gridTab2->addMultiCellWidget(m_curvesWidget, 0, 0, 2, 2);
    gridTab2->addMultiCellWidget(spaceh,         1, 1, 2, 2);
    gridTab2->addMultiCellWidget(hGradient,      2, 2, 2, 2);
    gridTab2->addMultiCellWidget(m_cInput,       4, 4, 0, 2);
//    gridTab2->setRowSpacing(3);
    gridTab2->setRowStretch(5, 10);

    // -------------------------------------------------------------

    m_tab->insertTab(m_bwFilm, i18n("Film"),         FilmTab);
    m_tab->insertTab(vbox,     i18n("Lens Filters"), BWFiltersTab);
    m_tab->insertTab(m_bwTone, i18n("Tone"),         ToneTab);
    m_tab->insertTab(curveBox, i18n("Lightness"),    LuminosityTab);

    gridSettings->addMultiCellWidget(m_tab, 3, 3, 0, 4);
    gridSettings->setRowStretch(3, 10);
    setToolSettings(gboxSettings);
    init();

    // -------------------------------------------------------------

    connect(m_channelCB, SIGNAL(activated(int)),
            this, SLOT(slotChannelChanged(int)));

    connect(m_scaleBG, SIGNAL(released(int)),
            this, SLOT(slotScaleChanged(int)));

    connect(m_previewWidget, SIGNAL(spotPositionChangedFromOriginal(const Digikam::DColor&, const QPoint&)),
            this, SLOT(slotSpotColorChanged(const Digikam::DColor&)));

    connect(m_previewWidget, SIGNAL(spotPositionChangedFromTarget( const Digikam::DColor &, const QPoint & )),
            this, SLOT(slotColorSelectedFromTarget( const Digikam::DColor & )));

    connect(m_bwFilters, SIGNAL(highlighted(int)),
            this, SLOT(slotFilterSelected(int)));

    connect(m_strengthInput, SIGNAL(valueChanged(int)),
            this, SLOT(slotTimer()));

    connect(m_bwFilm, SIGNAL(highlighted(int)),
            this, SLOT(slotEffect()));

    connect(m_bwTone, SIGNAL(highlighted(int)),
            this, SLOT(slotEffect()));

    connect(m_curvesWidget, SIGNAL(signalCurvesChanged()),
            this, SLOT(slotTimer()));

    connect(m_cInput, SIGNAL(valueChanged(int)),
            this, SLOT(slotTimer()));

    connect(m_previewWidget, SIGNAL(signalResized()),
            this, SLOT(slotEffect()));
}

BWSepiaTool::~BWSepiaTool()
{
    if (m_destinationPreviewData)
        delete [] m_destinationPreviewData;
}

void BWSepiaTool::slotFilterSelected(int filter)
{
    if (filter == BWNoFilter)
        m_strengthInput->setEnabled(false);
    else
        m_strengthInput->setEnabled(true);

    slotEffect();
}

QPixmap BWSepiaTool::getThumbnailForEffect(int type)
{
    DImg thumb = m_thumbnailImage.copy();
    int w   = thumb.width();
    int h   = thumb.height();
    bool sb = thumb.sixteenBit();
    bool a  = thumb.hasAlpha();

    if (type < BWGeneric)
    {
        // In Filter view, we will render a preview of the B&W filter with the generic B&W film.
        blackAndWhiteConversion(thumb.bits(), w, h, sb, type);
        blackAndWhiteConversion(thumb.bits(), w, h, sb, BWGeneric);
    }
    else
    {
        // In Film and Tone view, we will render the preview without to use the B&W Filter
        blackAndWhiteConversion(thumb.bits(), w, h, sb, type);
    }

    if (m_curvesWidget->curves())   // in case we're called before the creator is done
    {
        uchar *targetData = new uchar[w*h*(sb ? 8 : 4)];
        m_curvesWidget->curves()->curvesLutSetup(ImageHistogram::AlphaChannel);
        m_curvesWidget->curves()->curvesLutProcess(thumb.bits(), targetData, w, h);

        DImg preview(w, h, sb, a, targetData);
        BCGModifier cmod;
        cmod.setContrast((double)(m_cInput->value()/100.0) + 1.00);
        cmod.applyBCG(preview);

        thumb.putImageData(preview.bits());

        delete [] targetData;
    }
    return (thumb.convertToPixmap());
}

void BWSepiaTool::slotChannelChanged(int channel)
{
    switch (channel)
    {
        case LuminosityChannel:
            m_histogramWidget->m_channelType = HistogramWidget::ValueHistogram;
            m_hGradient->setColors(QColor("black"), QColor("white"));
            break;

        case RedChannel:
            m_histogramWidget->m_channelType = HistogramWidget::RedChannelHistogram;
            m_hGradient->setColors(QColor("black"), QColor("red"));
            break;

        case GreenChannel:
            m_histogramWidget->m_channelType = HistogramWidget::GreenChannelHistogram;
            m_hGradient->setColors(QColor("black"), QColor("green"));
            break;

        case BlueChannel:
            m_histogramWidget->m_channelType = HistogramWidget::BlueChannelHistogram;
            m_hGradient->setColors(QColor("black"), QColor("blue"));
            break;
    }

    m_histogramWidget->repaint(false);
}

void BWSepiaTool::slotScaleChanged(int scale)
{
    m_histogramWidget->m_scaleType = scale;
    m_histogramWidget->repaint(false);
    m_curvesWidget->m_scaleType = scale;
    m_curvesWidget->repaint(false);
}

void BWSepiaTool::slotSpotColorChanged(const DColor &color)
{
    m_curvesWidget->setCurveGuide(color);
}

void BWSepiaTool::slotColorSelectedFromTarget(const DColor &color)
{
    m_histogramWidget->setHistogramGuideByColor(color);
}

void BWSepiaTool::readSettings()
{
    KConfig* config = kapp->config();
    config->setGroup("convertbw Tool");

    m_tab->setCurrentPage(config->readNumEntry("Settings Tab", BWFiltersTab));
    m_channelCB->setCurrentItem(config->readNumEntry("Histogram Channel", 0));    // Luminosity.
    m_scaleBG->setButton(config->readNumEntry("Histogram Scale", HistogramWidget::LogScaleHistogram));
    m_bwFilters->setCurrentItem(config->readNumEntry("BW Filter", 0));
    m_bwFilm->setCurrentItem(config->readNumEntry("BW Film", 0));
    m_bwTone->setCurrentItem(config->readNumEntry("BW Tone", 0));
    m_cInput->setValue(config->readNumEntry("ContrastAjustment", m_cInput->defaultValue()));
    m_strengthInput->setValue(config->readNumEntry("StrengthAjustment", m_strengthInput->defaultValue()));

    for (int i = 0 ; i < 5 ; i++)
        m_curvesWidget->curves()->curvesChannelReset(i);

    m_curvesWidget->curves()->setCurveType(m_curvesWidget->m_channelType, ImageCurves::CURVE_SMOOTH);
    m_curvesWidget->reset();

    for (int j = 0 ; j < 17 ; j++)
    {
        QPoint disable(-1, -1);
        QPoint p = config->readPointEntry(QString("CurveAjustmentPoint%1").arg(j), &disable);

        if (m_originalImage->sixteenBit() && p.x() != -1)
        {
            p.setX(p.x()*255);
            p.setY(p.y()*255);
        }

        m_curvesWidget->curves()->setCurvePoint(ImageHistogram::ValueChannel, j, p);
    }

    for (int i = 0 ; i < 5 ; i++)
        m_curvesWidget->curves()->curvesCalculateCurve(i);

    m_histogramWidget->reset();
    slotChannelChanged(m_channelCB->currentItem());
    slotScaleChanged(m_scaleBG->selectedId());
    slotFilterSelected(m_bwFilters->currentItem());
}

void BWSepiaTool::writeSettings()
{
    KConfig* config = kapp->config();
    config->setGroup("convertbw Tool");
    config->writeEntry("Settings Tab", m_tab->currentPageIndex());
    config->writeEntry("Histogram Channel", m_channelCB->currentItem());
    config->writeEntry("Histogram Scale", m_scaleBG->selectedId());
    config->writeEntry("BW Filter", m_bwFilters->currentItem());
    config->writeEntry("BW Film", m_bwFilm->currentItem());
    config->writeEntry("BW Tone", m_bwTone->currentItem());
    config->writeEntry("ContrastAjustment", m_cInput->value());
    config->writeEntry("StrengthAjustment", m_strengthInput->value());

    for (int j = 0 ; j < 17 ; j++)
    {
        QPoint p = m_curvesWidget->curves()->getCurvePoint(ImageHistogram::ValueChannel, j);

        if (m_originalImage->sixteenBit() && p.x() != -1)
        {
            p.setX(p.x()/255);
            p.setY(p.y()/255);
        }

        config->writeEntry(QString("CurveAjustmentPoint%1").arg(j), p);
    }

    m_previewWidget->writeSettings();
    config->sync();
}

void BWSepiaTool::slotResetSettings()
{
    m_bwFilm->blockSignals(true);
    m_bwFilters->blockSignals(true);
    m_bwTone->blockSignals(true);
    m_cInput->blockSignals(true);
    m_strengthInput->blockSignals(true);

    m_bwFilm->setCurrentItem(0);
    m_bwFilm->setSelected(0, true);

    m_bwFilters->setCurrentItem(0);
    m_bwFilters->setSelected(0, true);

    m_bwTone->setCurrentItem(0);
    m_bwTone->setSelected(0, true);

    m_cInput->slotReset();
    m_strengthInput->slotReset();

    for (int channel = 0; channel < 5; channel++)
        m_curvesWidget->curves()->curvesChannelReset(channel);

    m_curvesWidget->reset();

    m_bwFilm->blockSignals(false);
    m_bwFilters->blockSignals(false);
    m_bwTone->blockSignals(false);
    m_cInput->blockSignals(false);
    m_strengthInput->blockSignals(false);

    m_histogramWidget->reset();
    m_previewPixmapFactory->invalidate();
    m_bwFilters->triggerUpdate(false);
    m_bwTone->triggerUpdate(false);

    slotEffect();
}

void BWSepiaTool::slotEffect()
{
    kapp->setOverrideCursor( KCursor::waitCursor() );

    m_histogramWidget->stopHistogramComputation();

    delete [] m_destinationPreviewData;

    ImageIface* iface = m_previewWidget->imageIface();
    m_destinationPreviewData   = iface->getPreviewImage();
    int w                      = iface->previewWidth();
    int h                      = iface->previewHeight();
    bool a                     = iface->previewHasAlpha();
    bool sb                    = iface->previewSixteenBit();

    // Apply black and white filter.

    blackAndWhiteConversion(m_destinationPreviewData, w, h, sb, m_bwFilters->currentItem());

    // Apply black and white film type.

    blackAndWhiteConversion(m_destinationPreviewData, w, h, sb, m_bwFilm->currentItem() + BWGeneric);

    // Apply color tone filter.

    blackAndWhiteConversion(m_destinationPreviewData, w, h, sb, m_bwTone->currentItem() + BWNoTone);

    // Calculate and apply the curve on image.

    uchar *targetData = new uchar[w*h*(sb ? 8 : 4)];
    m_curvesWidget->curves()->curvesLutSetup(ImageHistogram::AlphaChannel);
    m_curvesWidget->curves()->curvesLutProcess(m_destinationPreviewData, targetData, w, h);

    // Adjust contrast.

    DImg preview(w, h, sb, a, targetData);
    BCGModifier cmod;
    cmod.setContrast((double)(m_cInput->value()/100.0) + 1.00);
    cmod.applyBCG(preview);
    iface->putPreviewImage(preview.bits());

    m_previewWidget->updatePreview();

    // Update histogram.

    memcpy(m_destinationPreviewData, preview.bits(), preview.numBytes());
    m_histogramWidget->updateData(m_destinationPreviewData, w, h, sb, 0, 0, 0, false);
    delete [] targetData;

    kapp->restoreOverrideCursor();
}

void BWSepiaTool::finalRendering()
{
    kapp->setOverrideCursor(KCursor::waitCursor());
    ImageIface* iface = m_previewWidget->imageIface();
    uchar *data                = iface->getOriginalImage();
    int w                      = iface->originalWidth();
    int h                      = iface->originalHeight();
    bool a                     = iface->originalHasAlpha();
    bool sb                    = iface->originalSixteenBit();

    if (data)
    {
        // Apply black and white filter.

        blackAndWhiteConversion(data, w, h, sb, m_bwFilters->currentItem());

        // Apply black and white film type.

        blackAndWhiteConversion(data, w, h, sb, m_bwFilm->currentItem() + BWGeneric);

        // Apply color tone filter.

        blackAndWhiteConversion(data, w, h, sb, m_bwTone->currentItem() + BWNoTone);

        // Calculate and apply the curve on image.

        uchar *targetData = new uchar[w*h*(sb ? 8 : 4)];
        m_curvesWidget->curves()->curvesLutSetup(ImageHistogram::AlphaChannel);
        m_curvesWidget->curves()->curvesLutProcess(data, targetData, w, h);

        // Adjust contrast.

        DImg img(w, h, sb, a, targetData);
        BCGModifier cmod;
        cmod.setContrast((double)(m_cInput->value()/100.0) + 1.00);
        cmod.applyBCG(img);

        iface->putOriginalImage(i18n("Convert to Black && White"), img.bits());

        delete [] data;
        delete [] targetData;
    }

    kapp->restoreOverrideCursor();
}

void BWSepiaTool::blackAndWhiteConversion(uchar *data, int w, int h, bool sb, int type)
{
    // Value to multiply RGB 8 bits component of mask used by changeTonality() method.
    int mul = sb ? 255 : 1;
    DImgImageFilters filter;
    double strength = 1.0 + ((double)m_strengthInput->value() - 1.0) * (1.0 / 3.0);

    switch (type)
    {
        case BWNoFilter:
            m_redAttn = 0.0;
            m_greenAttn = 0.0;
            m_blueAttn = 0.0;
            break;

        case BWGreenFilter:
            m_redAttn = -0.20 * strength;
            m_greenAttn = +0.11 * strength;
            m_blueAttn = +0.09 * strength;
            break;

        case BWOrangeFilter:
            m_redAttn = +0.48 * strength;
            m_greenAttn = -0.37 * strength;
            m_blueAttn = -0.11 * strength;
            break;

        case BWRedFilter:
            m_redAttn = +0.60 * strength;
            m_greenAttn = -0.49 * strength;
            m_blueAttn = -0.11 * strength;
            break;

        case BWYellowFilter:
            m_redAttn = +0.30 * strength;
            m_greenAttn = -0.31 * strength;
            m_blueAttn = +0.01 * strength;
            break;

            // --------------------------------------------------------------------------------

        case BWGeneric:
        case BWNoTone:
            m_redMult = 0.24;
            m_greenMult = 0.68;
            m_blueMult = 0.08;
          filter.channelMixerImage(data, w, h, sb, true, true,
                 m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
                 0.0, 1.0, 0.0,
                 0.0, 0.0, 1.0);
          break;

       case BWAgfa200X:
          m_redMult   = 0.18;
          m_greenMult = 0.41;
          m_blueMult  = 0.41;
          filter.channelMixerImage(data, w, h, sb, true, true,
                 m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
                 0.0, 1.0, 0.0,
                 0.0, 0.0, 1.0);
          break;

       case BWAgfapan25:
          m_redMult   = 0.25;
          m_greenMult = 0.39;
          m_blueMult  = 0.36;
          filter.channelMixerImage(data, w, h, sb, true, true,
                 m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
                 0.0, 1.0, 0.0,
                 0.0, 0.0, 1.0);
          break;

       case BWAgfapan100:
          m_redMult   = 0.21;
          m_greenMult = 0.40;
          m_blueMult  = 0.39;
          filter.channelMixerImage(data, w, h, sb, true, true,
                 m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
                 0.0, 1.0, 0.0,
                 0.0, 0.0, 1.0);
          break;

       case BWAgfapan400:
          m_redMult   = 0.20;
          m_greenMult = 0.41;
          m_blueMult  = 0.39;
          filter.channelMixerImage(data, w, h, sb, true, true,
                 m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
                 0.0, 1.0, 0.0,
                 0.0, 0.0, 1.0);
          break;

       case BWIlfordDelta100:
          m_redMult   = 0.21;
          m_greenMult = 0.42;
          m_blueMult  = 0.37;
          filter.channelMixerImage(data, w, h, sb, true, true,
                 m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
                 0.0, 1.0, 0.0,
                 0.0, 0.0, 1.0);
          break;

       case BWIlfordDelta400:
          m_redMult   = 0.22;
          m_greenMult = 0.42;
          m_blueMult  = 0.36;
          filter.channelMixerImage(data, w, h, sb, true, true,
                 m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
                 0.0, 1.0, 0.0,
                 0.0, 0.0, 1.0);
          break;

       case BWIlfordDelta400Pro3200:
          m_redMult   = 0.31;
          m_greenMult = 0.36;
          m_blueMult  = 0.33;
          filter.channelMixerImage(data, w, h, sb, true, true,
                 m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
                 0.0, 1.0, 0.0,
                 0.0, 0.0, 1.0);
          break;

       case BWIlfordFP4:
          m_redMult   = 0.28;
          m_greenMult = 0.41;
          m_blueMult  = 0.31;
          filter.channelMixerImage(data, w, h, sb, true, true,
                 m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
                 0.0, 1.0, 0.0,
                 0.0, 0.0, 1.0);
          break;

       case BWIlfordHP5:
          m_redMult   = 0.23;
          m_greenMult = 0.37;
          m_blueMult  = 0.40;
          filter.channelMixerImage(data, w, h, sb, true, true,
                 m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
                 0.0, 1.0, 0.0,
                 0.0, 0.0, 1.0);
          break;

       case BWIlfordPanF:
          m_redMult   = 0.33;
          m_greenMult = 0.36;
          m_blueMult  = 0.31;
          filter.channelMixerImage(data, w, h, sb, true, true,
                 m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
                 0.0, 1.0, 0.0,
                 0.0, 0.0, 1.0);
          break;

       case BWIlfordXP2Super:
          m_redMult   = 0.21;
          m_greenMult = 0.42;
          m_blueMult  = 0.37;
          filter.channelMixerImage(data, w, h, sb, true, true,
                 m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
                 0.0, 1.0, 0.0,
                 0.0, 0.0, 1.0);
          break;

       case BWKodakTmax100:
          m_redMult   = 0.24;
          m_greenMult = 0.37;
          m_blueMult  = 0.39;
          filter.channelMixerImage(data, w, h, sb, true, true,
                 m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
                 0.0, 1.0, 0.0,
                 0.0, 0.0, 1.0);
          break;

       case BWKodakTmax400:
          m_redMult   = 0.27;
          m_greenMult = 0.36;
          m_blueMult  = 0.37;
          filter.channelMixerImage(data, w, h, sb, true, true,
                 m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
                 0.0, 1.0, 0.0,
                 0.0, 0.0, 1.0);
          break;

       case BWKodakTriX:
          m_redMult   = 0.25;
          m_greenMult = 0.35;
          m_blueMult  = 0.40;
          filter.channelMixerImage(data, w, h, sb, true, true,
                 m_redMult + m_redMult*m_redAttn, m_greenMult + m_greenMult*m_greenAttn, m_blueMult + m_blueMult*m_blueAttn,
                 0.0, 1.0, 0.0,
                 0.0, 0.0, 1.0);
          break;

       // --------------------------------------------------------------------------------

       case BWSepiaTone:
          filter.changeTonality(data, w, h, sb, 162*mul, 132*mul, 101*mul);
          break;

       case BWBrownTone:
          filter.changeTonality(data, w, h, sb, 129*mul, 115*mul, 104*mul);
          break;

       case BWColdTone:
          filter.changeTonality(data, w, h, sb, 102*mul, 109*mul, 128*mul);
          break;

       case BWSeleniumTone:
          filter.changeTonality(data, w, h, sb, 122*mul, 115*mul, 122*mul);
          break;

       case BWPlatinumTone:
          filter.changeTonality(data, w, h, sb, 115*mul, 110*mul, 106*mul);
          break;

       case BWGreenTone:
          filter.changeTonality(data, w, h, sb, 108*mul, 116*mul, 100*mul);
          break;

    }
}

//-- Load all settings from file --------------------------------------

void BWSepiaTool::slotLoadSettings()
{
    KURL loadFile = KFileDialog::getOpenURL(KGlobalSettings::documentPath(),
                                            QString( "*" ), kapp->activeWindow(),
                                            QString( i18n("Black & White Settings File to Load")) );
    if( loadFile.isEmpty() )
       return;

    QFile file(loadFile.path());

    if (file.open(IO_ReadOnly))
    {
        QTextStream stream(&file);

        if (stream.readLine() != "# Black & White Configuration File")
        {
           KMessageBox::error(kapp->activeWindow(),
                              i18n("\"%1\" is not a Black & White settings text file.")
                              .arg(loadFile.fileName()));
           file.close();
           return;
        }

        m_bwFilters->blockSignals(true);
        m_bwTone->blockSignals(true);
        m_cInput->blockSignals(true);

        m_bwFilters->setCurrentItem(stream.readLine().toInt());
        m_bwTone->setCurrentItem(stream.readLine().toInt());
        m_cInput->setValue(stream.readLine().toInt());

        for (int i = 0 ; i < 5 ; i++)
            m_curvesWidget->curves()->curvesChannelReset(i);

        m_curvesWidget->curves()->setCurveType(m_curvesWidget->m_channelType, ImageCurves::CURVE_SMOOTH);
        m_curvesWidget->reset();

        for (int j = 0; j < 17; j++)
        {
            QPoint disable(-1, -1);
            QPoint p;
            p.setX(stream.readLine().toInt());
            p.setY(stream.readLine().toInt());

            if (m_originalImage->sixteenBit() && p != disable)
            {
                p.setX(p.x()*255);
                p.setY(p.y()*255);
            }

            m_curvesWidget->curves()->setCurvePoint(ImageHistogram::ValueChannel, j, p);
        }

        for (int i = 0 ; i < 5 ; i++)
           m_curvesWidget->curves()->curvesCalculateCurve(i);

        m_bwFilters->blockSignals(false);
        m_bwTone->blockSignals(false);
        m_cInput->blockSignals(false);

        m_histogramWidget->reset();
        m_previewPixmapFactory->invalidate();
        m_bwFilters->triggerUpdate(false);
        m_bwTone->triggerUpdate(false);

        slotEffect();
    }
    else
        KMessageBox::error(kapp->activeWindow(),
                           i18n("Cannot load settings from the Black & White text file."));

    file.close();
}

//-- Save all settings to file ---------------------------------------

void BWSepiaTool::slotSaveAsSettings()
{
    KURL saveFile = KFileDialog::getSaveURL(KGlobalSettings::documentPath(),
                                            QString( "*" ), kapp->activeWindow(),
                                            QString( i18n("Black & White Settings File to Save")));
    if( saveFile.isEmpty() )
       return;

    QFile file(saveFile.path());

    if (file.open(IO_WriteOnly))
    {
        QTextStream stream(&file);
        stream << "# Black & White Configuration File\n";
        stream << m_bwFilters->currentItem() << "\n";
        stream << m_bwTone->currentItem() << "\n";
        stream << m_cInput->value() << "\n";

        for (int j = 0; j < 17; j++)
        {
            QPoint p = m_curvesWidget->curves()->getCurvePoint(ImageHistogram::ValueChannel, j);
            if (m_originalImage->sixteenBit())
            {
                p.setX(p.x() / 255);
                p.setY(p.y() / 255);
            }
            stream << p.x() << "\n";
            stream << p.y() << "\n";
        }
    }
    else
        KMessageBox::error(kapp->activeWindow(),
                           i18n("Cannot save settings to the Black & White text file."));

    file.close();
}

}  // NameSpace DigikamImagesPluginCore
