/*!
 * \file sourceerror.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef SOURCEERROR_H
#define SOURCEERROR_H

#include <QString>

#include "sourceposition.h"

/*!
 * \todo Document.
 */
class SourceError
{
public:
   /*!
    * \brief The type of the error.
    */
   enum class Type
   {
      None,
      OpenError,
      ParseError
   };

   /*!
    * Constructs a source error with the type \a type, the text \a text and the position \a position.
    */
   explicit SourceError(Type type = Type::None, const QString& text = QString(), SourcePosition position = SourcePosition());

   /*!
    * Sets the type to \a type.
    */
   void setType(const Type& type);
   /*!
    * Returns the type.
    */
   Type type() const;

   /*!
    * Sets the text to \a text.
    */
   void setText(const QString& text);
   /*!
    * Returns the text.
    */
   QString text() const;

   /*!
    * Sets the source position to \a position.
    */
   void setPosition(const SourcePosition& position);
   /*!
    * Returns the source position.
    */
   SourcePosition position() const;

private:
   /*!
    * The type of the error.
    */
   Type type_;

   /*!
    * The text describing the error.
    */
   QString text_;

   /*!
    * The position of the error.
    */
   SourcePosition position_;
};

#endif // SOURCEERROR_H

