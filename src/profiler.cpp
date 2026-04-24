#include "profiler.h"
#include <algorithm>
#include <cstdio>
#include <vector>

Profiler& Profiler::get() {
    static Profiler instance;
    return instance;
}

void Profiler::addSample(const std::string& name, double milliseconds) {
    accumulated[name] += milliseconds;
}

void Profiler::frameEnd() {
    frameCount++;
    if (frameCount >= reportEvery) {
        printReport();
        accumulated.clear();
        frameCount = 0;
    }
}

void Profiler::printReport() {
    std::vector<std::pair<std::string, double>> entries(
        accumulated.begin(), accumulated.end()
    );

    double frameTotal = 0.0;
    double measuredSum = 0.0;
    for (const auto& e : entries) {
        if (e.first == "frame_total") {
            frameTotal = e.second;
        } else {
            measuredSum += e.second;
        }
    }
    double totalForPct = (frameTotal > 0.0) ? frameTotal : measuredSum;

    entries.erase(
        std::remove_if(entries.begin(), entries.end(),
            [](const std::pair<std::string, double>& e) {
                return e.first == "frame_total";
            }),
        entries.end()
    );

    std::sort(entries.begin(), entries.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; }
    );

    double invFrames = 1.0 / (double)frameCount;

    printf("\n=== Profile (sorted by time, avg over %d frames) ===\n", frameCount);
    for (const auto& e : entries) {
        double avg = e.second * invFrames;
        double pct = (totalForPct > 0.0) ? (e.second / totalForPct * 100.0) : 0.0;

        int bars = (int)(pct / 5.0);
        char barStr[32] = {0};
        for (int i = 0; i < bars && i < 20; i++) barStr[i] = '#';

        printf("  %-14s %7.2f ms  (%5.1f%%)  %s\n",
               e.first.c_str(), avg, pct, barStr);
    }

    printf("  ----------------------------------------\n");
    if (frameTotal > 0.0) {
        double avgMeasured = measuredSum * invFrames;
        double avgFrame    = frameTotal  * invFrames;
        double pctMeasured = measuredSum / frameTotal * 100.0;
        double fps         = (avgFrame > 0.0) ? (1000.0 / avgFrame) : 0.0;

        printf("  %-14s %7.2f ms  (%5.1f%%)\n", "measured", avgMeasured, pctMeasured);
        printf("  %-14s %7.2f ms  (~%.0f FPS)\n", "frame_total", avgFrame, fps);
    } else {
        double avg = measuredSum * invFrames;
        double fps = (avg > 0.0) ? (1000.0 / avg) : 0.0;
        printf("  %-14s %7.2f ms  (~%.0f FPS)\n", "total (measured)", avg, fps);
    }
    printf("\n");
    fflush(stdout);
}

ScopedTimer::ScopedTimer(const std::string& n) : name(n) {
    startTicks = SDL_GetPerformanceCounter();
}

ScopedTimer::~ScopedTimer() {
    Uint64 endTicks = SDL_GetPerformanceCounter();

    double freq = (double)SDL_GetPerformanceFrequency();
    double ms = (double)(endTicks - startTicks) * 1000.0 / freq;
    Profiler::get().addSample(name, ms);
}