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
    * The sort algorithms.
    */
   enum SortAlgorithm
   {
      NameSortAlgorithm = 0, /* The items shall be sorted by name. */
      TagSortAlgorithm = 1, /* The items shall be sorted by tag, then name. */
   };
   Q_ENUM(SortAlgorithm)

   /*!
    * The filters.
    */
   enum Filter
   {
      EmptySearchExpressionFilter = 0x01, /*< Filter all item is expression is empty. */
      ParameterMismatchFilter = 0x02 /*< Filter all items where the parameter do not match. */
   };
   Q_FLAG(Filter)

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
    * Sets the filter to \a filter..
    */
   void setFilter(Filter filter)
   {
      filter_ = filter;
   }
   /*!
    * Returns the filter.
    */
   Filter filter() const
   {
      return filter_;
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
   void setSearchExpression(const QString& expression, SearchExpression::ImplicitMatch implicitMatch);

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
    * The filters to be applied when \a filterAcceptsRow() is called.
    */
   Filter filter_ = static_cast<Filter>(EmptySearchExpressionFilter | ParameterMismatchFilter);
};

Q_DECLARE_METATYPE(SearchItemFilterModel::SortAlgorithm)
Q_DECLARE_METATYPE(SearchItemFilterModel::Filter)

#endif // SEARCHFILTERMODEL_H
