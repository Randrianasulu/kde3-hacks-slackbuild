/****************************************************************************
** IpodExport::ImageList meta object code from reading C++ file 'imagelist.h'
**
** Created: Wed Jun 14 01:10:10 2017
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "imagelist.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8d. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *IpodExport::ImageList::className() const
{
    return "IpodExport::ImageList";
}

QMetaObject *IpodExport::ImageList::metaObj = 0;
static QMetaObjectCleanUp cleanUp_IpodExport__ImageList( "IpodExport::ImageList", &IpodExport::ImageList::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString IpodExport::ImageList::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "IpodExport::ImageList", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString IpodExport::ImageList::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "IpodExport::ImageList", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* IpodExport::ImageList::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = KListView::staticMetaObject();
    static const QUParameter param_signal_0[] = {
	{ "filesPath", &static_QUType_varptr, "\x04", QUParameter::In }
    };
    static const QUMethod signal_0 = {"addedDropItems", 1, param_signal_0 };
    static const QMetaData signal_tbl[] = {
	{ "addedDropItems(QStringList)", &signal_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"IpodExport::ImageList", parentObject,
	0, 0,
	signal_tbl, 1,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_IpodExport__ImageList.setMetaObject( metaObj );
    return metaObj;
}

void* IpodExport::ImageList::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "IpodExport::ImageList" ) )
	return this;
    return KListView::qt_cast( clname );
}

#include <qobjectdefs.h>
#include <qsignalslotimp.h>

// SIGNAL addedDropItems
void IpodExport::ImageList::addedDropItems( QStringList t0 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 0 );
    if ( !clist )
	return;
    QUObject o[2];
    static_QUType_varptr.set(o+1,&t0);
    activate_signal( clist, o );
}

bool IpodExport::ImageList::qt_invoke( int _id, QUObject* _o )
{
    return KListView::qt_invoke(_id,_o);
}

bool IpodExport::ImageList::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: addedDropItems((QStringList)(*((QStringList*)static_QUType_ptr.get(_o+1)))); break;
    default:
	return KListView::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool IpodExport::ImageList::qt_property( int id, int f, QVariant* v)
{
    return KListView::qt_property( id, f, v);
}

bool IpodExport::ImageList::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
