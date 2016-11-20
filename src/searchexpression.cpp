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
   bool matches = true;

   matches &= (nameTerms_.isEmpty() || matchesName_(name));
   matches &= (tagsTerms_.isEmpty() || matchesTags_(tags));

   return matches;
}

void SearchExpression::compile_(const QString& expression)
{
   //
   // The predicates used to match an operation or negation.
   //

   static const auto& conjunctPredicates = QStringList{QObject::tr("and"), "&&", "&"};
   static const auto& disjunctPredicates = QStringList{QObject::tr("or"),  "||", "|"};
   static const auto& negationPredicates = QStringList{QObject::tr("not"), "!!", "!"};

   //
   // The type of the token currently being parsed.
   //

   enum class TokenType { Operator, Name, Tag, Parameter } tokenType = TokenType::Name;

   //
   // The term currently being parsed. Be aware that the first term is conjunct by default.
   //

   Term_ term;
   term.operation = Term_::Operation::Conjunct;
   term.negation = Term_::Negation::NotNegated;

   //
   // Parse the expression, token by token, term by term.
   //

   nameTerms_.clear();
   tagsTerms_.clear();

   parameters_.clear();

   auto tokens = expression.split(' ', QString::SkipEmptyParts);
   for (auto token : tokens)
   {
      //
      // Determine token type.
      //

      if (conjunctPredicates.contains(token, Qt::CaseInsensitive))
      {
         tokenType = TokenType::Operator;
         term.operation = Term_::Operation::Conjunct;
      }
      else if (disjunctPredicates.contains(token, Qt::CaseInsensitive))
      {
         tokenType = TokenType::Operator;
         term.operation = Term_::Operation::Disjunct;
      }
      else if (negationPredicates.contains(token, Qt::CaseInsensitive))
      {
         tokenType = TokenType::Operator;
         term.negation = Term_::Negation::Negated;
      }
      else if (token.startsWith('@'))
      {
         tokenType = TokenType::Tag;
      }
      else if (term.operation != Term_::Operation::None)
      {
         tokenType = TokenType::Name;
      }
      else
      {
         tokenType = TokenType::Parameter;
      }

      //
      // Determine term (given that enough tokens have been parsed).
      //

      switch (tokenType)
      {
      case TokenType::Operator:
      {
         break;
      }
      case TokenType::Name:
      {
         token.prepend('^');
         token.replace("?", ".");
         token.replace("*", ".*");

         term.expression.setPattern(token);
         term.expression.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

         nameTerms_.append(term);

         term.operation = Term_::Operation::None;
         term.negation = Term_::Negation::NotNegated;

         break;
      }
      case TokenType::Tag:
      {
         token.replace(0, 1, '^');
         token.replace("?", ".");
         token.replace("*", ".*");

         term.expression.setPattern(token);
         term.expression.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

         if (term.operation == Term_::Operation::None)
         {
            term.operation = Term_::Operation::Conjunct;
         }

         tagsTerms_.append(term);

         term.operation = Term_::Operation::None;
         term.negation = Term_::Negation::NotNegated;

         break;
      }
      case TokenType::Parameter:
      {
         parameters_.append(token);

         break;
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

   return (isConjunctMatch || isDisjunctMatch);
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

   return (isConjunctMatch || isDisjunctMatch);
}

QDebug operator<<(QDebug stream, const SearchExpression::Term_& searchExpressionTerm)
{
   switch (searchExpressionTerm.operation)
   {
   case SearchExpression::Term_::Operation::None: break;
   case SearchExpression::Term_::Operation::Conjunct: stream << "&"; break;
   case SearchExpression::Term_::Operation::Disjunct: stream << "|"; break;
   }

   switch (searchExpressionTerm.negation)
   {
   case SearchExpression::Term_::Negation::NotNegated: break;
   case SearchExpression::Term_::Negation::Negated: stream << "!"; break;
   }

   stream << searchExpressionTerm.expression.pattern();

   return stream;
}

QDebug operator<<(QDebug stream, const SearchExpression& searchExpression)
{
   stream << "name" << searchExpression.nameTerms_;
   stream << "tags" << searchExpression.tagsTerms_;
   stream << "parameter" << searchExpression.parameters_;

   return stream;
}

