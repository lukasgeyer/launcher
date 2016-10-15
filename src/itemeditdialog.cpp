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

#include "application.h"
#include "geometrystore.h"
#include "importitem.h"
#include "importitemeditor.h"
#include "itemeditor.h"
#include "itemeditdialog.h"
#include "itemmodel.h"
#include "linkitem.h"
#include "linkitemeditor.h"

namespace {

/*!
 * \brief An invalid item editor.
 */
class InvalidItemEditor_ : public ItemEditor
{
public:
   /*!
    * \reimp
    */
   inline void read(Item * /* item */) override
   {
   }
   /*!
    * \reimp
    */
   inline void write(Item * /* item */) override
   {
   }
};

} // namespace

ItemEditDialog::ItemEditDialog(ItemModel* itemModel, QWidget* parent, Qt::WindowFlags windowFlags) : QDialog(parent, windowFlags | Qt::Tool)
{
   Q_ASSERT(itemModel != nullptr);

   Application* application = static_cast<Application*>(Application::instance());

   //
   // Setup the item editor stack.
   //
   editorLayout_ = new QStackedLayout;
   editorLayout_->insertWidget(static_cast<int>(Item::Type::Invalid), new InvalidItemEditor_);
   for (auto type : application->itemEditorFactory()->types())
   {
      editorLayout_->insertWidget(static_cast<int>(type), application->itemEditorFactory()->create(type));
   }

   //
   // Set up the item view.
   //
   auto itemView = new QTreeView;
   itemView->setHeaderHidden(true);
   itemView->setModel(itemModel);
   itemView->expandAll();
   itemView->setContextMenuPolicy(Qt::CustomContextMenu);
   itemView->connect(itemView, &QTreeView::customContextMenuRequested, [this, application, itemModel, itemView](const QPoint& point)
   {
      //
      // Get the item the context menu has been requested for.
      //
      Item* item = itemModel->item(itemView->indexAt(point));
      if (item != nullptr)
      {
         QMenu contextMenu;

         //
         // If the item is a group item check if it can serve as container for a type supported by
         // an editor and offer an action to create such an item.
         //
         if (auto groupItem = Item::cast<GroupItem>(item))
         {
            for (auto type : application->itemEditorFactory()->types())
            {
               if (groupItem->isContainerOf(type))
               {
                  contextMenu.addAction(tr("Add ").append(application->itemFactory()->typeName(type)), [application, groupItem, type]()
                  {
                     groupItem->appendItem(application->itemFactory()->create(type));
                  });
               }
            }
         }

         contextMenu.addAction(tr("Remove Item"), [item]()
         {
            //< CONTINUE HERE: Remove Item
         });

         contextMenu.exec(itemView->mapToGlobal(point));
      }
   });
   itemView->connect(itemView->selectionModel(), &QItemSelectionModel::currentChanged,
                     [this, itemModel](const QModelIndex& currentIndex, const QModelIndex& previousIndex)
   {
      //
      // Write the previous item.
      //
      Item* previousItem = itemModel->item(previousIndex);
      if (auto editor = static_cast<ItemEditor*>(editorLayout_->currentWidget()))
      {
         editor->write(previousItem);
      }

      //
      // Display the editor for the item type and read the item into the editor. If there is no
      // editor available for that type show the default editor.
      //
      Item* currentItem = itemModel->item(currentIndex);
      if (auto editor = static_cast<ItemEditor*>(editorLayout_->widget(static_cast<int>(currentItem->type()))))
      {
         editor->read(currentItem);

         editorLayout_->setCurrentWidget(editor);
      }
      else
      {
         editorLayout_->setCurrentIndex(static_cast<int>(Item::Type::Invalid));
      }
   });

   //
   // Set up the layout.
   //
   auto layout = new QHBoxLayout;
   layout->addWidget(itemView);
   layout->addLayout(editorLayout_);

   setLayout(layout);

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
