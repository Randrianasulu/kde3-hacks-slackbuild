// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/*
A KIPI plugin to generate HTML image galleries
Copyright 2006 Aurelien Gateau <aurelien dot gateau at free.fr>

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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/
#ifndef GENERATOR_H
#define GENERATOR_H

#include <qobject.h>

namespace KIPI {
class BatchProgressDialog;
class Interface;
}

namespace KIPIHTMLExport {

class GalleryInfo;


/**
 * This class is responsible for generating the HTML and scaling the images
 * according to the settings specified by the user.
 */
class Generator : public QObject {
Q_OBJECT
public:
	Generator(KIPI::Interface*, GalleryInfo*, KIPI::BatchProgressDialog*);
	~Generator();
	bool run();
	bool warnings() const;

private:
	struct Private;
	Private* d;
};


} // namespace

#endif /* GENERATOR_H */
