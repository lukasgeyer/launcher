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
#include "geometrystore.h"

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
   // Create geometry store.
   //
   geometryStore_.reset(new GeometryStore(settings_.data()));
}

GeometryStore& Application::geometryStore()
{
   Q_ASSERT(geometryStore_.isNull() == false);

   return *geometryStore_;
}

const GeometryStore& Application::geometryStore() const
{
   Q_ASSERT(geometryStore_.isNull() == false);

   return *geometryStore_;
}

QSettings& Application::settings()
{
   Q_ASSERT(settings_.isNull() == false);

   return *settings_;
}

const QSettings& Application::settings() const
{
   Q_ASSERT(settings_.isNull() == false);

   return *settings_;
}
