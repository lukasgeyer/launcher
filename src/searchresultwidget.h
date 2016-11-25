/*!
 * \file searchresultwidget.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QTableView>

#ifndef SEARCHRESULTWIDGET_H
#define SEARCHRESULTWIDGET_H

/*!
 * \brief A widget being capable of shown search results.
 */
class SearchResultWidget : public QTableView
{
   Q_OBJECT

public:
   /*!
    * Constructs a search result widget with a parent \a parent.
    */
   SearchResultWidget(QWidget* parent = nullptr) : QTableView(parent)
   {
      setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
   }

   /*!
    * \reimp
    */
   QSize minimumSizeHint() const override
   {
      return QSize(0, 0);
   }
};

#endif // SEARCHRESULTWIDGET_H
