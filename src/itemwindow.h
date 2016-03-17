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
    * Is \a true if the window is being dragged; \a false otherwise.
    */
   bool windowDrag_ = false;
   /*!
    * Is \a true if the required modifier for window dragging is active; \a false otherwise.
    */
   bool windowDragModifier_ = false;
   /*!
    * The origin of the window drag operation.
    */
   QPoint windowDragOrigin_ = {};

   /*!
    * Is \a true if the window is being resized; \a false otherwise.
    */
   bool windowResize_ = false;
   /*!
    * Is \a true if the required modifier for window resizing is active; \a false otherwise.
    */
   bool windowResizeModifier_ = false;
   /*!
    * The origin of the window resize operation.
    */
   QPoint windowResizeOrigin_ = {};

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
