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
#include "listthemeparameter.h"

// Qt
#include <qcombobox.h>

// KDE
#include <kconfigbase.h>

namespace KIPIHTMLExport {

static const char* ITEM_VALUE_KEY = "Value_";
static const char* ITEM_CAPTION_KEY = "Caption_";

struct ListThemeParameter::Private {
	QStringList mOrderedValueList;
	QMap<QString, QString> mContentMap;
};

ListThemeParameter::ListThemeParameter() {
	d = new Private;
}

ListThemeParameter::~ListThemeParameter() {
	delete d;
}

void ListThemeParameter::init(const QCString& internalName, const KConfigBase* configFile) {
	AbstractThemeParameter::init(internalName, configFile);

	for (int pos=0;; ++pos) {
		QString valueKey = QString("%1%2").arg(ITEM_VALUE_KEY).arg(pos);
		QString captionKey = QString("%1%2").arg(ITEM_CAPTION_KEY).arg(pos);
		if (!configFile->hasKey(valueKey) || !configFile->hasKey(captionKey)) {
			break;
		}

		QString value = configFile->readEntry(valueKey);
		QString caption = configFile->readEntry(captionKey);

		d->mOrderedValueList << value;
		d->mContentMap[value] = caption;
	}
}

QWidget* ListThemeParameter::createWidget(QWidget* parent, const QString& widgetDefaultValue) const {
	QComboBox* comboBox = new QComboBox(parent);

	QStringList::ConstIterator
		it = d->mOrderedValueList.begin(),
		end = d->mOrderedValueList.end();
	for (;it!=end; ++it) {
		QString value = *it;
		QString caption = d->mContentMap[value];
		comboBox->insertItem(caption);
		if (value == widgetDefaultValue) {
			comboBox->setCurrentItem(comboBox->count() - 1);
		}
	}

	return comboBox;
}

QString ListThemeParameter::valueFromWidget(QWidget* widget) const {
	Q_ASSERT(widget);
	QComboBox* comboBox = static_cast<QComboBox*>(widget);
	return d->mOrderedValueList[comboBox->currentItem()];
}

} // namespace

