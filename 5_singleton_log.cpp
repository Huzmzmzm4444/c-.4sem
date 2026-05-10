#include <chrono>
#include <ctime>
#include <deque>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

enum LogLevel
{
    LOG_NORMAL,
    LOG_WARNING,
    LOG_ERROR
};

class Log
{
public:
    static Log* Instance()
    {
        static Log instance;
        return &instance;
    }

    void message(LogLevel level, const std::string& text)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (events_.size() == max_events_) {
            events_.pop_front();
        }
        events_.push_back(Event{std::chrono::system_clock::now(), level, text});
    }

    void print(std::ostream& out = std::cout) const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& event : events_) {
            out << formatTime(event.time_point) << " [" << levelToString(event.level) << "] " << event.text << '\n';
        }
    }

private:
    struct Event
    {
        std::chrono::system_clock::time_point time_point;
        LogLevel level;
        std::string text;
    };

    static constexpr std::size_t max_events_ = 10;

    Log() = default;
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;

    static std::string levelToString(LogLevel level)
    {
        switch (level) {
            case LOG_NORMAL:
                return "NORMAL";
            case LOG_WARNING:
                return "WARNING";
            case LOG_ERROR:
                return "ERROR";
        }
        return "UNKNOWN";
    }

    static std::string formatTime(const std::chrono::system_clock::time_point& tp)
    {
        const std::time_t time = std::chrono::system_clock::to_time_t(tp);
        std::tm local_tm{};
#ifdef _WIN32
        localtime_s(&local_tm, &time);
#else
        localtime_r(&time, &local_tm);
#endif
        std::ostringstream out;
        out << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S");
        return out.str();
    }

    mutable std::mutex mutex_;
    std::deque<Event> events_;
};

int main()
{
    Log* log = Log::Instance();

    log->message(LOG_NORMAL, "program loaded");
    log->message(LOG_WARNING, "configuration is partial");
    log->message(LOG_ERROR, "error happens! help me!");

    for (int i = 0; i < 12; ++i) {
        log->message(LOG_NORMAL, "event #" + std::to_string(i));
    }

    log->print();
    return 0;
}
