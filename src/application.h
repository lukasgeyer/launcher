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
#include "itemeditorfactory.h"
#include "itemsourcefactory.h"

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
    * Returns a pointer to the geometry store.
    */
   inline GeometryStore* geometryStore()
   {
      Q_ASSERT(geometryStore_.isNull() == false);

      return geometryStore_.data();
   }

   /*!
    * Returns a pointer to the item editor factory.
    */
   inline ItemEditorFactory* itemEditorFactory()
   {
      Q_ASSERT(itemEditorFactory_.isNull() == false);

      return itemEditorFactory_.data();
   }
   /*!
    * Returns a pointer to the item source factory.
    */
   inline ItemSourceFactory* itemSourceFactory()
   {
      Q_ASSERT(itemSourceFactory_.isNull() == false);

      return itemSourceFactory_.data();
   }

   /*!
    * Returns a pointer to the application-wide settings.
    */
   inline QSettings* settings()
   {
      Q_ASSERT(settings_.isNull() == false);

      return settings_.data();
   }

private:
   /*!
    * The geometry store.
    */
   QScopedPointer<GeometryStore> geometryStore_;

   /*!
    * The item editor factory.
    */
   QScopedPointer<ItemEditorFactory> itemEditorFactory_;
   /*!
    * The item source factory.
    */
   QScopedPointer<ItemSourceFactory> itemSourceFactory_;

   /*!
    * The application-wide settings.
    */
   QScopedPointer<QSettings> settings_;
};

#endif // APPLICATION_H
