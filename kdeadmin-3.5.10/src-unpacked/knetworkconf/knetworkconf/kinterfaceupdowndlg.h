/****************************************************************************
** Form interface generated from reading ui file './kinterfaceupdowndlg.ui'
**
** Created: Вс фев 1 00:56:08 2015
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KINTERFACEUPDOWNDLG_H
#define KINTERFACEUPDOWNDLG_H

#include <qvariant.h>
#include <qpixmap.h>
#include <kdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;

class KInterfaceUpDownDlg : public KDialog
{
    Q_OBJECT

public:
    KInterfaceUpDownDlg( QWidget* parent = 0, const char* name = 0 );
    ~KInterfaceUpDownDlg();

    QLabel* pixmapLabel1;
    QLabel* label;

public slots:
    virtual void close();

protected:
    QHBoxLayout* KInterfaceUpDownDlgLayout;
    QSpacerItem* spacer12;
    QSpacerItem* spacer14;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;
    QPixmap image1;

};

#endif // KINTERFACEUPDOWNDLG_H
