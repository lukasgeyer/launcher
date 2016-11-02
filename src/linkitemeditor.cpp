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

LinkItemEditor::LinkItemEditor(QWidget* parent) : ItemEditor(parent)
{
   auto layout = new QFormLayout;
   layout->addRow(tr("Name"), nameEdit_ = new QLineEdit);
   layout->addRow(tr("Url"), urlEdit_ = new QLineEdit);
   layout->addRow(tr("Tags"), tagEdit_ = new QLineEdit);
   layout->addRow(tr("Color"), colorEdit_= new QLineEdit);

   setLayout(layout);
}

void LinkItemEditor::read(Item* item)
{
   LinkItem* linkItem = Item::cast<LinkItem>(item);
   if (linkItem != nullptr)
   {
      nameEdit_->setText(linkItem->name());
      urlEdit_->setText(linkItem->link());
      tagEdit_->setText(linkItem->tags().join(QStringLiteral(", ")));
      colorEdit_->setText(linkItem->brush().color().name());
   }
}

void LinkItemEditor::write(Item* item)
{
   LinkItem* linkItem = Item::cast<LinkItem>(item);
   if (linkItem != nullptr)
   {
      linkItem->setName(nameEdit_->text());
      linkItem->setLink(urlEdit_->text());
      linkItem->setTags(tagEdit_->text().split(QStringLiteral(",")));

      if (!colorEdit_->text().isEmpty())
      {
         linkItem->setBrush(QBrush(QColor(colorEdit_->text())));
      }

   }
}
