/*
** ETIB PROJECT, 2025
** maverik
** File description:
** Logger
*/

#pragma once

#include "ALogger.hpp"
#include <cstdio>
#include <format>
#include <vector>

#if defined(__linux__) || defined(__APPLE__)
extern const char *BINARY_NAME;
#include <cxxabi.h>
#include <execinfo.h>
#elif defined(_WIN32)
#include <windows.h>
#include <dbghelp.h>
#endif

/**
 * @namespace maverik
 */
namespace maverik {
    /**
     * @namespace vk
     */
    namespace vk {
        /**
         * @class Logger
         * @brief The `maverik::vk::Logger` class is a concrete implementation of the `maverik::ALogger` interface, designed to log messages to a specified output stream. It provides functionality to log messages with a log level and caller information, and it supports initialization with a program name and an optional environment setting.
         */
        class Logger : public maverik::ALogger {
            public:
                /**
                 * @brief Construct a new Logger object
                 *
                 * @param stream The `std::ostream` reference to the output stream where log messages will be written (e.g., `std::cout`, `std::cerr`).
                 * @param programName The name of the program using the logger.
                 * @param env The environment in which the logger operates (e.g., `DEV` or `PROD`). Default is `DEV`.
                 */
                Logger(std::ostream &stream, const std::string& programName, const Environment& env = DEV);

                /**
                 * @brief Destroy the Logger object
                 */
                ~Logger() override = default;

                /**
                 * @brief The log method in the `maverik::vk::Logger` class is a virtual function that overrides a base class method. It logs a message with a specified log level and caller information, taking three constant string references as parameters and ensuring no modification to the class state due to its const qualifier.
                 * @param message The message to log.
                 * @param logLevel The log level (e.g., ERROR, WARNING, INFO, DEBUG).
                 * @param caller The name of the function or method that called this function.
                 */
                void log(const std::string &message, const std::string& logLevel, const std::string& caller) const override;
            private:
                std::ostream &_stream;      ///< A reference to the output stream where log messages will be written
        };

        class Backtrace {
            public:
                #if defined(__linux__) || defined(__APPLE__)
                static std::vector<std::string> getBacktrace(int size = 128, int skip = 0) {
                    void **array = (void **)malloc(sizeof(void *) * size);
                    size_t count = backtrace(array, size);
                    char **strings = backtrace_symbols(array, count);
                    std::vector<std::string> result;
                    for (size_t i = skip; i < count; ++i) {
                        result.push_back(strings[i]);
                    }
                    for (int i = 0; i < 3; i++) {
                        result.pop_back();
                    }

                    free(strings);
                    free(array);
                    return result;
                }
            #elif defined(_WIN32)
                static std::vector<std::string> getBacktrace(int size = 128, int skip = 0) {
                    // Windows-specific implementation
                    return {};
                }
            #endif
        };
    }
}