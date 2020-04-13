#include "scopedtimer.h"

inline QDebug operator<<(QDebug dbg, const std::string& str)
{
    dbg.nospace() << QString::fromStdString(str);
    return dbg.space();
}

ScopedTimer::ScopedTimer()
{
    std::cout << "Default Ctor\n";
}

ScopedTimer::ScopedTimer(const std::string& ver) {
    std::cout << "Running " << ver << " version...\n";
    start = std::chrono::high_resolution_clock::now();
}

ScopedTimer::~ScopedTimer() {
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Version took " << duration.count() << " seconds.\n";
}

QScopedTimer::QScopedTimer(const std::string &in)
{
    qDebug() << "Running " << in << " version...\n";
    start = std::chrono::high_resolution_clock::now();
}

QScopedTimer::~QScopedTimer()
{
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    qDebug()<< "Version took " << duration.count() << " seconds.\n";
}
