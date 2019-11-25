/****************************************************************************
** KAMUApp meta object code from reading C++ file 'kamuapp.h'
**
** Created: Wed Dec 20 17:19:34 2017
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "kamuapp.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8d. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *KAMUApp::className() const
{
    return "KAMUApp";
}

QMetaObject *KAMUApp::metaObj = 0;
static QMetaObjectCleanUp cleanUp_KAMUApp( "KAMUApp", &KAMUApp::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString KAMUApp::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "KAMUApp", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString KAMUApp::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "KAMUApp", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* KAMUApp::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = KUniqueApplication::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"KAMUApp", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_KAMUApp.setMetaObject( metaObj );
    return metaObj;
}

void* KAMUApp::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "KAMUApp" ) )
	return this;
    return KUniqueApplication::qt_cast( clname );
}

bool KAMUApp::qt_invoke( int _id, QUObject* _o )
{
    return KUniqueApplication::qt_invoke(_id,_o);
}

bool KAMUApp::qt_emit( int _id, QUObject* _o )
{
    return KUniqueApplication::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool KAMUApp::qt_property( int id, int f, QVariant* v)
{
    return KUniqueApplication::qt_property( id, f, v);
}

bool KAMUApp::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
