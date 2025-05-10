/*
** ETIB PROJECT, 2025
** maverik
** File description:
** ALogger
*/

#pragma once

#include <cstdint>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>

#define PROPER_FILE std::string(__FILE__).substr(std::string(__FILE__).find_last_of("/\\") + 1)
#define LOCATION std::string("In ") + std::string(__PRETTY_FUNCTION__) + std::string(" at ") + PROPER_FILE + ":" + std::to_string(__LINE__)

/**
 * @brief The `LOG` macro is a utility for logging messages at various severity levels (`ERROR`, `WARNING`, `INFO`, `DEBUG`, `FATAL`) using a `maverik::ALogger` instance. If an unknown log level is provided, it outputs an error message to `std::cerr` along with the unrecognized level and message.
 * @param level The log level (e.g., `maverik::ALogger::ERROR`, `maverik::ALogger::WARNING`, etc.).
 * @param message The message to log.
 * @note This macro uses the `logger` instance of type `std::shared_ptr<maverik::ALogger>` to log messages.
 */
#define LOG(level, message) \
    do { \
        if (level == maverik::ALogger::ERROR) { \
            logger->error(message, LOCATION); \
        } else if (level == maverik::ALogger::WARNING) { \
            logger->warning(message, LOCATION); \
        } else if (level == maverik::ALogger::INFO) { \
            logger->info(message, LOCATION); \
        } else if (level == maverik::ALogger::DEBUG) { \
            logger->debug(message, LOCATION); \
        } else if (level == maverik::ALogger::FATAL) { \
            logger->fatal(message, LOCATION); \
        } else { \
            std::cerr << "Unknown log level: " << level << " but got this message :\"" << message << "\"" << std::endl; \
        } \
    } while (0)

/**
** @namespace maverik
*/
namespace maverik {
    /**
     * @class ALogger
     * @brief The `maverik::ALogger` class is an abstract base class that defines a common interface for logging messages at various levels (fatal, error, warning, info, debug) in different environments (development or production). It includes pure virtual methods for logging and a protected pure virtual log method to be implemented by derived classes, along with member variables for environment and program name.
     */
    class ALogger {
        public:
            /**
             * @enum Environment
             * @brief The `maverik::ALogger::Environment` enumeration defines the possible environments for logging, with `DEV` representing the development environment and `PROD` representing the production environment. It is part of the `maverik::ALogger` class, which serves as an abstract base class for logging messages at various levels.
             * @var DEV Represents the development environment.
             * @var PROD Represents the production environment.
             */
            enum Environment {
                DEV,            ///< Development environment
                PROD            ///< Production environment
            };

            /**
             * @enum LogLevel
             * @brief The `maverik::ALogger::LogLevel` enumeration defines the severity levels for logging messages, including `FATAL`, `ERROR`, `WARNING`, `INFO`, and `DEBUG`, to categorize log entries in the `maverik::ALogger` abstract base class, which provides a common interface for logging in various environments.
             * @var FATAL Represents a fatal error that causes the program to terminate.
             * @var ERROR Represents an error that may not cause termination but indicates a significant issue.
             * @var WARNING Represents a warning that indicates a potential problem but does not require immediate action.
             * @var INFO Represents informational messages that provide general information about the program's execution.
             * @var DEBUG Represents debug messages that provide detailed information for debugging purposes.
             */
            enum LogLevel {
                FATAL,          ///< Fatal error that causes the program to terminate
                ERROR,          ///< Error that may not cause termination but indicates a significant issue
                WARNING,        ///< Warning that indicates a potential problem but does not require immediate action
                INFO,           ///< Informational messages that provide general information about the program's execution
                DEBUG           ///< Debug messages that provide detailed information for debugging purposes
            };

            /**
             * @brief Destroy the ALogger object
             */
            virtual ~ALogger();

            /**
             * @brief The `maverik::ALogger::fatal` method is a const virtual function that logs a fatal error message. It takes two std::string parameters: message, which contains the error details, and caller, which specifies the function or method that called it.
             * @param message The message to log.
             * @param caller The name of the function or method that called this function.
             */
            virtual void fatal(const std::string &message, const std::string& caller) const;

            /**
             * @brief The `maverik::ALogger::error` method is a const virtual function that logs an error message. It takes two std::string parameters: message, which contains the error details, and caller, which specifies the function or method that called it.
             * @param message The message to log.
             * @param caller The name of the function or method that called this function.
             */
            virtual void error(const std::string &message, const std::string& caller) const;

            /**
             * @brief The `maverik::ALogger::warning` method is a const virtual function that logs a warning message. It takes two std::string parameters: message, which contains the error details, and caller, which specifies the function or method that called it.
             * @param message The message to log.
             * @param caller The name of the function or method that called this function.
             */
            virtual void warning(const std::string &message, const std::string& caller) const;

            /**
             * @brief The `maverik::ALogger::info` method is a const virtual function that logs an info message. It takes two std::string parameters: message, which contains the error details, and caller, which specifies the function or method that called it.
             * @param message The message to log.
             * @param caller The name of the function or method that called this function.
             */
            virtual void info(const std::string &message, const std::string& caller) const;

            /**
             * @brief The `maverik::ALogger::debug` method is a const virtual function that logs a debug message. It takes two std::string parameters: message, which contains the error details, and caller, which specifies the function or method that called it.
             * @param message The message to log.
             * @param caller The name of the function or method that called this function.
             */
            virtual void debug(const std::string &message, const std::string& caller) const;

        protected:
            /**
             * @brief The `maverik::ALogger::log` method is a const virtual function that logs a message with a specified log level and caller information. It takes three `const std::string&` parameters: message for the log content, logLevel to indicate the severity or type of the log, and caller to specify the origin of the log entry.
             * @param message The message to log.
             * @param logLevel The log level (e.g., ERROR, WARNING, INFO, DEBUG).
             * @param caller The name of the function or method that called this function.
             */
            virtual void log(const std::string &message, const std::string& logLevel, const std::string& caller) const = 0;

            std::string _env;           ///< The environment in which the logger operates (e.g., DEV or PROD)
            std::string _programName;   ///< The name of the program using the logger
    };
}

extern std::shared_ptr<maverik::ALogger> logger;    ///< Global logger instance