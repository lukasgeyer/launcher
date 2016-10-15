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
#include "itemfactory.h"
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
    * Returns a pointer to the item factory.
    */
   inline ItemFactory* itemFactory()
   {
      return &itemFactory_;
   }
   /*!
    * Returns a pointer to the item editor factory.
    */
   inline ItemEditorFactory* itemEditorFactory()
   {
      return &itemEditorFactory_;
   }
   /*!
    * Returns a pointer to the item source factory.
    */
   inline ItemSourceFactory* itemSourceFactory()
   {
      return &itemSourceFactory_;
   }

   /*!
    * Returns a pointer to the application-wide settings.
    */
   inline QSettings* settings()
   {
      return &settings_;
   }

   /*!
    * Returns a pointer to the geometry store.
    */
   inline GeometryStore* geometryStore()
   {
      return &geometryStore_;
   }

private:
   /*!
    * The application-wide settings.
    */
   QSettings settings_;

   /*!
    * The item factory.
    */
   ItemFactory itemFactory_;
   /*!
    * The item editor factory.
    */
   ItemEditorFactory itemEditorFactory_;
   /*!
    * The item source factory.
    */
   ItemSourceFactory itemSourceFactory_;

   /*!
    * The geometry store.
    */
   GeometryStore geometryStore_;
};

#endif // APPLICATION_H
