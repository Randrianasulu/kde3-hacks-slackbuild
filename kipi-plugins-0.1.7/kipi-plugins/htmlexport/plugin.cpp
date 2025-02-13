// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/*
A KIPI plugin to generate HTML image galleries
Copyright 2006 by Aurelien Gateau <aurelien dot gateau at free.fr>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.

*/
// Self
#include "plugin.moc"

// KDE
#include <kaction.h>
#include <kapplication.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>
#include <krun.h>

// KIPI
#include <libkipi/batchprogressdialog.h>

// Local
#include "galleryinfo.h"
#include "generator.h"
#include "wizard.h"

typedef KGenericFactory<KIPIHTMLExport::Plugin> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_htmlexport, Factory("kipiplugin_htmlexport"))

namespace KIPIHTMLExport {


struct Plugin::Private {
	KAction* mAction;
};


Plugin::Plugin(QObject *parent, const char*, const QStringList&)
: KIPI::Plugin(Factory::instance(), parent, "HTMLExport")
{
	d=new Private;
	d->mAction=0;
}


Plugin::~Plugin() {
	delete d;
}


void Plugin::setup( QWidget* widget ) {
	KIPI::Plugin::setup( widget );
	d->mAction = new KAction(i18n("HTML Gallery..."), "www", 0,
		this, SLOT(slotActivate()),
		actionCollection(), "htmlexport");
	addAction(d->mAction);
}


void Plugin::slotActivate() {
	KIPI::Interface* interface = dynamic_cast< KIPI::Interface* >( parent() );
	Q_ASSERT(interface);

	GalleryInfo info;
	info.readConfig();
	QWidget* parent=KApplication::kApplication()->mainWidget();
	Wizard wizard(parent, interface, &info);
	if (wizard.exec()==QDialog::Rejected) return;
	info.writeConfig();
	
	KIPI::BatchProgressDialog* progressDialog=new KIPI::BatchProgressDialog(parent, i18n("Generating gallery..."));
	
	Generator generator(interface, &info, progressDialog);
	progressDialog->show();
	if (!generator.run()) return;

	if (!generator.warnings()) {
		progressDialog->close();
	}

	if (info.openInBrowser()) {
		KURL url=info.destKURL();
		url.addPath("index.html");
		KRun::runURL(url, "text/html");
	}
}


KIPI::Category Plugin::category(KAction* action) const {
	if (action == d->mAction) {
		return KIPI::EXPORTPLUGIN;
	}
	
	kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
	return KIPI::EXPORTPLUGIN; // no warning from compiler, please
}

} // namespace
