/*!
 * \file itemeditor.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEMEDITOR_H
#define ITEMEDITOR_H

#include <QWidget>

class Item;

/*!
 * \brief An abstract item editor.
 */
class ItemEditor : public QWidget
{
public:
   /*!
    * Constructs an item editor with the parent \a parent.
    */
   ItemEditor(QWidget* parent = nullptr) : QWidget(parent)
   {
   }

   /*!
    * Reads the item \a item into the editor.
    */
   virtual void read(Item* item) = 0;
   /*!
    * Writes the editor data to the item \a item.
    */
   virtual void write(Item *item) = 0;
};

#endif // ITEMEDITOR_H
