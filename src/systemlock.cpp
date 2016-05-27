/*!
 * \file systemlock.cpp
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
#elif defined(Q_OS_WIN)
   #include <qt_windows.h>
#endif // defined(Q_OS_LINUX)

#include "systemlock.h"

SystemLock::~SystemLock()
{
   unlock();
}

bool SystemLock::tryLock()
{
   bool isLocked = false;

#if defined(Q_OS_LINUX)
   isLocked = (lockFileDescriptor_ >= 0);
   if (isLocked == false)
   {
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
   }
#elif defined(Q_OS_WIN)
   isLocked = (lockFileHandle_ != nullptr);
   if (isLocked == false)
   {
      lockFileHandle_ = CreateFileA(qApp->applicationFilePath().toLocal8Bit().data(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
      if (lockFileHandle_ >= 0)
      {
         isLocked = true;
      }
#endif // defined(Q_OS_LINUX)
   }

   return isLocked;
}

void SystemLock::unlock()
{
#if defined(Q_OS_LINUX)
   if (lockFileDescriptor_ >= 0)
   {
      close(lockFileDescriptor_);

      lockFileDescriptor_ = -1;
   }
#elif defined(Q_OS_WIN)
   if (lockFileHandle_)
   {
      CloseHandle(lockFileHandle_);

      lockFileHandle_ = nullptr;
   }
#endif // defined(Q_OS_LINUX)
}
