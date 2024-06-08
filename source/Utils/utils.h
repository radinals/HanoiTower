#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <string>
#include <tuple>

namespace Utils {

    static inline std::tuple<long long int, long long int, long long int>
    extractTimeFromMs(long long int ms)
    {
        long long int total_seconds, hours, minutes, seconds;

        total_seconds = ms / 1000;
        hours         = total_seconds / 3600;           // Calculate hours
        minutes       = (total_seconds % 3600) / 60;    // Calculate minutes
        seconds       = (ms % 60000) / 1000.0;          // Calculate seconds

        return { hours, minutes, seconds };
    }

    static inline long long int
    extractMsFromTime(long long int hh, long long int mm, long long int ss)
    {
        return (hh * 3600000) + (mm * 60000) + (ss * 1000);
    }

    // convert the numeric labels of stacks to alphabets
    static inline QString numToChar(size_t n)
    {
        std::string str;
        str += char('A' + n);
        return QString::fromStdString(str);
    };

};    // namespace Utils

#endif    // !UTILS_H
