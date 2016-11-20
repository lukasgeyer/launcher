/*!
 * \file searchitemfiltermodel.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef SEARCHFILTERMODEL_H
#define SEARCHFILTERMODEL_H

#include <QSortFilterProxyModel>

#include "searchexpression.h"

class LinkItem;

/*!
 * \brief A sort filter proxy model filtering an item based on the name and tag by a regular
 *        expression, sorted by type (tagged or not tagged) and name.
 */
class SearchItemFilterModel : public QSortFilterProxyModel
{
   Q_OBJECT

public:
   /*!
    * Constructs an ItemFilterModel with the parent \a parent.
    */
   SearchItemFilterModel(QObject* parent = nullptr);

   /*!
    * Returns the current search expression.
    */
   const SearchExpression& searchExpression() const
   {
      return searchExpression_;
   }

   /*!
    * If \a filterEmptySearchExpression is \a true any item is filtered if the search expression
    * is empty.
    */
   void setFilterEmptySearchExpression(bool filterEmptySearchExpression)
   {
      filterEmptySearchExpression_ = filterEmptySearchExpression;
   }
   /*!
    * Returns \a true if any item is filtered if the search expression is empty; \a false
    * otherwise.
    */
   bool filterEmptySearchExpression() const
   {
      return filterEmptySearchExpression_;
   }

   /*!
    * If \a filterParameterMismatch is \a true an item whose parameter do not match the search
    * expression will be filtered out. The default value is \a false.
    */
   void setFilterParameterMismatch(bool filterParameterMismatch)
   {
      filterParameterMismatch_ = filterParameterMismatch;
   }
   /*!
    * Returns \a true if items whose parameter do not match the search expression are
    * filtered out; \a false otherwise.
    */
   bool filterParameterMismatch() const
   {
      return filterParameterMismatch_;
   }

   /*!
    * Returns a pointer to the item at the index \a proxyIndex or \a nullptr if there is no such item.
    */
   LinkItem* item(const QModelIndex& proxyIndex);
   /*!
    * Returns a pointer to the item at the index \a proxyIndex or \a nullptr if there is no such item.
    */
   const LinkItem* item(const QModelIndex& proxyIndex) const;

public slots:
   /*!
    * Sets the regular pattern filter to \a regularExpressionPattern.
    */
   void setSearchExpression(const QString& expression);

protected:
   /*!
    * \reimp
    */
   bool filterAcceptsRow(int sourceRaw, const QModelIndex& sourceParent) const override;
   /*!
    * \reimp
    */
   bool lessThan(const QModelIndex &sourceLeft, const QModelIndex& sourceRight) const override;

private:
   /*!
    * The regular expression used for matching.
    */
   SearchExpression searchExpression_;

   /*!
    * If \a filterEmptySearchExpression is \a true any item is filtered if the search expression
    * is empty.
    */
   bool filterEmptySearchExpression_ = false;
   /*!
    * If set to \a true an item whose parameter do not match the search expression will be
    * filtered out. The default value is \a false.
    */
   bool filterParameterMismatch_ = false;
};

#endif // SEARCHFILTERMODEL_H
