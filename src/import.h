/*!
 * \file import.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef IMPORT_H
#define IMPORT_H

#include <QDebug>
#include <QString>

/*!
 * An import as found in a source.
 */
class Import
{
public:
   /*!
    * Constructs an import for the file \a file.
    */
   explicit Import(const QString& file = QString());

   /*!
    * Sets the file of this import to \a file.
    */
   void setFile(const QString& file);
   /*!
    * Returns the file of this import.
    */
   QString file() const;

private:
   /*!
    * The file of this import.
    */
   QString file_;

   /*!
    * Returns \a true if \a lhs and \a rhs are equal (refer to the same file); \a false otherwise.
    */
   friend bool operator==(const Import& lhs, const Import& rhs);
   /*!
    * Returns \a false if \a lhs and \a rhs are equal (refer to the same file); \a true otherwise.
    */
   friend bool operator!=(const Import& lhs, const Import& rhs);

   /*!
    * Inserts the item \a item into the stream \a stream and returns the stream.
    */
   friend QDebug operator<<(QDebug stream, const Import &import);
};


#endif // IMPORT_H
