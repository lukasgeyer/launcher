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

protected:
   /*!
    * \reimp
    */
   void closeEvent(QCloseEvent* event);

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
    * Opens the url \a url with the default application.
    */
   void openUrl_(const QUrl& url);
};

#endif // ITEMWINDOW_H
