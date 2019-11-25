/****************************************************************************
** IpodExport::IpodHeader meta object code from reading C++ file 'ipodheader.h'
**
** Created: Wed Jun 14 01:10:10 2017
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "ipodheader.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8d. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *IpodExport::IpodHeader::className() const
{
    return "IpodExport::IpodHeader";
}

QMetaObject *IpodExport::IpodHeader::metaObj = 0;
static QMetaObjectCleanUp cleanUp_IpodExport__IpodHeader( "IpodExport::IpodHeader", &IpodExport::IpodHeader::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString IpodExport::IpodHeader::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "IpodExport::IpodHeader", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString IpodExport::IpodHeader::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "IpodExport::IpodHeader", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* IpodExport::IpodHeader::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QFrame::staticMetaObject();
    static const QUMethod signal_0 = {"refreshDevices", 0, 0 };
    static const QUMethod signal_1 = {"updateSysInfo", 0, 0 };
    static const QMetaData signal_tbl[] = {
	{ "refreshDevices()", &signal_0, QMetaData::Public },
	{ "updateSysInfo()", &signal_1, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"IpodExport::IpodHeader", parentObject,
	0, 0,
	signal_tbl, 2,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_IpodExport__IpodHeader.setMetaObject( metaObj );
    return metaObj;
}

void* IpodExport::IpodHeader::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "IpodExport::IpodHeader" ) )
	return this;
    return QFrame::qt_cast( clname );
}

// SIGNAL refreshDevices
void IpodExport::IpodHeader::refreshDevices()
{
    activate_signal( staticMetaObject()->signalOffset() + 0 );
}

// SIGNAL updateSysInfo
void IpodExport::IpodHeader::updateSysInfo()
{
    activate_signal( staticMetaObject()->signalOffset() + 1 );
}

bool IpodExport::IpodHeader::qt_invoke( int _id, QUObject* _o )
{
    return QFrame::qt_invoke(_id,_o);
}

bool IpodExport::IpodHeader::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: refreshDevices(); break;
    case 1: updateSysInfo(); break;
    default:
	return QFrame::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool IpodExport::IpodHeader::qt_property( int id, int f, QVariant* v)
{
    return QFrame::qt_property( id, f, v);
}

bool IpodExport::IpodHeader::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
