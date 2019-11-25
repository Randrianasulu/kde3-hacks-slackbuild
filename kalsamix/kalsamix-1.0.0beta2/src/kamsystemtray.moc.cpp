/****************************************************************************
** KAMSystemTray meta object code from reading C++ file 'kamsystemtray.h'
**
** Created: Wed Dec 20 17:19:34 2017
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "kamsystemtray.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8d. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *KAMSystemTray::className() const
{
    return "KAMSystemTray";
}

QMetaObject *KAMSystemTray::metaObj = 0;
static QMetaObjectCleanUp cleanUp_KAMSystemTray( "KAMSystemTray", &KAMSystemTray::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString KAMSystemTray::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "KAMSystemTray", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString KAMSystemTray::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "KAMSystemTray", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* KAMSystemTray::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = KSystemTray::staticMetaObject();
    static const QUMethod signal_0 = {"wheelUp", 0, 0 };
    static const QUMethod signal_1 = {"wheelDown", 0, 0 };
    static const QUMethod signal_2 = {"midButtonPressed", 0, 0 };
    static const QMetaData signal_tbl[] = {
	{ "wheelUp()", &signal_0, QMetaData::Public },
	{ "wheelDown()", &signal_1, QMetaData::Public },
	{ "midButtonPressed()", &signal_2, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"KAMSystemTray", parentObject,
	0, 0,
	signal_tbl, 3,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_KAMSystemTray.setMetaObject( metaObj );
    return metaObj;
}

void* KAMSystemTray::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "KAMSystemTray" ) )
	return this;
    return KSystemTray::qt_cast( clname );
}

// SIGNAL wheelUp
void KAMSystemTray::wheelUp()
{
    activate_signal( staticMetaObject()->signalOffset() + 0 );
}

// SIGNAL wheelDown
void KAMSystemTray::wheelDown()
{
    activate_signal( staticMetaObject()->signalOffset() + 1 );
}

// SIGNAL midButtonPressed
void KAMSystemTray::midButtonPressed()
{
    activate_signal( staticMetaObject()->signalOffset() + 2 );
}

bool KAMSystemTray::qt_invoke( int _id, QUObject* _o )
{
    return KSystemTray::qt_invoke(_id,_o);
}

bool KAMSystemTray::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: wheelUp(); break;
    case 1: wheelDown(); break;
    case 2: midButtonPressed(); break;
    default:
	return KSystemTray::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool KAMSystemTray::qt_property( int id, int f, QVariant* v)
{
    return KSystemTray::qt_property( id, f, v);
}

bool KAMSystemTray::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
