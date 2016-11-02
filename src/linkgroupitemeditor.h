/*!
 * \file linkgroupitemeditor.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef LINKGROUPITEMEDITOR_H
#define LINKGROUPITEMEDITOR_H

#include "itemeditor.h"

class QComboBox;
class QLineEdit;

/*!
 * \brief An group item editor.
 */
class LinkGroupItemEditor : public ItemEditor
{
public:
   /*!
    * Constructs an group item editor with the parent \a parent.
    */
   LinkGroupItemEditor(QWidget* parent = nullptr);

   /*!
    * \reimp
    */
   void read(Item* item) override;
   /*!
    * \reimp
    */
   void write(Item* item) override;

private:
   /*!
    * The widgets displaying the group item.
    */
   QLineEdit* nameEdit_ = nullptr;
   QLineEdit* tagEdit_ = nullptr;
   QLineEdit* colorEdit_ = nullptr;
};

#endif // LINKGROUPITEMEDITOR_H
