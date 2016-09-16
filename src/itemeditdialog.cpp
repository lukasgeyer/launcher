/*!
 * \file itemeditdialog.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QStackedLayout>
#include <QTreeView>
#include <QVBoxLayout>

#include "importitem.h"
#include "itemeditdialog.h"
#include "itemmodel.h"
#include "linkitem.h"

ItemEditDialog::ItemEditDialog(ItemModel* model, QWidget* parent, Qt::WindowFlags windowFlags) : QDialog(parent, windowFlags)
{
   Q_ASSERT(model != nullptr);

   auto linkItemEditorLayout = new QFormLayout;
   linkItemEditorLayout->addRow("Link Edit", new QLineEdit);

   auto linkItemEditorWidget = new QWidget;
   linkItemEditorWidget->setLayout(linkItemEditorLayout);

   auto importItemEditorLayout = new QFormLayout;
   importItemEditorLayout->addRow("Import Edit", new QLineEdit);

   auto importItemEditorWidget = new QWidget;
   importItemEditorWidget->setLayout(importItemEditorLayout);

   enum { LinkItemEditor, ImportItemEditor };

   editorLayout_ = new QStackedLayout;
   editorLayout_->addWidget(linkItemEditorWidget);
   editorLayout_->addWidget(importItemEditorWidget);

   auto itemView = new QTreeView;
   itemView->setHeaderHidden(true);
   itemView->setModel(model);
   itemView->connect(itemView, &QAbstractItemView::clicked, [this, model](const QModelIndex& index)
   {
      if (model->item(index)->cast<LinkItem>())
      {
         editorLayout_->setCurrentIndex(LinkItemEditor);
      }
      else if (model->item(index)->cast<ImportItem>())
      {
         editorLayout_->setCurrentIndex(ImportItemEditor);
      }
   });

   auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Discard);

   auto layout = new QVBoxLayout;
   layout->addWidget(itemView, 1);
   layout->addLayout(editorLayout_);
   layout->addWidget(dialogButtonBox);

   setLayout(layout);
}
