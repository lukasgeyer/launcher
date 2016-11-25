/*!
 * \file searchbarwidget.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QDebug>
#include <QIcon>
#include <QKeyEvent>

#include "searchbarwidget.h"

SearchBarWidget::SearchBarWidget(QWidget *parent) : QLineEdit(parent)
{
   indicationsAction_ = addAction(QIcon(QStringLiteral(":/images/error.png")), QLineEdit::TrailingPosition);
   indicationsAction_->setVisible(false);
   indicationsAction_->connect(indicationsAction_, &QAction::triggered, this, &SearchBarWidget::removeIndications);
}

void SearchBarWidget::addInidication(const QString& id, const QString& indication)
{
   indications_.insert(id, indication);

   updateIndicationsAction_();
}

void SearchBarWidget::removeIndication(const QString& id)
{
   if (id.endsWith('*'))
   {
      //
      // If the ID ends in an asterisk wildcard matching shall occur and any indication, whose
      // ID starts with the string preceding the asterisk, shall be removed.
      //
      auto idWithoutWildcard = id.leftRef(id.size() - 1 /* '*' */);

      bool indicationMatches = false;
      for (auto indication = std::begin(indications_); indication != std::end(indications_); )
      {
         if (indication.key().startsWith(idWithoutWildcard))
         {
            indicationMatches = true;
            indication = indications_.erase(indication);
         }
         else
         {
            ++indication;
         }
      }

      if (indicationMatches )
      {
         updateIndicationsAction_();
      }
   }
   else
   {
      //
      // If the ID does not end in an asterisk an exact match is required.
      //
      auto indication = indications_.find(id);
      if (indication != indications_.end())
      {
         indications_.erase(indication);

         updateIndicationsAction_();
      }
   }
}

void SearchBarWidget::removeIndications()
{
   indications_.clear();

   updateIndicationsAction_();
}

void SearchBarWidget::updateIndicationsAction_()
{
   if (indications_.isEmpty())
   {
      //
      // If there are no indications hide the indication action.
      //
      indicationsAction_->setVisible(false);
   }
   else
   {
     //
     // If there are indications in the indications list display the error indication and the
     // list of indications as tooltip for that action.
     //
     QString indicationsActionTooltip;
     for (const auto& indication : indications_)
     {
        indicationsActionTooltip.append(indication);
        indicationsActionTooltip.append("\n");
     }
     indicationsActionTooltip.chop(1 /* '\n' */);

     indicationsAction_->setToolTip(indicationsActionTooltip);
     indicationsAction_->setVisible(true);
  }
}

bool SearchBarWidget::event(QEvent* event)
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
      case Qt::Key_PageUp:
         emit keyPageUpPressed();
         break;
      case Qt::Key_PageDown:
         emit keyPageDownPressed();
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
