#ifndef UTILS_H
#define UTILS_H

#include <tuple>

namespace Utils
{
	static std::tuple<long long int, long long int, long long int>
	extractTimeFromMs(long long int ms)
	{
		long long int total_seconds, hours, minutes, seconds;

		total_seconds = ms / 1000;

		// Calculate hours
		hours = total_seconds / 3600;

		// Calculate remaining minutes
		minutes = (total_seconds % 3600) / 60;

		// Calculate remaining seconds
		seconds = (ms % 60000) / 1000.0;

		return {hours, minutes, seconds};
	}

	static inline long long int extractMsFromTime(long long int hh,
	                                              long long int mm,
	                                              long long int ss)
	{
		return (hh * 3600000) + (mm * 60000) + (ss * 1000);
	}

}; // namespace Utils

#endif // !UTILS_H