/*!
 * \file itemlock.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEMLOCK_H
#define ITEMLOCK_H

#include <QString>

/*!
 * \brief An application-spanning lock.
 */
class ItemLock
{
public:
   /*!
    * Destructs an item lock, unlocking it if locked.
    */
   ~ItemLock();

   /*!
    * Tries to lock and returns \a true if the lock could be obtained; \a false otherwise.
    */
   bool tryLock();
   /*!
    * Unlocks the lock if it is locked.
    */
   void unlock();

private:
#if defined(Q_OS_LINUX)
   /*!
    * The file descriptor of the lock file.
    */
   int lockFileDescriptor_ = -1;
#elif defined(Q_OS_WIN)
   /*!
    * The file handle of the lock file.
    */
   void* lockFileHandle_ = nullptr;
#endif // defined(Q_OS_LINUX)
};

#endif // ITEMLOCK_H
