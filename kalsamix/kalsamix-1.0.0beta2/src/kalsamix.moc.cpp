/****************************************************************************
** kalsamix meta object code from reading C++ file 'kalsamix.h'
**
** Created: Wed Dec 20 17:19:34 2017
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "kalsamix.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8d. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *kalsamix::className() const
{
    return "kalsamix";
}

QMetaObject *kalsamix::metaObj = 0;
static QMetaObjectCleanUp cleanUp_kalsamix( "kalsamix", &kalsamix::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString kalsamix::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "kalsamix", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString kalsamix::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "kalsamix", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* kalsamix::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = KMainWindow::staticMetaObject();
    static const QUMethod slot_0 = {"reinitApp", 0, 0 };
    static const QUMethod slot_1 = {"readMixer", 0, 0 };
    static const QUMethod slot_2 = {"readConfig", 0, 0 };
    static const QUMethod slot_3 = {"showConfig", 0, 0 };
    static const QUMethod slot_4 = {"quit", 0, 0 };
    static const QUMethod slot_5 = {"volDown", 0, 0 };
    static const QUParameter param_slot_6[] = {
	{ "v", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_6 = {"volDown", 1, param_slot_6 };
    static const QUMethod slot_7 = {"volUp", 0, 0 };
    static const QUParameter param_slot_8[] = {
	{ "v", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_8 = {"volUp", 1, param_slot_8 };
    static const QUMethod slot_9 = {"reinitControls", 0, 0 };
    static const QUMethod slot_10 = {"showAbout", 0, 0 };
    static const QUMethod slot_11 = {"showMaster", 0, 0 };
    static const QUMethod slot_12 = {"toggleViewTypeV", 0, 0 };
    static const QUMethod slot_13 = {"toggleViewTypeH", 0, 0 };
    static const QUParameter param_slot_14[] = {
	{ "t", &static_QUType_ptr, "KMixerView::viewType", QUParameter::In }
    };
    static const QUMethod slot_14 = {"toggleViewType", 1, param_slot_14 };
    static const QUParameter param_slot_15[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_15 = {"presetsPopup", 1, param_slot_15 };
    static const QUParameter param_slot_16[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_16 = {"presetsPopupMenu", 1, param_slot_16 };
    static const QUMethod slot_17 = {"readLevel", 0, 0 };
    static const QUMethod slot_18 = {"toggleMeter", 0, 0 };
    static const QUMethod slot_19 = {"midButtonPressed", 0, 0 };
    static const QUMethod slot_20 = {"updateTrayTip", 0, 0 };
    static const QUMethod slot_21 = {"osdEnable", 0, 0 };
    static const QUMethod slot_22 = {"osdDisable", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "reinitApp()", &slot_0, QMetaData::Public },
	{ "readMixer()", &slot_1, QMetaData::Public },
	{ "readConfig()", &slot_2, QMetaData::Public },
	{ "showConfig()", &slot_3, QMetaData::Public },
	{ "quit()", &slot_4, QMetaData::Public },
	{ "volDown()", &slot_5, QMetaData::Public },
	{ "volDown(int)", &slot_6, QMetaData::Public },
	{ "volUp()", &slot_7, QMetaData::Public },
	{ "volUp(int)", &slot_8, QMetaData::Public },
	{ "reinitControls()", &slot_9, QMetaData::Public },
	{ "showAbout()", &slot_10, QMetaData::Public },
	{ "showMaster()", &slot_11, QMetaData::Public },
	{ "toggleViewTypeV()", &slot_12, QMetaData::Public },
	{ "toggleViewTypeH()", &slot_13, QMetaData::Public },
	{ "toggleViewType(KMixerView::viewType)", &slot_14, QMetaData::Public },
	{ "presetsPopup(int)", &slot_15, QMetaData::Public },
	{ "presetsPopupMenu(int)", &slot_16, QMetaData::Public },
	{ "readLevel()", &slot_17, QMetaData::Public },
	{ "toggleMeter()", &slot_18, QMetaData::Public },
	{ "midButtonPressed()", &slot_19, QMetaData::Public },
	{ "updateTrayTip()", &slot_20, QMetaData::Public },
	{ "osdEnable()", &slot_21, QMetaData::Public },
	{ "osdDisable()", &slot_22, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"kalsamix", parentObject,
	slot_tbl, 23,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_kalsamix.setMetaObject( metaObj );
    return metaObj;
}

void* kalsamix::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "kalsamix" ) )
	return this;
    if ( !qstrcmp( clname, "KAMDCOPIface" ) )
	return (KAMDCOPIface*)this;
    return KMainWindow::qt_cast( clname );
}

bool kalsamix::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: reinitApp(); break;
    case 1: readMixer(); break;
    case 2: readConfig(); break;
    case 3: showConfig(); break;
    case 4: quit(); break;
    case 5: volDown(); break;
    case 6: volDown((int)static_QUType_int.get(_o+1)); break;
    case 7: volUp(); break;
    case 8: volUp((int)static_QUType_int.get(_o+1)); break;
    case 9: reinitControls(); break;
    case 10: showAbout(); break;
    case 11: showMaster(); break;
    case 12: toggleViewTypeV(); break;
    case 13: toggleViewTypeH(); break;
    case 14: toggleViewType((KMixerView::viewType)(*((KMixerView::viewType*)static_QUType_ptr.get(_o+1)))); break;
    case 15: presetsPopup((int)static_QUType_int.get(_o+1)); break;
    case 16: presetsPopupMenu((int)static_QUType_int.get(_o+1)); break;
    case 17: readLevel(); break;
    case 18: toggleMeter(); break;
    case 19: midButtonPressed(); break;
    case 20: updateTrayTip(); break;
    case 21: osdEnable(); break;
    case 22: osdDisable(); break;
    default:
	return KMainWindow::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool kalsamix::qt_emit( int _id, QUObject* _o )
{
    return KMainWindow::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool kalsamix::qt_property( int id, int f, QVariant* v)
{
    return KMainWindow::qt_property( id, f, v);
}

bool kalsamix::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
