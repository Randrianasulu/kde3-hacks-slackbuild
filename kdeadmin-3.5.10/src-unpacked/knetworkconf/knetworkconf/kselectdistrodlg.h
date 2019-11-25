/****************************************************************************
** Form interface generated from reading ui file './kselectdistrodlg.ui'
**
** Created: Вс фев 1 00:56:08 2015
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef KSELECTDISTRODLG_H
#define KSELECTDISTRODLG_H

#include <qvariant.h>
#include <qpixmap.h>
#include <kdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class KListBox;
class QListBoxItem;
class KPushButton;
class QCheckBox;
class QLabel;

class KSelectDistroDlg : public KDialog
{
    Q_OBJECT

public:
    KSelectDistroDlg( QWidget* parent = 0, const char* name = 0 );
    ~KSelectDistroDlg();

    KListBox* klbDistroList;
    KPushButton* kpbOk;
    KPushButton* kpbCancel;
    QCheckBox* cbAskAgain;
    QLabel* textLabel1;
    QLabel* textLabel2;

public slots:
    virtual void exitSlot();

protected:
    QGridLayout* KSelectDistroDlgLayout;
    QSpacerItem* Horizontal_Spacing2;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;

};

#endif // KSELECTDISTRODLG_H
