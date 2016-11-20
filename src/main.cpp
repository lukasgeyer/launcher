/*!
 * \file main.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QCommandLineParser>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QThreadPool>

#include "application.h"
#include "systemlock.h"
#include "searchwindow.h"

#include "itemmodel.h"

namespace {

/*!
 * The default log message handler.
 */
QtMessageHandler logMessageHandler_ = nullptr;
/*!
 * The log file stream.
 */
QTextStream logFileStream_;
/*!
 * The log file handler.
 */
void logFileHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
   static const char* typeString[] = { "DEBUG", "WARNING", "CRITICAL", "FATAL", "INFO", "SYSTEM" };

   logFileStream_ << QDateTime::currentDateTime().toString(Qt::ISODate) << " " << typeString[type] << " " << message.toLocal8Bit() << endl;

   if (logMessageHandler_ != nullptr)
   {
      (*logMessageHandler_)(type, context, message);
   }
}

} // namespace

int main(int argc, char *argv[])
{
   int result = EXIT_FAILURE;

   //
   // Set up the application.
   //

   Application application(argc, argv);

   //
   // Parse command line.
   //

   QCommandLineParser commandLineParser;
   const auto& helpCommandLineOption = commandLineParser.addHelpOption();
   const auto& versionCommandLineOption = commandLineParser.addVersionOption();

   QCommandLineOption sourceCommandLineOption(QStringLiteral("source"));
   sourceCommandLineOption.setDescription(QObject::tr("The XML-file items should be read from"));
   sourceCommandLineOption.setValueName(QObject::tr("file"));
   sourceCommandLineOption.setDefaultValue(QStringLiteral("launcher.xml"));
   commandLineParser.addOption(sourceCommandLineOption);


   if (commandLineParser.parse(application.arguments()))
   {
      if (commandLineParser.isSet(helpCommandLineOption))
      {
         QMessageBox::information(nullptr, "Help", commandLineParser.helpText());
      }
      else if (commandLineParser.isSet(versionCommandLineOption))
      {
         QMessageBox::information(nullptr, "Version", application.applicationVersion());
      }
      else
      {
         //
         // Try to acquire the application lock and exit immediately if it cannot be acquired (so that
         // just a single instance of the application is running at a time).
         //

         SystemLock lock;
         if (lock.tryLock())
         {
            //
            // Set up logging. If the log file cannot be openend (just) the default message handler is used.
            //

            QFile logFile(QStringLiteral("launcher.log"));
            if (logFile.open(QIODevice::Text | QIODevice::Truncate | QIODevice::WriteOnly))
            {
               logFileStream_.setDevice(&logFile);

               logMessageHandler_ = qInstallMessageHandler(logFileHandler);
            }

            qInfo().noquote() << application.applicationName() << application.applicationVersion();


            //
            // Show the search window.
            //

            ItemModel itemModel;

            SearchWindow searchWindow(&itemModel);
            searchWindow.show();

            //
            // Update the item model. Be aware that this must be done after creating the UI, so that
            // possible errors during the model update are properly indicated.
            //

            itemModel.read(commandLineParser.value(sourceCommandLineOption));


            //
            // Execute the application.
            //

            result = application.exec();
         }
      }
   }
   else
   {
      QMessageBox::critical(nullptr, QObject::tr("Invalid parameter"), commandLineParser.errorText());
   }

   return result;
}
