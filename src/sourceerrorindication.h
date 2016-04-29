/*!
 * \file sourceerrorindication.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef SOURCEERRORINDICATION_H
#define SOURCEERRORINDICATION_H

#include <QString>

#include "indication.h"
#include "sourceposition.h"

/*!
 * \brief An indication for a source error.
 */
class SourceErrorIndication : public Indication
{
   Q_OBJECT

public:
   /*!
    * Constructs an SourceErrorIndication with the ID \a id, the text \a text and the source
    * position \a sourcePosition.
    */
   SourceErrorIndication(const QString& text = QString(),
                         const QString& source = QString(), const SourcePosition& sourcePosition = SourcePosition());

   /*!
    * Sets the source to \A source.
    */
   void setSource(const QString& source);
   /*!
    * Returns the source.
    */
   QString source() const;

   /*!
    * Sets the source position to \a sourcePosition.
    */
   void setSourcePosition(const SourcePosition& sourcePosition);
   /*!
    * Returns the source position.
    */
   SourcePosition sourcePosition() const;

private:
   /*!
    * The source.
    */
   QString source_;
   /*!
    * The source position.
    */
   SourcePosition sourcePosition_;
};

#endif // SOURCEERRORINDICATION_H
