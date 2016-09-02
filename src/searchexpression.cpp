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
   return (((nameTerms_.isEmpty() == false) || (tagsTerms_.isEmpty() == false)) &&
           ((nameTerms_.isEmpty() == true) || (MatchesName_(name) == true)) &&
           ((tagsTerms_.isEmpty() == true) || (MatchesTags_(tags) == true)));
}

void SearchExpression::Compile_(const QString& expression)
{
   auto operation = Term_::Operation::Conjunct;
   auto negation = Term_::Negation::NotNegated;

   nameTerms_.clear();
   tagsTerms_.clear();

   auto expressionTokens = expression.split(' ', QString::SkipEmptyParts);
   for (auto expressionToken : expressionTokens)
   {
      if (expressionToken.compare("AND", Qt::CaseInsensitive) == 0)
      {
         operation = Term_::Operation::Conjunct;
      }
      else if (expressionToken.compare("OR", Qt::CaseInsensitive) == 0)
      {
         operation = Term_::Operation::Disjunct;
      }
      else if (expressionToken.compare("NOT", Qt::CaseInsensitive) == 0)
      {
         negation = Term_::Negation::Negated;
      }
      else
      {
         expressionToken.replace("?", ".");
         expressionToken.replace("*", ".*");

         if (expressionToken.startsWith('@') == false)
         {
            expressionToken.prepend('^');

            nameTerms_.append({QRegularExpression(expressionToken, QRegularExpression::CaseInsensitiveOption), operation, negation});
         }
         else if (expressionToken.size() >= 2)
         {
            expressionToken.replace(0, 1, '^');

            tagsTerms_.append({QRegularExpression(expressionToken, QRegularExpression::CaseInsensitiveOption), operation, negation});
         }

         operation = Term_::Operation::Conjunct;
         negation = Term_::Negation::NotNegated;
      }
   }
}

bool SearchExpression::MatchesName_(const QString& name) const
{
   bool isConjunctMatch = false;
   bool isConjunctMatchInitialization = true;

   bool isDisjunctMatch = false;

   for (const auto &nameTerm : nameTerms_)
   {
      bool isTermMatch = (nameTerm.expression.match(name).hasMatch() ^ (nameTerm.negation == Term_::Negation::Negated));

      if (nameTerm.operation == Term_::Operation::Conjunct)
      {
         isConjunctMatch = isTermMatch && (isConjunctMatch || isConjunctMatchInitialization);
         isConjunctMatchInitialization = false;
      }
      else if (nameTerm.operation == Term_::Operation::Disjunct)
      {
         isDisjunctMatch = isTermMatch || (isDisjunctMatch);
      }
   }

   return ((isConjunctMatch == true) || (isDisjunctMatch == true));
}

bool SearchExpression::MatchesTags_(const QStringList& tags) const
{
   bool isConjunctMatch = false;
   bool isConjunctMatchInitialization = true;

   bool isDisjunctMatch = false;

   for (const auto &tagsTerm : tagsTerms_)
   {
      bool isTermMatch = false;
      for (auto tag = std::begin(tags); ((tag != std::end(tags)) && (isTermMatch == false)); ++tag)
      {
         isTermMatch = (tagsTerm.expression.match(*tag).hasMatch() ^ (tagsTerm.negation == Term_::Negation::Negated));
      }

      if (tagsTerm.operation == Term_::Operation::Conjunct)
      {
         isConjunctMatch = isTermMatch && (isConjunctMatch || isConjunctMatchInitialization);
         isConjunctMatchInitialization = false;
      }
      else if (tagsTerm.operation == Term_::Operation::Disjunct)
      {
         isDisjunctMatch = isTermMatch || (isDisjunctMatch);
      }
   }

   return ((isConjunctMatch == true) || (isDisjunctMatch == true));
}
