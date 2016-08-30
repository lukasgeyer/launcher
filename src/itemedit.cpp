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

#include "indication.h"
#include "itemedit.h"

ItemEdit::ItemEdit(QWidget *parent) : QLineEdit(parent)
{
   indicationAction_ = addAction(QIcon(QStringLiteral(":/images/error.png")), QLineEdit::TrailingPosition);
   indicationAction_->setVisible(false);
   indicationAction_->connect(indicationAction_, &QAction::triggered, [this](){
      removeIndications();
   });
}

void ItemEdit::addInidication(const QString& id, Indication* indication)
{
   Q_ASSERT(indication != nullptr);

   qDebug() << "add indication" << id << indication->metaObject()->className();

   emit indicationAdded(id, *indication);

   indications_.insert(id, indication);

   updateIndicationAction_();
}

void ItemEdit::removeIndication(const QString& id)
{
   qDebug() << "remove indication" << id;

   auto indication = indications_.find(id);
   if (indication != indications_.end())
   {
      emit indicationRemoved(indication.key(), *indication.value());

      delete indication.value();

      indications_.erase(indication);
   }

   updateIndicationAction_();
}

void ItemEdit::removeIndications()
{
   qDebug() << "remove indications";

   for(auto indication = indications_.begin(); indication != indications_.end(); )
   {
      emit indicationRemoved(indication.key(), *indication.value());

      delete indication.value();

      indication = indications_.erase(indication);
   }

   updateIndicationAction_();
}

void ItemEdit::updateIndicationAction_()
{
   Q_ASSERT(indicationAction_ != nullptr);

   if (indications_.isEmpty() == false)
   {
      //
      // If there are indications in the indications list display the error indication and the
      // list of indications as tooltip for that action.
      //
      QString indicationActionTooltip;
      for (const auto& indication : indications_)
      {
         indicationActionTooltip.append(indication->text());
         indicationActionTooltip.append("\n");
      }
      indicationActionTooltip.chop(1 /* '\n' */);

      indicationAction_->setToolTip(indicationActionTooltip);
      indicationAction_->setVisible(true);
   }
   else
   {
      //
      // If there are no indications hide the indication action.
      //
      indicationAction_->setVisible(false);
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
