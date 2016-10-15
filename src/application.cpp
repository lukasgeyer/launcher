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
#include "importitemeditor.h"
#include "linkitemeditor.h"
#include "xmlitemsource.h"

Application::Application(int& argc, char** argv) : QApplication(argc, argv)
{
   //
   // Set up application data (the actual values are defined in the .pro file).
   //
   setApplicationName(QStringLiteral(APPLICATION_NAME));
   setApplicationVersion(QStringLiteral(APPLICATION_VERSION));
   setOrganizationName(QStringLiteral(ORGANIZATION_NAME));
   setOrganizationDomain(QStringLiteral(ORGANIZATION_DOMAIN));

   //
   // Create settings.
   //
   settings_.reset(new QSettings(QSettings::IniFormat, QSettings::UserScope, applicationName()));

   //
   // Create item editor factory and register usable item editor types.
   //
   itemEditorFactory_.reset(new ItemEditorFactory);
   itemEditorFactory_->declare<ImportItemEditor>(Item::ItemType::Import, tr("Import"));
   itemEditorFactory_->declare<LinkItemEditor>(Item::ItemType::Link, tr("Link"));

   //
   // Create item source factory and register usable item source types.
   //
   itemSourceFactory_.reset(new ItemSourceFactory);
   itemSourceFactory_->declare<CsvItemSource>("text/csv");
   itemSourceFactory_->declare<XmlItemSource>("text/xml");

   //
   // Create geometry store.
   //
   geometryStore_.reset(new GeometryStore(settings_.data()));
}
