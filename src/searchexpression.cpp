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
   return (terms_.isEmpty() || matches_(name, tags));
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

   enum class TokenType { Operator, Name, Tag, Both, Parameter } tokenType = TokenType::Name;

   //
   // The term currently being parsed. Be aware that the first term is conjunct by default.
   //

   Term_ term;
   term.type = Term_::Type::Both;
   term.operation = Term_::Operation::Conjunct;
   term.negation = Term_::Negation::NotNegated;

   //
   // Parse the expression, token by token, term by term.
   //

   terms_.clear();
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
         term.type = Term_::Type::Tag;
      }
      else if (token.startsWith(':'))
      {
         tokenType = TokenType::Name;
         term.type = Term_::Type::Name;
      }
      else if (term.operation != Term_::Operation::None)
      {
         tokenType = TokenType::Both;
         term.type = Term_::Type::Both;
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
      case TokenType::Tag:
      {
         token.remove(0, 1);

         //
         // Fall Through
         //
      }
      case TokenType::Both:
      {
         token.prepend('^');

         if (token.length() >= 2)
         {
            token.replace("?", ".");
            token.replace("*", ".*");

            if (term.operation == Term_::Operation::None)
            {
               term.operation = Term_::Operation::Conjunct;
            }

            term.expression.setPattern(token);
            term.expression.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

            terms_.append(term);
         }

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

bool SearchExpression::matches_(const QString& name, const QStringList& tags) const
{
   bool isConjunctMatch = false;
   bool isConjunctMatchInitialization = true;

   bool isDisjunctMatch = false;

   for (const auto &term : terms_)
   {
      bool isTermMatch = false;

      if ((term.type == Term_::Type::Name) || (term.type == Term_::Type::Both))
      {
         isTermMatch = (term.expression.match(name).hasMatch() ^ (term.negation == Term_::Negation::Negated));
      }
      if ((term.type == Term_::Type::Tag) || (term.type == Term_::Type::Both))
      {
         for (auto tag = std::begin(tags); ((tag != std::end(tags)) && (!isTermMatch)); ++tag)
         {
            isTermMatch = (term.expression.match(*tag).hasMatch() ^ (term.negation == Term_::Negation::Negated));
         }
      }

      if (term.operation == Term_::Operation::Conjunct)
      {
         isConjunctMatch = isTermMatch && (isConjunctMatch || isConjunctMatchInitialization);
         isConjunctMatchInitialization = false;
      }
      else if (term.operation == Term_::Operation::Disjunct)
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
   return (stream << searchExpression.terms_ << searchExpression.parameters_);
}

