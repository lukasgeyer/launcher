/*!
 * \file itemwindow.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEMWINDOW_H
#define ITEMWINDOW_H

#include <QPoint>
#include <QWidget>

#include "itemmodel.h"

class Indicator;
class ItemEdit;
class ItemView;

/*!
 * \brief An item window, listing all items, filtered by name and tags.
 */
class ItemWindow : public QWidget
{
   Q_OBJECT

public:
   /*!
    * Constructs an ItemWindow with the parent \a parent.
    */
   ItemWindow(QWidget* parent = nullptr);
   /*!
    * Destructs the ItemWindow.
    */
   ~ItemWindow();

private:
   /*!
    * The type of window modification currently active.
    */
   enum class WindowModificationOperation
   {
      None, /*< No window modification is currently performed. */
      Move, /*< The window is currently moved. */
      Resize /*< The window is currently resized. */
   };
   /*!
    * The current window modifcation.
    */
   WindowModificationOperation windowModificationOperation_ = WindowModificationOperation::None;
   /*!
    * Is \a true if the window modification operation is active.
    */
   bool windowModificationOperationActive_ = false;
   /*!
    * Is \a true if the required modifier for window modification is active.
    */
   bool windowModificationModifierActive_ = false;
   /*!
    * The origin of the window modification.
    */
   QPoint windowModificationOrigin_ = {};

   /*!
    * \reimp
    */
   bool eventFilter(QObject* object, QEvent* event) override;

   /*!
    * Tries to open a source editor for the source \a source and returns \a true if the source
    * can be opened; \a false otherwise and the error will be indicated at the error indication
    * \a errorIndication.
    * */
   bool openSource_(const QString& source, SourcePosition position, Indicator *errorIndicator = nullptr);
   /*!
    * Tries to open the URL \a url with the default application and returns \a true if the URL
    * can be opened; \a false otherwise and the error will be indicated at the error indication
    * \a errorIndication.
    */
   bool openUrl_(const QUrl& url, Indicator *errorIndicator = nullptr);
};

#endif // ITEMWINDOW_H
