/****************************************************************************
** Form interface generated from reading ui file './kreloadnetworkdlg.ui'
**
** Created: Вс фев 1 00:56:10 2015
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KRELOADNETWORKDLG_H
#define KRELOADNETWORKDLG_H

#include <qvariant.h>
#include <qpixmap.h>
#include <kdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class KPushButton;

class KReloadNetworkDlg : public KDialog
{
    Q_OBJECT

public:
    KReloadNetworkDlg( QWidget* parent = 0, const char* name = 0 );
    ~KReloadNetworkDlg();

    QLabel* tlNetworkStatus;
    KPushButton* kpbOk;
    QLabel* PixmapLabel1;
    QLabel* TextLabel1;

protected:

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;
    QPixmap image1;

};

#endif // KRELOADNETWORKDLG_H
