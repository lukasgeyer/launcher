/*!
 * \file geometrystore.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QHideEvent>
#include <QSettings>
#include <QShowEvent>
#include <QWidget>

#include "event.h"
#include "geometrystore.h"

GeometryStore::GeometryStore(QSettings* settings, QObject* parent) : QObject(parent), settings_(settings)
{
   Q_ASSERT(settings != nullptr);
}

void GeometryStore::addWidget(QWidget *widget, const QRect& defaultGeometry)
{
   Q_ASSERT(widget != nullptr);

   ///
   /// Update the default geometry.
   ///
   settings_->setValue(defaultGeometryKey(widget), defaultGeometry);

   ///
   /// Install an event filter for the tracked widget so the the geometry can be automatically
   /// stored and restored.
   ///
   widget->installEventFilter(this);
}

void GeometryStore::removeWidget(QWidget *widget)
{
   Q_ASSERT(widget != nullptr);

   ///
   /// Remove the event filter from the tracked widget.
   ///
   widget->removeEventFilter(this);
}

bool GeometryStore::eventFilter(QObject* object, QEvent* event)
{
   Q_ASSERT(event != nullptr);
   Q_ASSERT(object->isWidgetType());

   auto widget = static_cast<QWidget*>(object);

   if (/* auto showEvent = */ eventAs<QShowEvent>(event, QEvent::Show))
   {
      ///
      /// Try to restore the geometry. If no geometry is available try to restore the
      /// default geometry.
      ///
      auto geometry = settings_->value(geometryKey(widget));
      if (geometry.isValid() == true)
      {
         widget->restoreGeometry(geometry.value<QByteArray>());
      }
      else
      {
         auto defaultGeometry = settings_->value(defaultGeometryKey(widget));
         if (defaultGeometry.isValid() == true)
         {
            widget->resize(defaultGeometry.value<QRect>().size());
            widget->move(defaultGeometry.value<QRect>().topLeft());
         }
      }
   }
   else if (/* auto hideEvent = */ eventAs<QHideEvent>(event, QEvent::Hide))
   {
      ///
      /// Store the current geometry.
      ///
      settings_->setValue(geometryKey(widget), widget->saveGeometry());
   }
   else if (/* auto destroyEvent = */ eventAs<QEvent>(event, QEvent::Destroy))
   {
      ///
      /// Remove the widget as it has been destroyed.
      ///
      removeWidget(widget);
   }

   return false;
}

QString GeometryStore::defaultGeometryKey(QWidget* widget)
{
   Q_ASSERT(widget != nullptr);
   Q_ASSERT(widget->metaObject() != nullptr);

   return (widget->metaObject()->className() + QStringLiteral("/defaultGeometry"));
}

QString GeometryStore::geometryKey(QWidget *widget)
{
   Q_ASSERT(widget != nullptr);
   Q_ASSERT(widget->metaObject() != nullptr);

   return (widget->metaObject()->className() + QStringLiteral("/geometry"));
}
