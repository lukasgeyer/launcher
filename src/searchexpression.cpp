/*!
 * \file searchexpression.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QObject>

#include "searchexpression.h"

SearchExpression::SearchExpression()
{
}

SearchExpression::SearchExpression(const QString& expression)
{
   compile_(expression);
}

void SearchExpression::setExpression(const QString& expression)
{
   compile_(expression);
}

bool SearchExpression::matches(const QString& name, const QStringList& tags) const
{
   return (((!nameTerms_.isEmpty()) || (!tagsTerms_.isEmpty())) && ((nameTerms_.isEmpty()) || ( matchesName_(name))) &&
                                                                   ((tagsTerms_.isEmpty()) || ( matchesTags_(tags))));
}

void SearchExpression::compile_(const QString& expression)
{
   const auto& conjunctPredicate = QObject::tr("and", "search expression predicate");
   const auto& disjunctPredicate = QObject::tr("or", "search expression predicate");
   const auto& negationPredicate = QObject::tr("not", "search expression predicate");

   auto operation = Term_::Operation::Conjunct;
   auto negation = Term_::Negation::NotNegated;

   nameTerms_.clear();
   tagsTerms_.clear();

   auto expressionTokens = expression.split(' ', QString::SkipEmptyParts);
   for (auto expressionToken : expressionTokens)
   {
      if (expressionToken.compare(conjunctPredicate, Qt::CaseInsensitive) == 0)
      {
         operation = Term_::Operation::Conjunct;
      }
      else if (expressionToken.compare(disjunctPredicate, Qt::CaseInsensitive) == 0)
      {
         operation = Term_::Operation::Disjunct;
      }
      else if (expressionToken.compare(negationPredicate, Qt::CaseInsensitive) == 0)
      {
         negation = Term_::Negation::Negated;
      }
      else
      {
         if (expressionToken.startsWith(':'))
         {
            parameter_.append(expressionToken.right(expressionToken.length() - 1 /* ':' */));
         }
         else
         {
            expressionToken.replace("?", ".");
            expressionToken.replace("*", ".*");

            if (expressionToken.startsWith('@'))
            {
               if (expressionToken.size() >= 2)
               {
                  expressionToken.replace(0, 1, '^');

                  tagsTerms_.append({QRegularExpression(expressionToken, QRegularExpression::CaseInsensitiveOption), operation, negation});
               }
            }
            else
            {
               expressionToken.prepend('^');

               nameTerms_.append({QRegularExpression(expressionToken, QRegularExpression::CaseInsensitiveOption), operation, negation});
            }

            operation = Term_::Operation::Conjunct;
            negation = Term_::Negation::NotNegated;
         }
      }
   }
}

bool SearchExpression::matchesName_(const QString& name) const
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

   return ((isConjunctMatch ) || (isDisjunctMatch ));
}

bool SearchExpression::matchesTags_(const QStringList& tags) const
{
   bool isConjunctMatch = false;
   bool isConjunctMatchInitialization = true;

   bool isDisjunctMatch = false;

   for (const auto &tagsTerm : tagsTerms_)
   {
      bool isTermMatch = false;
      for (auto tag = std::begin(tags); ((tag != std::end(tags)) && (!isTermMatch)); ++tag)
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

   return ((isConjunctMatch ) || (isDisjunctMatch ));
}
