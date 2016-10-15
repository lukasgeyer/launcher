/*!
 * \file itemeditdialog.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QDesktopWidget>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QMenu>
#include <QStackedLayout>
#include <QTreeView>
#include <QVBoxLayout>

#include "application.h"
#include "geometrystore.h"
#include "importitem.h"
#include "importitemeditor.h"
#include "itemeditdialog.h"
#include "itemmodel.h"
#include "linkitem.h"
#include "linkitemeditor.h"

ItemEditDialog::ItemEditDialog(ItemModel* model, QWidget* parent, Qt::WindowFlags windowFlags) : QDialog(parent, windowFlags | Qt::Tool)
{
   Q_ASSERT(model != nullptr);

   Application* application = static_cast<Application*>(Application::instance());

   editorLayout_ = new QStackedLayout;
   editorLayout_->insertWidget(static_cast<int>(Item::ItemType::Invalid), new QWidget);
   for (auto itemType : application->itemEditorFactory()->itemTypes())
   {
      editorLayout_->insertWidget(static_cast<int>(itemType), application->itemEditorFactory()->create(itemType));
   }

   auto itemView = new QTreeView;
   itemView->setHeaderHidden(true);
   itemView->setModel(model);
   itemView->expandAll();
   itemView->connect(itemView->selectionModel(), &QItemSelectionModel::currentChanged, [this, model](const QModelIndex& current, const QModelIndex&)
   {
      ///
      /// Display the editor for the item type and read the item into the editor. If there is no
      /// editor available for that type show the default editor.
      ///
      Item* item = model->item(current);
      if (auto editor = static_cast<ItemEditor*>(editorLayout_->widget(static_cast<int>(item->type()))))
      {
         editor->read(item);

         editorLayout_->setCurrentWidget(editor);
      }
      else
      {
         editorLayout_->setCurrentIndex(static_cast<int>(Item::ItemType::Invalid));
      }
   });

   auto dialogButtonBox = new QDialogButtonBox;
   dialogButtonBox->addButton(tr("Close"), QDialogButtonBox::AcceptRole);
   dialogButtonBox->connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);

   auto horizontalLayout = new QHBoxLayout;
   horizontalLayout->addWidget(itemView);
   horizontalLayout->addLayout(editorLayout_);

   auto veritcalLayout = new QVBoxLayout;
   veritcalLayout->addLayout(horizontalLayout);
   veritcalLayout->addWidget(dialogButtonBox);

   setLayout(veritcalLayout);

   setWindowTitle(tr("Item Editor"));

   //
   // Register the widget with the geomerty store.
   //
   auto screenGeometry = QApplication::desktop()->screenGeometry();

   QSize defaultSize(screenGeometry.width() * 0.5, screenGeometry.height() * 0.5);
   QPoint defaultPosition((screenGeometry.right() - defaultSize.width()) / 2,
                          (screenGeometry.bottom() - defaultSize.height()) / 2);

   application->geometryStore()->addWidget(this, QRect(defaultPosition, defaultSize));
}
