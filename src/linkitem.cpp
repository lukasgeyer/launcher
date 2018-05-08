/*!
 * \file linkitem.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QRegularExpression>

#include "linkitem.h"

LinkItem::LinkItem() : Item(Item::Type::Link)
{
}

void LinkItem::setName(const QString& name)
{
   name_ = name;
}

QString LinkItem::name() const
{
   return name_;
}

void LinkItem::setLink(const QString& link)
{
   static thread_local QRegularExpression placeholderExpression(QStringLiteral("{([0-9]+|\\*)}"));

   link_ = applyProcessEnvironment(link);
   linkParameterCount_ = 0;
   linkParameterWildcard_ = false;

   auto placeholderMatches = placeholderExpression.globalMatch(link_);
   while (placeholderMatches.hasNext())
   {
      const auto& placeholderString = placeholderMatches.next().captured(1);

      if (placeholderString == "*")
      {
         linkParameterWildcard_ = true;
      }
      else
      {
         int placeholderNumber = placeholderString.toInt();

         if (linkParameterCount_ < (placeholderNumber + 1))
         {
            linkParameterCount_ = (placeholderNumber + 1);
         }
      }
   }
}

QString LinkItem::link() const
{
   return link_;
}

QString LinkItem::link(const QStringList& parameters)
{
   QString link = link_;

   if (linkParameterCount_ > 0)
   {
      for (int placeholderNumber = 0; placeholderNumber < linkParameterCount_; ++placeholderNumber)
      {
         link.replace((QStringLiteral("{") + QString::number(placeholderNumber) + QStringLiteral("}")), parameters.at(placeholderNumber));
      }
   }

   if (linkParameterWildcard_ == true)
   {
      link.replace(QStringLiteral("{*}"), parameters.join(' '));
   }

   return link;
}

LinkItem::LinkMatch LinkItem::linkMatches(const QStringList& parameters) const
{
   LinkMatch linkMatch = LinkMatch::SufficientParameters;

   if ((linkParameterWildcard_ == false) && (parameters.count() > linkParameterCount_))
   {
      linkMatch = LinkMatch::ExcessParameters;
   }
   else if ((linkParameterWildcard_ == false) && (parameters.count() < linkParameterCount_))
   {
      linkMatch = LinkMatch::InsufficientParameters;
   }

   return linkMatch;
}

void LinkItem::setBrush(const QBrush& brush)
{
   brush_ = brush;
}

QBrush LinkItem::brush() const
{
   return brush_;
}

void LinkItem::appendTag(const QString& tag)
{
   tags_.append(tag);
}

void LinkItem::setTags(const QStringList& tags)
{
   tags_ = tags;
}

const QStringList& LinkItem::tags() const
{
   return tags_;
}

QDebug operator<<(QDebug stream, const LinkItem& item)
{
   return (stream << item.name() << item.link() << item.brush() << item.tags());
}
