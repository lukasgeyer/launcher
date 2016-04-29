/*!
 * \file geometrystore.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QObject>

#ifndef GEOMETRYSTORE_H
#define GEOMETRYSTORE_H

class QWidget;
class QSettings;

/*!
 * \brief A store automatically storing and restoring geometry for widgets.
 */
class GeometryStore : public QObject
{
public:
   /*!
    * Creates a GeometryStore with the parent \a parent. The store is using \a settings to store
    * and restore the geometry.
    */
   GeometryStore(QSettings* settings, QObject* parent = nullptr);

   /*!
    * Adds the widget \a widget to the geometry store.
    */
   void addWidget(QWidget* widget, const QRect& defaultGeometry);
   /*!
    * Removes the widget with the ID \a id from the geometry store.
    */
   void removeWidget(QWidget* widget);

private:
   /*!
    * The settings to be used to store and restore geometry.
    */
   QSettings* settings_;

   /*!
    * \reimp
    */
   bool eventFilter(QObject* object, QEvent* event) override;

   /*!
    * Returns the key to be used to store and restore default geometry for the widget \a widget.
    */
   static QString defaultGeometryKey(QWidget* widget);
   /*!
    * Returns the key to be used to store and restore geometry for the widget \a widget.
    */
   static QString geometryKey(QWidget* widget);
};

#endif // GEOMETRYSTORE_H
