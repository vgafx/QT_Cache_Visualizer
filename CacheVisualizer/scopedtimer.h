/*** Scoped Timer
 * Timing class taking advantage of object lifetime semantics to measure execution
 * time for specific regions of code. This class is not part of the final application,
 * It is only used for optimization measurements.
 ***/
#pragma once
#include <iostream>
#include <chrono>
#include <QDebug>

using namespace std::literals::chrono_literals;

class ScopedTimer {
protected:
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> duration;
public:
    ScopedTimer();
    ScopedTimer(const std::string& in);
    ~ScopedTimer();
};

class QScopedTimer : public ScopedTimer {

public:
    QScopedTimer(const std::string& in);
    ~QScopedTimer();
};
