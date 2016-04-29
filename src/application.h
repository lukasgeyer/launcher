/*!
 * \file application.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QSettings>
#include <QScopedPointer>

#include "geometrystore.h"


/*!
 * \brief The application.
 */
class Application : public QApplication
{
public:
   /*!
    * Constructs an Application object. At most one object may be created at any time.
    */
   Application(int& argc, char** argv);

   /*!
    * Returns a reference to the geometry store.
    */
   GeometryStore& geometryStore();
   /*!
    * Returns a const reference to the geometry store.
    */
   const GeometryStore& geometryStore() const;

   /*!
    * Returns a reference to the application-wide settings.
    */
   QSettings& settings();
   /*!
    * Returns a const reference to the application-wide settings.
    */
   const QSettings& settings() const;

private:
   /*!
    * The geometry store.
    */
   QScopedPointer<GeometryStore> geometryStore_;
   /*!
    * The application-wide settings.
    */
   QScopedPointer<QSettings> settings_;
};

#endif // APPLICATION_H
