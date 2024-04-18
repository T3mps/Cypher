#pragma once

#include <format>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <source_location>

namespace Cypher
{
   enum class LogLevel
   {
      Trace,
      Debug,
      Info,
      Warn,
      Error,
      Fatal
   };

   class ILogger
   {
   public:
      virtual ~ILogger() = default;

      template<typename... Args>
      void Log(LogLevel level, const std::source_location& location, std::string_view format, Args&&... args)
      {
         auto formatted_message = std::vformat(format, std::make_format_args(std::forward<Args>(args)...));
         LogImpl(level, formatted_message, location);
      }

   protected:
      virtual void LogImpl(LogLevel level, const std::string& message, const std::source_location& location) = 0;
   };

   class Logger : public ILogger
   {
   protected:
      void LogImpl(LogLevel level, const std::string& message, const std::source_location& location) override
      {
         std::cout << "["<< ToString(level) << "]["
                   << location.file_name() << ":" << location.line()
                   << "] - " << message << std::endl;
      }

   private:
      constexpr const char* ToString(LogLevel level)
      {
         switch (level)
         {
            case LogLevel::Trace: return "TRACE";
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info:  return "INFO";
            case LogLevel::Warn:  return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Fatal: return "FATAL";
            default:              return "UNKNOWN";
         }
      }
   };

   struct LoggerDeleter
   {
      void operator()(ILogger* ptr) const { delete ptr; }
   };

   class LogManager
   {
   public:
      template<typename Deleter = LoggerDeleter>
      static void SetLogger(ILogger* newLogger, Deleter deleter = Deleter())
      {
         GetInstance().m_logger.reset(newLogger, deleter);
      }

      static ILogger& GetLogger()
      {
         return *(GetInstance().m_logger);
      }

   private:
      LogManager() : m_logger(new Logger(), LoggerDeleter()) {}
      ~LogManager() {}

      LogManager(const LogManager&) = delete;
      LogManager& operator=(const LogManager&) = delete;

      static LogManager& GetInstance()
      {
         static LogManager logManagerInstance;
         return logManagerInstance;
      }

      std::unique_ptr<ILogger, std::function<void(ILogger*)>> m_logger;
   };
}

#define LOG_TRACE(fmt, ...) Cypher::LogManager::GetLogger().Log(Cypher::LogLevel::Trace, std::source_location::current(), fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) Cypher::LogManager::GetLogger().Log(Cypher::LogLevel::Debug, std::source_location::current(), fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  Cypher::LogManager::GetLogger().Log(Cypher::LogLevel::Info,  std::source_location::current(), fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  Cypher::LogManager::GetLogger().Log(Cypher::LogLevel::Warn,  std::source_location::current(), fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) Cypher::LogManager::GetLogger().Log(Cypher::LogLevel::Error, std::source_location::current(), fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) Cypher::LogManager::GetLogger().Log(Cypher::LogLevel::Fatal, std::source_location::current(), fmt, ##__VA_ARGS__)
