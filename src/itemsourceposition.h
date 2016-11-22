/*!
 * \file itemsourceposition.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEMSOURCEPOSITION_H
#define ITEMSOURCEPOSITION_H

/*!
 * \brief A position with an item source.
 */
class ItemSourcePosition
{
public:
   /*!
    * Constructs a source position with the line number \a lineNumber, the column number
    * \a columnNumber and the size \a size.
    */
   explicit ItemSourcePosition(int lineNumber = 0, int columnNumber = 0, int size = 0);

   /*!
    * Sets the line number to \a lineNumber.
    */
   void setLineNumber(int lineNumber);
   /*!
    * Returns the line number.
    */
   int lineNumber() const;
   /*!
    * Sets the column number \a columnNumber.
    */
   void setColumnNumber(int columnNumber);
   /*!
    * Returns the column number.
    */
   int columnNumber() const;

   /*!
    * Sets the size.
    */
   void setSize(int size);
   /*!
    * Returns the size.
    */
   int size() const;

private:
   /*!
    * The line number.
    */
   int lineNumber_;
   /*!
    * The column number.
    */
   int columnNumber_;
   /*!
    * The size.
    */
   int size_;
};

#endif // ITEMSOURCEPOSITION_H
