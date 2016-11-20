/*!
 * \file application.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QHideEvent>
#include <QSettings>
#include <QShowEvent>

#include "application.h"
#include "csvitemsource.h"
#include "event.h"
#include "importgroupitem.h"
#include "importitem.h"
#include "linkgroupitem.h"
#include "linkitem.h"
#include "xmlitemsource.h"

Application::Application(int& argc, char** argv) : QApplication(argc, argv),
   settings_(QSettings::IniFormat, QSettings::UserScope, QStringLiteral(APPLICATION_NAME))
{
   setApplicationName(QStringLiteral(APPLICATION_NAME));
   setApplicationVersion(QStringLiteral(APPLICATION_VERSION));
   setOrganizationName(QStringLiteral(ORGANIZATION_NAME));
   setOrganizationDomain(QStringLiteral(ORGANIZATION_DOMAIN));

   itemFactory_.declare<ImportItem>(Item::Type::Import, tr("Import Item"));
   itemFactory_.declare<ImportGroupItem>(Item::Type::ImportGroup, tr("Import Group Item"));
   itemFactory_.declare<LinkItem>(Item::Type::Link, tr("Link Item"));
   itemFactory_.declare<LinkGroupItem>(Item::Type::LinkGroup, tr("Link Group Item"));

   itemSourceFactory_.declare<CsvItemSource>(QStringLiteral("text/csv"));
   itemSourceFactory_.declare<XmlItemSource>(QStringLiteral("text/xml"));
}

void Application::updateGeometry(QWidget* widget, const QRect& defaultGeometry)
{
   Q_ASSERT(widget);

   //
   // Update the geometry if none is present.
   //

   const auto& geometry = setting<QByteArray>(widget, QStringLiteral("geometry"));
   if (!geometry.isNull())
   {
      widget->restoreGeometry(geometry);
   }
   else
   {
      widget->resize(defaultGeometry.size());
      widget->move(defaultGeometry.topLeft());
   }

   //
   // Install an event filter for the tracked widget so the the geometry can be automatically
   // stored and restored.
   //

   widget->installEventFilter(this);
}

bool Application::eventFilter(QObject* object, QEvent* event)
{
   if (Event::cast<QEvent>(event, QEvent::Close))
   {
      //
      // Store the current geometry.
      //

      setSetting(object, QStringLiteral("geometry"), static_cast<QWidget*>(object)->saveGeometry());

      //
      // Remove the widget as it has been destroyed.
      //

      object->removeEventFilter(this);
   }

   return false;
}
