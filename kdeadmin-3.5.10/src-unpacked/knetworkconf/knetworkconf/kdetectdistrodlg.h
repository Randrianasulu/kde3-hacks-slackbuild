/****************************************************************************
** Form interface generated from reading ui file './kdetectdistrodlg.ui'
**
** Created: Вс фев 1 00:56:10 2015
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KDETECTDISTRODLG_H
#define KDETECTDISTRODLG_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;

class KDetectDistroDlg : public QDialog
{
    Q_OBJECT

public:
    KDetectDistroDlg( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~KDetectDistroDlg();

    QLabel* pixmapLabel1;
    QLabel* text;

protected:
    QVBoxLayout* KDetectDistroDlgLayout;
    QHBoxLayout* layout4;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;

};

#endif // KDETECTDISTRODLG_H
