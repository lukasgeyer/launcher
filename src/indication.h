/*!
 * \file indication.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef INDICATION_H
#define INDICATION_H

#include <QObject>
#include <QString>

/*!
 * \brief An indication that can be shown on a widget.
 */
class Indication : public QObject
{
   Q_OBJECT

public:
   /*!
    * Constructs an Indication the text \a text.
    */
   Indication(const QString& text = QString());
   /*!
    * Destructs the indication.
    */
   virtual ~Indication();

   /*!
    * Sets the text of the indication to \a text.
    */
   void setText(const QString& text);
   /*!
    * Returns the text of the indication.
    */
   QString text() const;

private:
   /*!
    * The text of the indication.
    */
   QString text_;
};

#endif // INDICATION_H
