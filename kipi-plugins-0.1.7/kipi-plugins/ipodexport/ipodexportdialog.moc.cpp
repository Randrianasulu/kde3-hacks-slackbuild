/****************************************************************************
** IpodExport::UploadDialog meta object code from reading C++ file 'ipodexportdialog.h'
**
** Created: Wed Jun 14 01:10:10 2017
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "ipodexportdialog.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8d. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *IpodExport::UploadDialog::className() const
{
    return "IpodExport::UploadDialog";
}

QMetaObject *IpodExport::UploadDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_IpodExport__UploadDialog( "IpodExport::UploadDialog", &IpodExport::UploadDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString IpodExport::UploadDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "IpodExport::UploadDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString IpodExport::UploadDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "IpodExport::UploadDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* IpodExport::UploadDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = KDialogBase::staticMetaObject();
    static const QUMethod slot_0 = {"startTransfer", 0, 0 };
    static const QUParameter param_slot_1[] = {
	{ "filesPath", &static_QUType_varptr, "\x04", QUParameter::In }
    };
    static const QUMethod slot_1 = {"addDropItems", 1, param_slot_1 };
    static const QUParameter param_slot_2[] = {
	{ "item", &static_QUType_ptr, "QListViewItem", QUParameter::In }
    };
    static const QUMethod slot_2 = {"imageSelected", 1, param_slot_2 };
    static const QUParameter param_slot_3[] = {
	{ 0, &static_QUType_ptr, "KFileItem", QUParameter::In },
	{ "pixmap", &static_QUType_varptr, "\x06", QUParameter::In }
    };
    static const QUMethod slot_3 = {"gotImagePreview", 2, param_slot_3 };
    static const QUParameter param_slot_4[] = {
	{ "item", &static_QUType_ptr, "QListViewItem", QUParameter::In }
    };
    static const QUMethod slot_4 = {"ipodItemSelected", 1, param_slot_4 };
    static const QUParameter param_slot_5[] = {
	{ 0, &static_QUType_ptr, "QListViewItem", QUParameter::In }
    };
    static const QUMethod slot_5 = {"ipodShowContextMenu", 1, param_slot_5 };
    static const QUMethod slot_6 = {"imagesFilesButtonAdd", 0, 0 };
    static const QUMethod slot_7 = {"imagesFilesButtonRem", 0, 0 };
    static const QUMethod slot_8 = {"createIpodAlbum", 0, 0 };
    static const QUMethod slot_9 = {"deleteIpodAlbum", 0, 0 };
    static const QUMethod slot_10 = {"renameIpodAlbum", 0, 0 };
    static const QUMethod slot_11 = {"refreshDevices", 0, 0 };
    static const QUMethod slot_12 = {"updateSysInfo", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "startTransfer()", &slot_0, QMetaData::Private },
	{ "addDropItems(QStringList)", &slot_1, QMetaData::Private },
	{ "imageSelected(QListViewItem*)", &slot_2, QMetaData::Private },
	{ "gotImagePreview(const KFileItem*,const QPixmap&)", &slot_3, QMetaData::Private },
	{ "ipodItemSelected(QListViewItem*)", &slot_4, QMetaData::Private },
	{ "ipodShowContextMenu(QListViewItem*)", &slot_5, QMetaData::Private },
	{ "imagesFilesButtonAdd()", &slot_6, QMetaData::Private },
	{ "imagesFilesButtonRem()", &slot_7, QMetaData::Private },
	{ "createIpodAlbum()", &slot_8, QMetaData::Private },
	{ "deleteIpodAlbum()", &slot_9, QMetaData::Private },
	{ "renameIpodAlbum()", &slot_10, QMetaData::Private },
	{ "refreshDevices()", &slot_11, QMetaData::Private },
	{ "updateSysInfo()", &slot_12, QMetaData::Private }
    };
    metaObj = QMetaObject::new_metaobject(
	"IpodExport::UploadDialog", parentObject,
	slot_tbl, 13,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_IpodExport__UploadDialog.setMetaObject( metaObj );
    return metaObj;
}

void* IpodExport::UploadDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "IpodExport::UploadDialog" ) )
	return this;
    return KDialogBase::qt_cast( clname );
}

bool IpodExport::UploadDialog::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: startTransfer(); break;
    case 1: addDropItems((QStringList)(*((QStringList*)static_QUType_ptr.get(_o+1)))); break;
    case 2: imageSelected((QListViewItem*)static_QUType_ptr.get(_o+1)); break;
    case 3: gotImagePreview((const KFileItem*)static_QUType_ptr.get(_o+1),(const QPixmap&)*((const QPixmap*)static_QUType_ptr.get(_o+2))); break;
    case 4: ipodItemSelected((QListViewItem*)static_QUType_ptr.get(_o+1)); break;
    case 5: ipodShowContextMenu((QListViewItem*)static_QUType_ptr.get(_o+1)); break;
    case 6: imagesFilesButtonAdd(); break;
    case 7: imagesFilesButtonRem(); break;
    case 8: createIpodAlbum(); break;
    case 9: deleteIpodAlbum(); break;
    case 10: renameIpodAlbum(); break;
    case 11: refreshDevices(); break;
    case 12: updateSysInfo(); break;
    default:
	return KDialogBase::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool IpodExport::UploadDialog::qt_emit( int _id, QUObject* _o )
{
    return KDialogBase::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool IpodExport::UploadDialog::qt_property( int id, int f, QVariant* v)
{
    return KDialogBase::qt_property( id, f, v);
}

bool IpodExport::UploadDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
