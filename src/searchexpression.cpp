/*!
 * \file searchexpression.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include "searchexpression.h"

SearchExpression::SearchExpression()
{
}

SearchExpression::SearchExpression(const QString& expression)
{
   Compile_(expression);
}

void SearchExpression::setExpression(const QString& expression)
{
   Compile_(expression);
}

bool SearchExpression::Matches(const QString& name, const QStringList& tags) const
{
   bool isConjunctMatch = false;
   bool isConjunctMatchInitialization = true;

   bool isDisjunctMatch = false;

   for (const auto &term : terms_)
   {
      bool negate = (term.option.negation == Term_::Option::Negation::Negated);

      bool isTermMatch = false;
      if (term.option.target == Term_::Option::Target::Name)
      {
         isTermMatch = (term.expression.match(name).hasMatch() ^ negate);
      }
      else if (term.option.target == Term_::Option::Target::Tag)
      {
         for (auto tag = std::begin(tags); ((tag != std::end(tags)) && (isTermMatch == false)); ++tag)
         {
            isTermMatch = (term.expression.match(*tag).hasMatch() ^ negate);
         }
      }

      if (term.option.operation == Term_::Option::Operation::Conjunct)
      {
         isConjunctMatch = isTermMatch && (isConjunctMatch || isConjunctMatchInitialization);
         isConjunctMatchInitialization = false;
      }
      else if (term.option.operation == Term_::Option::Operation::Disjunct)
      {
         isDisjunctMatch = isTermMatch || (isDisjunctMatch);
      }
   }

   bool isMatch = ((isConjunctMatch == true) || (isDisjunctMatch == true));

   return isMatch;
}

void SearchExpression::Compile_(const QString& expression)
{
   Term_::Option termOption;

   terms_.clear();

   auto expressionTokens = expression.split(' ', QString::SkipEmptyParts);
   for (auto expressionToken : expressionTokens)
   {
      if (expressionToken.compare("AND", Qt::CaseInsensitive) == 0)
      {
         termOption.operation = Term_::Option::Operation::Conjunct;
      }
      else if (expressionToken.compare("OR", Qt::CaseInsensitive) == 0)
      {
         termOption.operation = Term_::Option::Operation::Disjunct;
      }
      else if (expressionToken.compare("NOT", Qt::CaseInsensitive) == 0)
      {
         termOption.negation = Term_::Option::Negation::Negated;
      }
      else
      {
         if (expressionToken.startsWith('@') == true)
         {
            expressionToken.remove(0 /* start of string */, 1 /* one character */);

            termOption.target = Term_::Option::Target::Tag;
         }

         if (expressionToken.isEmpty() == false)
         {
            expressionToken.prepend("^");
            expressionToken.replace("?", ".");
            expressionToken.replace("*", ".*");

            terms_.append({QRegularExpression(expressionToken, QRegularExpression::CaseInsensitiveOption), termOption});
         }

         termOption = Term_::Option{};
      }
   }
}
