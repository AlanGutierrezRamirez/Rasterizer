#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>

class Profiler {
public:
    static Profiler& get();
    void addSample(const std::string& name, double milliseconds);

    void frameEnd();

    void setReportInterval(int frames) { reportEvery = frames; }

private:
    Profiler() = default;
    void printReport();

    std::unordered_map<std::string, double> accumulated;
    int frameCount = 0;
    int reportEvery = 60;
};


class ScopedTimer {
public:
    ScopedTimer(const std::string& name);
    ~ScopedTimer();

private:
    std::string name;
    Uint64 startTicks;
};