/*!
 * \file importitemeditor.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef IMPORTITEMEDITOR_H
#define IMPORTITEMEDITOR_H

#include "itemeditor.h"

/*!
 * \brief An import item editor.
 */
class ImportItemEditor : public ItemEditor
{
public:
   /*!
    * Constructs an import item editor with the parent \a parent.
    */
   ImportItemEditor(QWidget* parent = nullptr);

   /*!
    * \reimp
    */
   void read(Item* item) override;
   /*!
    * \reimp
    */
   void write(Item* item) override;
};

#endif // LINKITEMEDITOR_H
