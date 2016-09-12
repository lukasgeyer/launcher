/*!
 * \file itemedit.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QDebug>
#include <QIcon>
#include <QKeyEvent>

#include "itemedit.h"

ItemEdit::ItemEdit(QWidget *parent) : QLineEdit(parent)
{
}

bool ItemEdit::event(QEvent* event)
{
   if (event->type() == QEvent::KeyPress)
   {
      QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
      switch (keyEvent->key())
      {
      case Qt::Key_Up:
         emit keyUpPressed();
         break;
      case Qt::Key_Down:
         emit keyDownPressed();
         break;
      case Qt::Key_Escape:
         emit escapePressed();
         break;
      default:
         break;
      }
   }

   return QLineEdit::event(event);
}
