/*!
 * \file itemlock.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QCoreApplication>

#if defined(Q_OS_LINUX)
   #include <fcntl.h>
   #include <sys/file.h>
   #include <unistd.h>
#elif defined(Q_OS_WINDOWS)
   #include <qt_windows.h>
#endif // defined(Q_OS_LINUX)

#include "itemlock.h"

ItemLock::~ItemLock()
{
   unlock();
}

bool ItemLock::tryLock()
{
   bool isLocked = false;

   isLocked = (lockFileDescriptor_ >= 0);
   if (isLocked == false)
   {
#if defined(Q_OS_LINUX)
      lockFileDescriptor_ = open(qApp->applicationFilePath().toLocal8Bit().data(), O_RDONLY, 0);
      if (lockFileDescriptor_ >= 0)
      {
         isLocked = (flock(lockFileDescriptor_, LOCK_EX | LOCK_NB) == 0);
         if (isLocked == false)
         {
            close(lockFileDescriptor_);

            lockFileDescriptor_ = -1;
         }
      }
#elif defined(Q_OS_WIN)
      lockFileDescriptor_ = CreateFile(qApp->applicationFilePath().toLocal8Bit().data(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
      if (lockFileDescriptor_ >= 0)
      {
         isLocked = true;
      }
#endif // defined(Q_OS_LINUX)
   }

   return isLocked;
}

void ItemLock::unlock()
{
   if (lockFileDescriptor_ >= 0)
   {
#if defined(Q_OS_LINUX)
      close(lockFileDescriptor_);
#elif defined(Q_OS_WIN)
      CloseHandle(lockFileDescriptor_);
#endif

      lockFileDescriptor_ = -1;
   }
}
