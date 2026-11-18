#pragma once

#include <cstdio>
#include <format>
#include <iostream>
#include <print>
#include <mutex>

class Logger
{
public:
    Logger() = default;
    ~Logger() = default;

    Logger(Logger const& logger) = delete;
    Logger(Logger&& logger) = delete;
    Logger& operator=(Logger const& logger) = delete;
    Logger& operator=(Logger&& logger) = delete;

    template<typename... Args>
    static void LogMessage(std::format_string<Args...> _fmt, Args&&... _args)
    {
        s_logger.LogMessage_(_fmt, std::forward<Args>(_args)...);
    }

    template<typename... Args>
    static void LogInformation(std::format_string<Args...> _fmt, Args&&... _args)
    {
        s_logger.LogInformation_(_fmt, std::forward<Args>(_args)...);
    }

    template<typename... Args>
    static void LogSuccess(std::format_string<Args...> _fmt, Args&&... _args)
    {
        s_logger.LogSuccess_(_fmt, std::forward<Args>(_args)...);
    }

    template<typename... Args>
    static void LogError(std::format_string<Args...> _fmt, Args&&... _args)
    {
        s_logger.LogError_(_fmt, std::forward<Args>(_args)...);
    }

    static void LogError(const char* str)
    {
        s_logger.LogError_(str);
    }

private:
    template<typename... Args>
    void LogMessage_(std::format_string<Args...> _fmt, Args&&... _args)
    {
        std::scoped_lock lock{ m_mutex };
        try
        {
            std::print(_fmt, std::forward<Args>(_args)...);
            std::print("\n");
        }
        catch (std::format_error& e)
        {
            LogFormattingError(e);
        }
    }

    template<typename... Args>
    void LogInformation_(std::format_string<Args...> _fmt, Args&&... _args)
    {
        std::scoped_lock lock{ m_mutex };
        try
        {
            std::print("\033[93m");
            std::print(_fmt, std::forward<Args>(_args)...);
            std::print("\033[0m\n");
        }
        catch (std::format_error& e)
        {
            LogFormattingError(e);
        }
    }

    template<typename... Args>
    void LogSuccess_(std::format_string<Args...> _fmt, Args&&... _args)
    {
        std::scoped_lock lock{ m_mutex };
        try
        {
            std::print("\033[32m");
            std::print(_fmt, std::forward<Args>(_args)...);
            std::print("\033[0m\n");
        }
        catch (std::format_error& e)
        {
            LogFormattingError(e);
        }
    }

    template<typename... Args>
    void LogError_(std::format_string<Args...> _fmt, Args&&... _args)
    {
        std::scoped_lock lock{ m_mutex };
        try
        {
            std::print(std::cerr, _fmt, std::forward<Args>(_args)...);
            std::print(std::cerr, "\n");
        }
        catch (std::format_error& e)
        {
            LogFormattingError(e);
        }
    }

    void LogError_(const char* str)
    {
        std::scoped_lock lock{ m_mutex };
        perror(str);
    }

    static void LogFormattingError(const std::format_error& e)
    {
        std::cerr << "Formatting error: " << e.what() << '\n';
    }

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
    static Logger s_logger;
    std::mutex m_mutex;
};
