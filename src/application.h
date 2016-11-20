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
#include <QDebug>
#include <QSettings>
#include <QScopedPointer>
#include <QWidget>

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
   ItemFactory* itemFactory()
   {
      return &itemFactory_;
   }
   /*!
    * Returns a pointer to the item source factory.
    */
   ItemSourceFactory* itemSourceFactory()
   {
      return &itemSourceFactory_;
   }

   /*!
    * Sets the setting with the key \a key to the value \a value.
    */
   void setSetting(const QString& key, const QVariant& value)
   {
      settings_.setValue(key, value);
   }
   /*!
    * Sets the setting for the object \a object and the key \a key to the value \a value.
    */
   void setSetting(QObject* object, const QString& key, const QVariant& value)
   {
      setSetting(settingKey_(object, key), value);
   }
   /*!
    * Returns the setting with the key \a key as the type \a Type or the default value
    * \a defaultValue if there is no such setting. If the setting cannot be converted
    * to \a Type a default-constructed value wil be returned.
    */
   template <typename Type> Type setting(const QString &key, const QVariant& defaultValue = QVariant()) const
   {
      return settings_.value(key, defaultValue).value<Type>();
   }
   /*!
    * Returns the setting for the object \a object with the key \a key as the type \a Type
    * or the default value \a defaultValue if there is no such setting. If the setting cannot
    * be converted to \a Type a default-constructed value wil be returned.
    */
   template <typename Type> Type setting(QObject* object, const QString& key, const QVariant& defaultValue = QVariant()) const
   {
      return setting<Type>(settingKey_(object, key), defaultValue);
   }

   /*!
    * Updates the geometry for the widget \a widget to the stored geometry or the default
    * geometry \a defaultGeometry if no geometry is stored.
    */
   void updateGeometry(QWidget* widget, const QRect& defaultGeometry);

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
    * The item source factory.
    */
   ItemSourceFactory itemSourceFactory_;

   /*!
    * \reimp
    */
   bool eventFilter(QObject* object, QEvent* event) override;

   /*!
    * Returns the setting key for the object \a object.
    */
   static QString settingKey_(QObject* object, const QString& key)
   {
      Q_ASSERT(object);
      Q_ASSERT(object->metaObject());

      return (object->metaObject()->className() + QStringLiteral("/") + key);
   }
};

#endif // APPLICATION_H
