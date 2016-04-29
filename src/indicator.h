/*!
 * \file indicator.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef INDICATOR_H
#define INDICATOR_H

class Indication;

/*!
 * \brief An entity displaying indications.
 */
class Indicator
{
public:
   /*!
    * Adds the indication \a indication with the ID \a id to the indicator. The indicator takes
    * ownership of the indication.
    */
   virtual void addInidication(const QString& id, Indication* indication) = 0;

   /*!
    * Removes the indication with the ID \a id from the indicator.
    */
   virtual void removeIndication(const QString& id) = 0;
   /*!
    * Removes all indications from the indicator.
    */
   virtual void removeIndications() = 0;

signals:
   /*!
    * Is emitted when the indication \a indication with the ID \a id has been added.
    */
   void indicationAdded(const QString& id, const Indication& indication);
   /*!
    * Is emitted when the indication \a indication with the ID \a id has been removed.
    */
   void indicationRemoved(const QString& id, const Indication& indication);
};

#endif // INDICATOR_H
