/*!
 * \file itemedit.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEMEDIT_H
#define ITEMEDIT_H

#include <QAction>
#include <QEvent>
#include <QLineEdit>
#include <QString>
#include <QVector>

/*!
 * \brief A line edit providing additional up-key, down-key or escape-key pressed signals.
 */
class ItemEdit : public QLineEdit
{
   Q_OBJECT

public:
   /*!
    * Constructs an ItemEdit with the parent \a parent.
    */
   ItemEdit(QWidget* parent = nullptr);

signals:
   /*!
    * Is emitted when the up-key is pressed.
    */
   void keyUpPressed();
   /*!
    * Is emitted when the down-key is pressed.
    */
   void keyDownPressed();
   /*!
    * Is emitted when the escape-key is pressed.
    */
   void escapePressed();

private:
   /*!
    * \reimp
    */
   bool event(QEvent* event);
};

#endif // ITEMEDIT_H
