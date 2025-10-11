/*
** ETIB PROJECT, 2025
** maverik
** File description:
** Logger
*/

#include "Logger.hpp"

maverik::vk::Logger::Logger(std::ostream &stream, const std::string& programName, const maverik::ALogger::Environment& env) : _stream(stream)
{
    _env = env == maverik::ALogger::DEV ? "DEV    " : "PROD   ";
    _programName = programName + "    ";
}

#if defined(__linux__)
void maverik::vk::Logger::log(const std::string &message, const std::string& logLevel, const std::string& caller) const
{
    std::string out = "";
    std::stringstream ss;
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    auto backtrace = Backtrace::getBacktrace(128, 4);
    std::string addr = "";
    std::string fmt = "";
    FILE *fp = nullptr;

    out += _env;
    out += _programName;
    out += logLevel;
    ss << std::put_time(&tm, "%b-%d %H:%M:%S    ");
    out += ss.str();
    out += message + "    \n\t";
    out += caller + "    \n";
    for (const auto& line : backtrace) {
        addr = line.substr(line.find(' ') + 2, line.find(' '));
        addr.erase(addr.find_last_of("]"));

        fmt = std::format("addr2line -C -s -f -p -e ./{} {}", BINARY_NAME, addr);
        fp = popen(fmt.c_str(), "r");
        if (fp == nullptr) {
            out += "\tError: Unable to execute addr2line command\n";
            continue;
        }
        char buffer[1024] = {0};
        fgets(buffer, 1024, fp);
        std::string lineAddr(buffer);
        size_t pos = lineAddr.find("(discriminator ");
        if (pos != std::string::npos) {
            lineAddr.erase(pos);
        }
        lineAddr.erase(lineAddr.find_last_not_of(" \n\t\r") + 1);
        out += "\tcalled by " + lineAddr + "\n";
        pclose(fp);
    }
    _stream << out;
    _stream.flush();
}
#elif defined(__APPLE__)
void maverik::vk::Logger::log(const std::string &message, const std::string& logLevel, const std::string& caller) const
{
    std::string out = "";
    std::stringstream ss;
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    auto backtrace = Backtrace::getBacktrace(128, 4);
    std::string addr = "";
    std::string fmt = "";
    FILE *fp = nullptr;

    out += _env;
    out += _programName;
    out += logLevel;
    ss << std::put_time(&tm, "%b-%d %H:%M:%S    ");
    out += ss.str();
    out += message + "    \n\t";
    out += caller + "    \n";
    for (const auto& line : backtrace) {
        out += "\t" + line;
    }
    _stream << out;
    _stream.flush();
}
// #elif defined(_WIN32)
// #elif defined(__ANDROID__)
#else
void maverik::vk::Logger::log(const std::string &message, const std::string& logLevel, const std::string& caller) const
{
    std::string out = "";
    std::stringstream ss;
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    out += _env;
    out += _programName;
    out += logLevel;
    ss << std::put_time(&tm, "%b-%d %H:%M:%S    ");
    out += ss.str();
    out += message + "    \n\t";
    out += caller + "    \n";
    out += "\tBacktrace not supported yet on this platform\n";
    _stream << out;
    _stream.flush();
}

#endif