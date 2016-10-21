/*!
 * \file importitemeditor.cpp
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
#include "importitem.h"
#include "importitemeditor.h"

ImportItemEditor::ImportItemEditor(QWidget* parent) : ItemEditor(parent)
{
   Application* application = static_cast<Application*>(Application::instance());

   fileEdit_ = new QLineEdit;

   auto getOpenFileNameAction = fileEdit_->addAction(QIcon(":/images/folder.png"), QLineEdit::TrailingPosition);
   getOpenFileNameAction->setVisible(true);
   getOpenFileNameAction->connect(getOpenFileNameAction, &QAction::triggered, [this]()
   {
      const auto& file = QFileDialog::getOpenFileName(this, QString(), QFileInfo(fileEdit_->text()).absolutePath());
      if (!file.isEmpty())
      {
         fileEdit_->setText(file);
      }
   });

   mimeTypeEdit_ = new QComboBox;
   mimeTypeEdit_->setEditable(true);
   mimeTypeEdit_->addItems(application->itemSourceFactory()->mimeTypes());

   auto layout = new QFormLayout;
   layout->addRow(tr("File"), fileEdit_);
   layout->addRow(tr("Mime Type"), mimeTypeEdit_);

   setLayout(layout);
}

void ImportItemEditor::read(Item* item)
{
   ImportItem* importItem = Item::cast<ImportItem>(item);
   if (importItem != nullptr)
   {
      fileEdit_->setText(importItem->file());
      mimeTypeEdit_->setCurrentText(importItem->mimeType());
   }
}

void ImportItemEditor::write(Item* item)
{
   ImportItem* importItem = Item::cast<ImportItem>(item);
   if (importItem != nullptr)
   {
      importItem->setFile(fileEdit_->text());
      importItem->setMimeType(mimeTypeEdit_->currentText());
   }
}
