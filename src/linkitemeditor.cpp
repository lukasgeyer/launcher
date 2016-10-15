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
   TagWidget_
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
   layout->insertRow(TagWidget_, tr("Tag"), new QLineEdit);

   setLayout(layout);
}

void LinkItemEditor::read(Item* item)
{
   LinkItem* linkItem = static_cast<LinkItem*>(item);

   widget_<QLineEdit>(layout(), NameWidget_)->setText(linkItem->name());
   widget_<QLineEdit>(layout(), UrlWidget_)->setText(linkItem->link());
   widget_<QLineEdit>(layout(), ColorWidget_)->setText(linkItem->brush().color().name());
   widget_<QLineEdit>(layout(), TagWidget_)->setText(linkItem->tags().join(QStringLiteral(", ")));
}

void LinkItemEditor::write(Item* item)
{

}
