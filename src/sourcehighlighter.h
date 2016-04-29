/*!
 * \file sourcehighlighter.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef SOURCEHIGHLIGHTER_H
#define SOURCEHIGHLIGHTER_H

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVector>

class QTextDocument;

/*!
 * A source highlighter.
 */
class SourceHighlighter : public QSyntaxHighlighter
{
   Q_OBJECT

public:
   /*!
    * Constructs a SourceHighlighter with a parent of \a parent.
    */
   SourceHighlighter(QObject* parent = nullptr);

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

#endif // SOURCEHIGHLIGHTER_H
