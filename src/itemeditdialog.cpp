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
   for (std::size_t type = 0; type < static_cast<std::size_t>(Item::Type::Count); ++type)
   {
      auto itemEditor = itemEditorFactory->create(static_cast<Item::Type>(type));
      if (itemEditor == nullptr)
      {
         itemEditor = new InvalidItemEditor_;
      }

      editorLayout_->addWidget(itemEditor);
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

         for (auto type : itemEditorFactory->types())
         {
            QModelIndex index;

            GroupItem* requestedGroupItemParent = Item::cast<GroupItem>(requestedItem->parent());
            if ((requestedGroupItemParent != nullptr) && (requestedGroupItemParent->isContainerOf(type)))
            {
               index = itemModel->index(requestedItemIndex.row() + 1, requestedItemIndex.column(), requestedItemIndex.parent());
            }

            GroupItem* requestedGroupItem = Item::cast<GroupItem>(requestedItem);
            if ((requestedGroupItem != nullptr) && (requestedGroupItem->isContainerOf(type)))
            {
               index = itemModel->index(0, requestedItemIndex.column(), requestedItemIndex);
            }

            if (index.isValid())
            {
               contextMenu.addAction(QIcon(QStringLiteral(":/images/add.png")), tr("Add ").append(itemFactory->typeName(type)),
                                     [itemFactory, itemModel, itemView, index, type]()
                                     {
                                        itemModel->insertItem(itemFactory->create(type), index.row(), index.parent());
                                        itemView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
                                     });
            }
         }

         if (!Item::is<ItemSource>(requestedItem))
         {
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

      Item* currentItem = itemModel->item(selectedItems.indexes().value(0));
      if (auto editor = static_cast<ItemEditor*>(editorLayout_->widget(static_cast<int>(currentItem->type()))))
      {
         editor->read(currentItem);

         editorLayout_->setCurrentWidget(editor);
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
