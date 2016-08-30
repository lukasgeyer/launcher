/*!
 * \file itemfiltermodel.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEMFILTERMODEL_H
#define ITEMFILTERMODEL_H

#include <QSortFilterProxyModel>

#include "searchexpression.h"

/*!
 * \brief A sort filter proxy model filtering an item based on the name and tag by a regular
 *        expression, sorted by type (tagged or not tagged) and name.
 */
class ItemFilterModel : public QSortFilterProxyModel
{
   Q_OBJECT

public:
   /*!
    * Constructs an ItemFilterModel with the parent \a parent.
    */
   ItemFilterModel(QObject* parent = nullptr);

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
};

#endif // ITEMFILTERMODEL_H
