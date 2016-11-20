/*!
 * \file event.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef EVENT_H
#define EVENT_H

#include <QEvent>

namespace Event {

/*!
 * Returns the event \a event as \a EventType if the type is \a type; \a nullptr otherwise.
 */
template <typename EventType> EventType* cast(QEvent* event, QEvent::Type type)
{
   return ((event != nullptr) ? ((event->type() == type) ? (static_cast<EventType*>(event)) : (nullptr)) : (nullptr));
}

} // namespace Event

#endif // EVENT_H
