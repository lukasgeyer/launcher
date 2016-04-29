/*!
 * \file sourcehighlighter.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QBrush>
#include <QPalette>
#include <QRegularExpressionMatch>

#include "sourcehighlighter.h"

SourceHighlighter::SourceHighlighter(QObject* parent) : QSyntaxHighlighter(parent)
{
   ///
   /// The keyword rule.
   ///
   {
      Rule_ rule;
      rule.expressions.append(QRegularExpression(QStringLiteral("(<[ ]*\?.*\?>)")));
      rule.format.setForeground(Qt::lightGray);
      rules_.append(std::move(rule));
   }

   ///
   /// The comment rule.
   ///
   {
      Rule_ rule;
      rule.expressions.append(QRegularExpression(QStringLiteral("(<!--[^\\n]*-->)")));
      rule.format.setForeground(Qt::lightGray);
      rules_.append(std::move(rule));
   }

   ///
   /// The item(s), import tag rule.
   ///
   {
      Rule_ rule;
      rule.expressions.append(QRegularExpression(QStringLiteral("(<[/]*item[s]*[ /]*>)")));
      rule.expressions.append(QRegularExpression(QStringLiteral("(<[/]*import[ /]*>)")));
      rule.format.setForeground(Qt::darkBlue);
      rules_.append(std::move(rule));
   }

   ///
   /// The name, URL, file tag rule.
   ///
   {
      Rule_ rule;
      rule.expressions.append(QRegularExpression(QStringLiteral("(<[/]*name[ /]*>)")));
      rule.expressions.append(QRegularExpression(QStringLiteral("(<[/]*url[ /]*>)")));
      rule.expressions.append(QRegularExpression(QStringLiteral("(<[/]*file[ /]*>)")));
      rule.format.setForeground(Qt::darkGray);
      rules_.append(std::move(rule));
   }

   ///
   /// The name content rule.
   ///
   {
      Rule_ rule;
      rule.expressions.append(QRegularExpression(QStringLiteral("<name>(.*)</name>")));
      rule.format.setFontWeight(QFont::Bold);
      rules_.append(std::move(rule));
   }
}

void SourceHighlighter::highlightBlock(const QString& text)
{
   ///
   /// Apply matching highlighting rules.
   ///
   for (const auto& rule : rules_)
   {
      for (const auto& expression : rule.expressions)
      {
         auto matches = expression.globalMatch(text);
         while (matches.hasNext() == true)
         {
            auto match = matches.next();

            setFormat(match.capturedStart(1), match.capturedLength(1), rule.format);
         }
      }
   }
}
