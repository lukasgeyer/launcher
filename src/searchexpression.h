/*!
 * \file searchexpression.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef SEARCHEXPRESSION_H
#define SEARCHEXPRESSION_H

#include <QRegularExpression>
#include <QString>
#include <QVector>

#include "linkitem.h"

/*!
 * \brief A search expression.
 *
 * Each search term within a search expression is separated by a whitespace
 * and conjunct by default. Using the "OR" keyword the following search term
 * is disjunct. A search term matches an item name by default, if it is
 * preceded with an "@" it matches any item tag. A '?' can be used to match
 * any character, a '*' can be used to match any string. The "NOT" keyword
 * can be used to negate the next search term (so it must not match). If
 * no operator is specified the term is interpreted as parameter.
 *
 * Creating a search expression is an expensive operation, so it should be
 * created once and used to match() often.
 */
class SearchExpression
{
   Q_GADGET

public:
   /*!
    * The implicit matching strategy defines how non-prefixed terms are interpreted.
    */
   enum ImplicitMatch
   {
       NoImplicitMatch, /*< A non-prefixed term is ignored. */
       TermImplicitMatch, /*< A non-prefixed term is treated as search term. */
       ParameterImplicitMatch /*< A non-prefixed term is treated as parameter. */
   };
   Q_ENUM(ImplicitMatch)

   /*!
    * Constructs a search expression.
    */
   SearchExpression();
   /*!
    * Constructs a search expression from the string \a expression.
    */
   explicit SearchExpression(const QString& expression, ImplicitMatch implicitMatch);

   /*!
    * Sets the search expression to \a expression.
    */
   void setExpression(const QString& expression, ImplicitMatch implicitMatch);

   /*!
    * Returns \a true if the expression is \a empty; \a false otherwise.
    */
   bool isEmpty() const
   {
      return (terms_.isEmpty() && parameters_.isEmpty());
   }

   /*!
    * Returns \a true if the search expression matches the string \a name or
    * one of the tags found in \a tags; \a false otherwise.
    */
   bool matches(const QString& name, const QStringList& tags) const;

   /*!
    * Returns the list of parameter provided with the search term since the
    * last call to setExpression().
    */
   QStringList parameters() const
   {
      return parameters_;
   }

private:
   /*!
    * \brief A search term.
    */
   struct Term_
   {
      QRegularExpression expression;

      enum class Type
      {
         Name,
         Tag,
         Both
      } type = Type::Name;

      enum class Operation
      {
         None,
         Conjunct,
         Disjunct,
      } operation = Operation::None;

      enum class Negation
      {
         NotNegated,
         Negated
      } negation = Negation::NotNegated;
   };

   /*!
    * The regular expression used to tokenize the search expression.
    */
   QRegularExpression tokenizeExpression_;

   /*!
    * A list of search terms to be applied.
    */
   QVector<Term_> terms_;
   /*!
    * A list of parameters provided with the search term.
    */
   QStringList parameters_;

   /*!
    * Compiles the search expression \a expression.
    */
   void compile_(const QString &expression, ImplicitMatch implicitMatch);

   /*!
    * Returns \a true if the name \a name and the tags \a tags match any name search terms.
    */
   bool matches_(const QString &name, const QStringList& tags) const;

   /*!
    * Inserts the search expression term  \a searchExpressionTerm into the stream \a stream and returns the stream.
    */
   friend QDebug operator<<(QDebug stream, const SearchExpression::Term_& searchExpressionTerm);
   /*!
    * Inserts the search expression \a searchExpression into the stream \a stream and returns the stream.
    */
   friend QDebug operator<<(QDebug stream, const SearchExpression& searchExpression);
};

#endif // SEARCHEXPRESSION_H
