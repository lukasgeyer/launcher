/*!
 * \file application.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QSettings>

#include "application.h"
#include "csvitemsource.h"
#include "geometrystore.h"
#include "importgroupitem.h"
#include "importitem.h"
#include "importitemeditor.h"
#include "linkgroupitem.h"
#include "linkgroupitemeditor.h"
#include "linkitem.h"
#include "linkitemeditor.h"
#include "xmlitemsource.h"

Application::Application(int& argc, char** argv) : QApplication(argc, argv),
   settings_(QSettings::IniFormat, QSettings::UserScope, QStringLiteral(APPLICATION_NAME)),
   geometryStore_(&settings_)
{
   setApplicationName(QStringLiteral(APPLICATION_NAME));
   setApplicationVersion(QStringLiteral(APPLICATION_VERSION));
   setOrganizationName(QStringLiteral(ORGANIZATION_NAME));
   setOrganizationDomain(QStringLiteral(ORGANIZATION_DOMAIN));

   itemFactory_.declare<ImportItem>(Item::Type::Import, tr("Import Item"));
   itemFactory_.declare<ImportGroupItem>(Item::Type::ImportGroup, tr("Import Group Item"));
   itemFactory_.declare<LinkItem>(Item::Type::Link, tr("Link Item"));
   itemFactory_.declare<LinkGroupItem>(Item::Type::LinkGroup, tr("Link Group Item"));

   itemEditorFactory_.declare<ImportItemEditor>(Item::Type::Import);
   itemEditorFactory_.declare<LinkItemEditor>(Item::Type::Link);
   itemEditorFactory_.declare<LinkGroupItemEditor>(Item::Type::LinkGroup);

   itemSourceFactory_.declare<CsvItemSource>("text/csv");
   itemSourceFactory_.declare<XmlItemSource>("text/xml");
}
