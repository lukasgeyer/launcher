/*!
 * \file itemsourcehighlighter.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEMSOURCEHIGHLIGHTER_H
#define ITEMSOURCEHIGHLIGHTER_H

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVector>

class QTextDocument;

/*!
 * An item source highlighter.
 */
class ItemSourceHighlighter : public QSyntaxHighlighter
{
   Q_OBJECT

public:
   /*!
    * Constructs a highlighter with a parent of \a parent.
    */
   ItemSourceHighlighter(QObject* parent = nullptr);

protected:
   /*!
    * \reimp
    */
   void highlightBlock(const QString& text) override;

private:
   /*!
    * A highlighting rule.
    */
   struct Rule_
   {
      /*!
       * The regular expression matching this rule.
       */
      QVector<QRegularExpression> expressions;
      /*!
       * The format for this rule.
       */
      QTextCharFormat format;
   };

   /*!
    * A list of highlighting rules.
    */
   QVector<Rule_> rules_;
};

#endif // ITEMSOURCEHIGHLIGHTER_H
