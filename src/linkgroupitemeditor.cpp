/*!
 * \file linkgroupitemeditor.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QAction>
#include <QComboBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QLineEdit>

#include "application.h"
#include "linkgroupitem.h"
#include "linkgroupitemeditor.h"

LinkGroupItemEditor::LinkGroupItemEditor(QWidget* parent) : ItemEditor(parent)
{
   auto layout = new QFormLayout;
   layout->addRow(tr("Name"), nameEdit_ = new QLineEdit);
   layout->addRow(tr("Tags"), tagEdit_ = new QLineEdit);
   layout->addRow(tr("Color"), colorEdit_= new QLineEdit);

   setLayout(layout);
}

void LinkGroupItemEditor::read(Item* item)
{
   LinkGroupItem* linkGroupItem = Item::cast<LinkGroupItem>(item);
   if (linkGroupItem != nullptr)
   {
      nameEdit_->setText(linkGroupItem->name());
      tagEdit_->setText(linkGroupItem->tags().join(QStringLiteral(", ")));
      colorEdit_->setText(linkGroupItem->brush().color().name());
   }
}

void LinkGroupItemEditor::write(Item* item)
{
   LinkGroupItem* linkGroupItem = Item::cast<LinkGroupItem>(item);
   if (linkGroupItem != nullptr)
   {
      linkGroupItem->setName(nameEdit_->text());
      linkGroupItem->setTags(tagEdit_->text().split(QStringLiteral(",")));

      if (!colorEdit_->text().isEmpty())
      {
         linkGroupItem->setBrush(QBrush(QColor(colorEdit_->text())));
      }

   }
}
