/*!
 * \file itemeditdialog.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QBoxLayout>
#include <QDesktopWidget>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QMenu>
#include <QSplitter>
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

   ItemFactory* itemFactory = application->itemFactory();
   ItemEditorFactory* itemEditorFactory = application->itemEditorFactory();

   //
   // Setup the item editor stack.
   //
   editorLayout_ = new QStackedLayout;
   editorLayout_->insertWidget(static_cast<int>(Item::Type::Invalid), new InvalidItemEditor_);
   for (auto type : itemEditorFactory->types())
   {
      editorLayout_->insertWidget(static_cast<int>(type), itemEditorFactory->create(type));
   }

   auto editorWidget = new QGroupBox;
   editorWidget->setLayout(editorLayout_);

   //
   // Set up the item view.
   //
   auto itemView = new QTreeView;
   itemView->setHeaderHidden(true);
   itemView->setModel(itemModel);
   itemView->expandAll();
   itemView->setContextMenuPolicy(Qt::CustomContextMenu);
   itemView->connect(itemView, &QTreeView::customContextMenuRequested, [this, itemFactory, itemEditorFactory, itemModel, itemView](const QPoint& point)
   {
      QModelIndex requestedItemIndex = itemView->indexAt(point);

      Item* requestedItem = itemModel->item(requestedItemIndex);
      if (requestedItem != nullptr)
      {
         QMenu contextMenu;

         //
         // If the item is a group item check if it can serve as container for a type supported by
         // an editor and offer an action to create such an item.
         //
         if (auto requestedGroupItem = Item::cast<GroupItem>(requestedItem))
         {
            QModelIndex index = itemModel->index(0, requestedItemIndex.column(), requestedItemIndex);

            for (auto type : itemEditorFactory->types())
            {
               if (requestedGroupItem->isContainerOf(type))
               {
                  contextMenu.addAction(QIcon(QStringLiteral(":/images/add.png")), tr("Add ").append(itemFactory->typeName(type)),
                                        [itemFactory, itemModel, itemView, requestedItem, index, type]()
                                        {
                                           itemModel->insertItem(itemFactory->create(type), index.row(), index.parent());
                                           itemView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
                                        });
               }
            }
         }

         //
         // An item of the same type can be added as long as it is not an item source (which are
         // added through adding an import item). The same applies for removing an item.
         //
         if (!Item::is<ItemSource>(requestedItem))
         {
            QModelIndex index = itemModel->index(requestedItemIndex.row() + 1, requestedItemIndex.column(), requestedItemIndex.parent());

            contextMenu.addAction(QIcon(QStringLiteral(":/images/add.png")), tr("Add ").append(itemFactory->typeName(requestedItem->type())),
                                  [itemFactory, itemModel, itemView, requestedItem, index]()
                                  {
                                     itemModel->insertItem(itemFactory->create(requestedItem->type()), index.row(), index.parent());
                                     itemView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
                                  });

            contextMenu.addAction(QIcon(QStringLiteral(":/images/remove.png")), tr("Remove Item"), [itemModel, requestedItemIndex]()
            {
               itemModel->removeItem(requestedItemIndex);
            });
         }

         contextMenu.exec(itemView->mapToGlobal(point));
      }
   });
   itemView->connect(itemView->selectionModel(), &QItemSelectionModel::selectionChanged,
                     [this, itemModel](const QItemSelection& selectedItems, const QItemSelection& deselectedItems)
   {
      for (const auto& deselectedItem : deselectedItems.indexes())
      {
         Item* previousItem = itemModel->item(deselectedItem);
         if (auto editor = static_cast<ItemEditor*>(editorLayout_->currentWidget()))
         {
            editor->write(previousItem);
         }
      }

      for (const auto& selectedItem : selectedItems.indexes())
      {
         Item* currentItem = itemModel->item(selectedItem);
         if (auto editor = static_cast<ItemEditor*>(editorLayout_->widget(static_cast<int>(currentItem->type()))))
         {
            editor->read(currentItem);

            editorLayout_->setCurrentWidget(editor);
         }
         else
         {
            editorLayout_->setCurrentIndex(static_cast<int>(Item::Type::Invalid));
         }
      }
   });

   //
   // Set up the layout.
   //
   auto splitter = new QSplitter;
   splitter->addWidget(itemView);
   splitter->addWidget(editorWidget);

   auto layout = new QBoxLayout(QBoxLayout::LeftToRight);
   layout->addWidget(splitter);

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
