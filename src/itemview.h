/*!
 * \file itemview.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEMVIEW_H
#define ITEMVIEW_H

#include <QListView>

/*!
 * \brief A list view displaying a list of items.
 */
class ItemView : public QListView
{
public:
   /*!
    * Constructs an ItemView with the parent \a parent.
    */
   ItemView(QWidget* parent = nullptr);

   /*!
    * \reimp
    */
   QSize sizeHint() const override;
};

#endif // ITEMVIEW_H
