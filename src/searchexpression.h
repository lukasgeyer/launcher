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

/*!
 * \brief A search expression.
 *
 * Each search term within a search expression is separated by a whitespace
 * and conjunct by default. Using the "OR" keyword the following search term
 * is disjunct. A search term matches an item name by default, if it is
 * preceded with an "@" it matches any item tag. A '?' can be used to match
 * any character, a '*' can be used to match any string. The "NOT" keyword
 * can be used to negate the next search term (so it must not match).
 *
 * Creating a search expression is an expensive operation, so it should be
 * created once and used to Match() often.
 */
class SearchExpression
{
public:
   /*!
    * Constructs a search expression.
    */
   explicit SearchExpression();
   /*!
    * Constructs a search expression from the string \a expression.
    */
   explicit SearchExpression(const QString& expression);

   /*!
    * Sets the search expression to \a expression.
    */
   void setExpression(const QString& expression);

   /*!
    * Returns \a true if the search expression matches the string \a name or
    * one of the tags found in \a tags; \a false otherwise.
    */
   bool Matches(const QString& name, const QStringList& tags) const;

private:
   /*!
    * \brief A search term.
    */
   struct Term_
   {
      QRegularExpression expression;

      struct Option
      {
         enum class Operation
         {
            Conjunct,
            Disjunct,
         } operation = Operation::Conjunct;

         enum class Negation
         {
            NotNegated,
            Negated
         } negation = Negation::NotNegated;

         enum class Target
         {
            Name,
            Tag
         } target = Target::Name;
      } option;
   };

   /*!
    * A list of search terms.
    */
   QVector<Term_> terms_;

   /*!
    * Compiles the search expression \a expression.
    */
   void Compile_(const QString &expression);
};


#endif // SEARCHEXPRESSION_H
