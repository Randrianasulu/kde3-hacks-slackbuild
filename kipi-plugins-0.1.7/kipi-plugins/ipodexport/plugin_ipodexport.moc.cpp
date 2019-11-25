/****************************************************************************
** Plugin_iPodExport meta object code from reading C++ file 'plugin_ipodexport.h'
**
** Created: Wed Jun 14 01:10:10 2017
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "plugin_ipodexport.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8d. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *Plugin_iPodExport::className() const
{
    return "Plugin_iPodExport";
}

QMetaObject *Plugin_iPodExport::metaObj = 0;
static QMetaObjectCleanUp cleanUp_Plugin_iPodExport( "Plugin_iPodExport", &Plugin_iPodExport::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString Plugin_iPodExport::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Plugin_iPodExport", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString Plugin_iPodExport::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Plugin_iPodExport", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* Plugin_iPodExport::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = KIPI::Plugin::staticMetaObject();
    static const QUMethod slot_0 = {"slotImageUpload", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "slotImageUpload()", &slot_0, QMetaData::Private }
    };
    metaObj = QMetaObject::new_metaobject(
	"Plugin_iPodExport", parentObject,
	slot_tbl, 1,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_Plugin_iPodExport.setMetaObject( metaObj );
    return metaObj;
}

void* Plugin_iPodExport::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "Plugin_iPodExport" ) )
	return this;
    return Plugin::qt_cast( clname );
}

bool Plugin_iPodExport::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: slotImageUpload(); break;
    default:
	return Plugin::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool Plugin_iPodExport::qt_emit( int _id, QUObject* _o )
{
    return Plugin::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool Plugin_iPodExport::qt_property( int id, int f, QVariant* v)
{
    return Plugin::qt_property( id, f, v);
}

bool Plugin_iPodExport::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
