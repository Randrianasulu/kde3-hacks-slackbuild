/****************************************************************************
** Form interface generated from reading ui file './OptionsDlgWidget.ui'
**
** Created: Вс фев 1 00:52:57 2015
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef OPTIONSDLGWIDGET_H
#define OPTIONSDLGWIDGET_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class KIntSpinBox;
class KComboBox;
class QLabel;
class KPushButton;
class KLineEdit;
class QButtonGroup;
class QCheckBox;

class OptionsDlgWidget : public QWidget
{
    Q_OBJECT

public:
    OptionsDlgWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~OptionsDlgWidget();

    KIntSpinBox* _defaultTapeSize;
    KIntSpinBox* _tapeBlockSize;
    KComboBox* _defaultTapeSizeUnits;
    QLabel* textLabel3;
    QLabel* textLabel2;
    QLabel* textLabel1;
    KPushButton* browseTarCommand;
    KLineEdit* _tapeDevice;
    QLabel* textLabel5;
    KPushButton* browseTapeDevice;
    KLineEdit* _tarCommand;
    QLabel* textLabel4;
    QButtonGroup* buttonGroup1;
    QCheckBox* _loadOnMount;
    QCheckBox* _lockOnMount;
    QCheckBox* _ejectOnUnmount;
    QCheckBox* _variableBlockSize;

signals:
    void doBrowseTapeDevice();
    void doBrowseTarCommand();
    void valueChanged();

protected:
    QHBoxLayout* OptionsDlgWidgetLayout;
    QVBoxLayout* layout4;
    QSpacerItem* spacer1;
    QGridLayout* layout5;
    QGridLayout* layout1;
    QVBoxLayout* buttonGroup1Layout;

protected slots:
    virtual void languageChange();

    virtual void slotValueChanged();
    virtual void slotBrowseTapeDevice();
    virtual void slotBrowseTarCommand();


};

#endif // OPTIONSDLGWIDGET_H
