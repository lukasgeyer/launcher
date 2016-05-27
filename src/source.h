/*!
 * \file source.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef SOURCE_H
#define SOURCE_H

#include <QString>

#include "import.h"
#include "imports.h"
#include "item.h"
#include "items.h"
#include "sourceerror.h"

class Source
{
public:
   /*!
    * Constructs a source for the file name \a file.
    */
   explicit Source(const QString& file = QString());

   /*!
    * Sets the file name of the source to \a file.
    */
   void setFile(const QString& file);
   /*!
    * Returns the file name of the source.
    */
   QString file() const;

   /*!
    * Sets the error for this source to \a error.
    */
   void setError(const SourceError& error);
   /*!
    * Returns the error for this source.
    */
   SourceError error() const;

   /*!
    * Appends the source item \a item to the list of items in this source.
    */
   void appendItem(const Item& item);
   /*!
    * Sets the list of item in this source to \a sourceItems.
    */
   void setItems(const Items& items);
   /*!
    * Returns the list of items in this source.
    */
   Items items() const;

   /*!
    * Appends the import \a import to the list of imports in this source.
    */
   void appendImport(const Import& import);
   /*!
    * Sets the list of imports to \a imports
    */
   void setImports(const Imports& imports);
   /*!
    * Returns the list of imports in this source.
    */
   Imports imports() const;

private:
   /*!
    * The file of the source.
    */
   QString file_;

   /*!
    * The error asscociated with this source.
    */
   SourceError error_;

   /*!
    * The list of items in this source.
    */
   Items items_;
   /*!
    * The list of imports in this source.
    */
   Imports imports_;
};

#endif // SOURCE_H
