/*!
 * \file itemedit.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QIcon>
#include <QKeyEvent>

#include "itemedit.h"

ItemEdit::ItemEdit(QWidget *parent) : QLineEdit(parent)
{
   errorIndication_ = addAction(QIcon(QStringLiteral(":/images/error.png")), QLineEdit::TrailingPosition);
   errorIndication_->setVisible(false);
}

void ItemEdit::addError(const QString& errorId, const QString& error)
{
   errors_.insert(errorId, error);

   updateErrorIndication_();
}

void ItemEdit::removeError(const QString& errorId)
{
   errors_.remove(errorId);

   updateErrorIndication_();
}

void ItemEdit::updateErrorIndication_()
{
   Q_ASSERT(errorIndication_ != nullptr);

   if (errors_.isEmpty() == false)
   {
      ///
      /// If there are errors in the error list display the error indication and the list of errors
      /// as tooltip for that action.
      ///
      QString errorIndicationToolTip;
      for (const auto& error : errors_)
      {
         errorIndicationToolTip.append(error);
         errorIndicationToolTip.append("\n");
      }
      errorIndicationToolTip.chop(1 /* '\n' */);

      errorIndication_->setToolTip(errorIndicationToolTip);
      errorIndication_->setVisible(true);
   }
   else
   {
      ///
      /// If there are no errors hide the error indication.
      ///
      errorIndication_->setVisible(false);
   }
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
