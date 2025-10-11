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

#if defined(__linux__)
extern const char *BINARY_NAME;         ///< The name of the binary, used for logging
#include <cxxabi.h>
#include <execinfo.h>
#elif defined(__APPLE__)
#include <dlfcn.h>
#include <execinfo.h>
#include <stdio.h>
#elif defined(_WIN32)
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

        /**
         * @class Backtrace
         * @brief The `maverik::vk::Backtrace` class provides functionality to capture and retrieve the current backtrace of function calls in the program. It is designed to work on different platforms, including Linux and Windows, and provides a method to obtain a vector of strings representing the backtrace.
         * @note This class has one static method `getBacktrace`. This method definition is platform-dependent, and the implementation may vary based on the operating system.
         * @todo Implement the Windows-specific backtrace retrieval logic.
         */
        class Backtrace {
            public:
                #if defined(__linux__)
                /**
                 * @brief This function retrieves the current backtrace of function calls in the program. It captures a specified number of stack frames and returns them as a vector of strings. The `size` parameter specifies the maximum number of frames to capture, and the `skip` parameter allows skipping a specified number of frames from the beginning of the backtrace.
                 *
                 * @param size Specifies the maximum number of frames to capture. Default is 128.
                 * @param skip Specifies the number of frames to skip from the beginning of the backtrace (last call). Default is 0.
                 * @return std::vector<std::string> A vector of strings representing the backtrace, with each string containing information about a specific frame following the format: "./path/to/binary() [function address in binary]".
                 */
                static std::vector<std::string> getBacktrace(int size = 128, int skip = 0) {
                    void **array = static_cast<void**>(malloc(sizeof(void *) * size));
                    if (!array) {
                        throw std::runtime_error("Failed to allocate memory for backtrace array");
                    }
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
            #elif defined(__APPLE__)
                /**
                 * @brief This function retrieves the current backtrace of function calls in the program. It captures a specified number of stack frames and returns them as a vector of strings. The `size` parameter specifies the maximum number of frames to capture, and the `skip` parameter allows skipping a specified number of frames from the beginning of the backtrace.
                 *
                 * @param size Specifies the maximum number of frames to capture. Default is 128.
                 * @param skip Specifies the number of frames to skip from the beginning of the backtrace (last call). Default is 0.
                 * @return std::vector<std::string> A vector of strings representing the backtrace, with each string containing information about a specific frame following the format: "./path/to/binary() [function address in binary]".
                 */
                static std::vector<std::string> getBacktrace(int size = 128, int skip = 0) {
                    void **array = static_cast<void**>(malloc(sizeof(void *) * size));
                    if (!array) {
                        throw std::runtime_error("Failed to allocate memory for backtrace array");
                    }
                    std::vector<std::string> result;
                    size_t count = backtrace(array, size);

                    for (int i = skip; i < count - 1; i++) {
                        Dl_info info = {0};
                        dladdr(array[i], &info);
                        std::stringstream ss;
                        ss << "atos -o " << info.dli_fname << " -l " << std::hex << reinterpret_cast<uint64_t>(info.dli_fbase) << ' ' << reinterpret_cast<uint64_t>(array[i]);

                        FILE *fp = popen(ss.str().c_str(), "r");

                        if (!fp) {
                            free(array);
                            throw std::runtime_error("Failed to execute atos command");
                        }
                        char buffer[1024] = {0};
                        fgets(buffer, sizeof(buffer), fp);
                        std::string tmp = std::string(buffer);
                        std::cout << "buffer: " << tmp << std::endl;
                        size_t pos = tmp.find(" (in maverik) ");
                        if (pos != std::string::npos) {
                            tmp.erase(pos, std::string(" (in maverik) ").length());
                            tmp.insert(pos, " at ");
                        }
                        size_t start = tmp.find_last_of('(');
                        size_t end = tmp.find(')', start);
                        if (start != std::string::npos && end != std::string::npos && end > start) {
                            std::string filename = tmp.substr(start + 1, end - start - 1);
                            tmp.erase(start, end - start + 1);
                            tmp.insert(start, filename);
                        }
                        tmp.insert(0, "called by ");
                        size_t mainPos = tmp.find("called by main ");
                        if (mainPos != std::string::npos) {
                            tmp.replace(mainPos, std::string("called by main ").length(), "called by main() ");
                        }
                        result.push_back(tmp);
                        pclose(fp);
                    }
                    free(array);
                    return result;
                }
            #elif defined(_WIN32)
                /**
                 * @brief This function retrieves the current backtrace of function calls in the program. It captures a specified number of stack frames and returns them as a vector of strings. The `size` parameter specifies the maximum number of frames to capture, and the `skip` parameter allows skipping a specified number of frames from the beginning of the backtrace.
                 *
                 * @param size Specifies the maximum number of frames to capture. Default is 128.
                 * @param skip Specifies the number of frames to skip from the beginning of the backtrace (last call). Default is 0.
                 * @return std::vector<std::string> A vector of strings representing the backtrace, with each string containing information about a specific frame following the format: "./path/to/binary() [function address in binary]".
                 */
                static std::vector<std::string> getBacktrace(int size = 128, int skip = 0) {
                    // Windows-specific implementation
                    return {};
                }
            #else
                static std::vector<std::string> getBacktrace(int size = 128, int skip = 0) {
                    return {};
                }
            #endif
        };
    }
}