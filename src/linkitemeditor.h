/*!
 * \file linkitemeditor.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef LINKITEMEDITOR_H
#define LINKITEMEDITOR_H

#include "itemeditor.h"

class QLineEdit;

/*!
 * \brief A link item editor.
 */
class LinkItemEditor : public ItemEditor
{
public:
   /*!
    * Constructs a link item editor with the parent \a parent.
    */
   LinkItemEditor(QWidget* parent = nullptr);

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
    * The widgets displaying the link item.
    */
   QLineEdit* nameEdit_ = nullptr;
   QLineEdit* urlEdit_ = nullptr;
   QLineEdit* tagEdit_ = nullptr;
   QLineEdit* colorEdit_ = nullptr;
};

#endif // LINKITEMEDITOR_H
