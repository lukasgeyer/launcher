/*!
 * \file searchwindow.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef SEARCHWINDOW_H
#define SEARCHWINDOW_H

#include <QPoint>
#include <QWidget>

#include "itemmodel.h"
#include "itemsourceposition.h"
#include "searchexpression.h"

class LinkItem;
class QTableView;
class SearchBarWidget;
class SearchItemFilterModel;
class SearchItemProxyModel;
class SearchResultWidget;

/*!
 * \brief An item window, listing all items, filtered by name and tags.
 */
class SearchWindow : public QWidget
{
   Q_OBJECT

public:
   /*!
    * Constructs an ItemWindow with the parent \a parent.
    */
   SearchWindow(ItemModel* itemModel, QWidget* parent = nullptr);
   /*!
    * Destructs the ItemWindow.
    */
   ~SearchWindow();

private:
   /*!
    * The type of window modification currently active.
    */
   enum class WindowModificationOperation
   {
      None, /*< No window modification is currently performed. */
      Move, /*< The window is currently moved. */
      Resize /*< The window is currently resized. */
   };
   /*!
    * The current window modifcation.
    */
   WindowModificationOperation windowModificationOperation_ = WindowModificationOperation::None;
   /*!
    * Is \a true if the window modification operation is active.
    */
   bool windowModificationOperationActive_ = false;
   /*!
    * Is \a true if the required modifier for window modification is active.
    */
   bool windowModificationModifierActive_ = false;
   /*!
    * The origin of the window modification.
    */
   QPoint windowModificationOrigin_ = {};

   /*!
    * The item model.
    */
   ItemModel* itemModel_ = nullptr;
   /*!
    * The search item proxy model.
    */
   SearchItemProxyModel* searchItemProxyModel_ = nullptr;
   /*!
    * The search item filter model.
    */
   SearchItemFilterModel* searchItemFilterModel_ = nullptr;
   /*!
    * The implicit matching strategy for the search item filter model.
    */
   SearchExpression::ImplicitMatch searchItemFilterModelImplicitMatch_ = SearchExpression::ParameterImplicitMatch;

   /*!
    * The item edit used to enter a search expression.
    */
   SearchBarWidget* searchBarWidget_ = nullptr;
   /*!
    * The view used to display the search result.
    */
   SearchResultWidget* searchResultWidget_ = nullptr;

   /*!
    * \reimp
    */
   bool event(QEvent *event) override;

   /*!
    * \reimp
    */
   bool eventFilter(QObject* object, QEvent* event) override;

   /*!
    * Opens the source \a source at position \a position.
    */
   bool openSource_(const QString& source, const ItemSourcePosition& position = ItemSourcePosition());

   /*!
    * Tries to open the item \a item with the default application and returns \a true if the item
    * could be opened; \a false otherwise  (in addition an indication will be shown).
    */
   bool openUrl_(LinkItem* item, const QStringList& parameters);
};

#endif // SEARCHWINDOW_H
