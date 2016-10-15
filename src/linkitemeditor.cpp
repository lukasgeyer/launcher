/*!
 * \file linkitemeditor.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QFormLayout>
#include <QLineEdit>

#include "linkitem.h"
#include "linkitemeditor.h"

namespace {

enum Widget_
{
   NameWidget_,
   UrlWidget_,
   ColorWidget_,
   TagsWidget_
};

/*!
 * Returns the widget \a widget of the form layout \a layout as type \a type.
 */
template <typename WidgetType> WidgetType* widget_(QLayout* layout, Widget_ widget)
{
   return static_cast<WidgetType*>(static_cast<QFormLayout*>(layout)->itemAt(widget, QFormLayout::FieldRole)->widget());
}

} // namespace

LinkItemEditor::LinkItemEditor(QWidget* parent) : ItemEditor(parent)
{
   auto layout = new QFormLayout;
   layout->insertRow(NameWidget_, tr("Name"), new QLineEdit);
   layout->insertRow(UrlWidget_, tr("Url"), new QLineEdit);
   layout->insertRow(ColorWidget_, tr("Color"), new QLineEdit);
   layout->insertRow(TagsWidget_, tr("Tag"), new QLineEdit);

   setLayout(layout);
}

void LinkItemEditor::read(Item* item)
{
   LinkItem* linkItem = Item::cast<LinkItem>(item);
   if (linkItem != nullptr)
   {
      widget_<QLineEdit>(layout(), NameWidget_)->setText(linkItem->name());
      widget_<QLineEdit>(layout(), UrlWidget_)->setText(linkItem->link());
      widget_<QLineEdit>(layout(), TagsWidget_)->setText(linkItem->tags().join(QStringLiteral(", ")));
      widget_<QLineEdit>(layout(), ColorWidget_)->setText(linkItem->brush().color().name());
   }
}

void LinkItemEditor::write(Item* item)
{
   LinkItem* linkItem = Item::cast<LinkItem>(item);
   if (linkItem != nullptr)
   {
      linkItem->setName(widget_<QLineEdit>(layout(), NameWidget_)->text());
      linkItem->setLink(widget_<QLineEdit>(layout(), UrlWidget_)->text());
      linkItem->setTags(widget_<QLineEdit>(layout(), TagsWidget_)->text().split(QStringLiteral(",")));

      const auto& color = widget_<QLineEdit>(layout(), ColorWidget_)->text();
      if (!color.isEmpty())
      {
         linkItem->setBrush(QBrush(QColor(color)));
      }

   }
}
