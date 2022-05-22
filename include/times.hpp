#pragma once
#include <chrono>
#include "serialOut.hpp"
#include "serialIn.hpp"
#include <ctime>
#include <system_error>

void serialize(serialOut& serial, const tm& time);

template <typename Rep, typename Period>
void serialize(serialOut& serial, const std::chrono::duration<Rep, Period>& duration) {
	//As long as count() isn't past the bounds of an int64_t we are good
	if (duration.count() > std::numeric_limits<int64_t>::max() || duration.count() < std::numeric_limits<int64_t>::min()) {
		throw std::runtime_error("Duration is too long (or short) to serialize");
	}

	int64_t durationIntegral = duration.count();
	serialize(serial, durationIntegral);
}

template <typename Clock, typename Duration>
void serialize(serialOut& serial, const std::chrono::time_point<Clock, Duration>& point) {
	//constexpr bool isSecondsOrMore = ( Duration(1) >= std::chrono::seconds(1) );
	//static_assert(isSecondsOrMore, "Currently only time_points with 1 second or worse accuracy is supported");

	//tm struct with UTC zone is desired
	//tm can be gotten in Unix/UTC from gmtime (or variation)
	//gmtime needs time_t
	//time_t can be got

	std::time_t tt = Clock::to_time_t(std::chrono::time_point_cast<std::chrono::seconds>(point));
	tm timeUnix;
	tm* e = gmtime_r(&tt, &timeUnix);
	if (e != &timeUnix) {
		//Error
		std::error_code ec = std::make_error_code(static_cast<std::errc>(errno));
		throw std::system_error(ec);
	}

	//We can now serialize timeUnix (tm structure)
	serialize(serial, timeUnix);

	//Now for getting more accuracy, we have to do some shenanigans. It increases serialize time but it garuntees accuracy, so we do it only when needed
	//Long processes made short, we convert to less accurate version (second accuracy) and then back as same type, then get the difference in nanoseconds
	std::time_t ttRedone = mktime(&timeUnix);
	ttRedone -= timezone;
	auto pointRedone = std::chrono::time_point_cast<Duration>(Clock::from_time_t(ttRedone)); //pointRedone is the fully re-casted value of our original input, now the heart of the trick
	std::chrono::nanoseconds nanosecondsThisSecond = point - pointRedone;
	serialize(serial, nanosecondsThisSecond);
}

void deserialize(serialIn& serial, tm& time, deserializationLimits limits = defaultDeserializationLimits);

template <typename Rep, typename Period>
void deserialize(serialIn& serial, std::chrono::duration<Rep, Period>& duration, deserializationLimits limits = defaultDeserializationLimits) {
	int64_t durationIntegral;
	deserialize(serial, durationIntegral, limits);

	std::chrono::duration<int64_t, Period> durationGot(durationIntegral);
	duration = std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(durationGot);
}

template <typename Clock, typename Duration>
void deserialize(serialIn& serial, std::chrono::time_point<Clock, Duration>& point, deserializationLimits limits = defaultDeserializationLimits) {
	//constexpr bool isSecondsOrMore = ( Duration(1) >= std::chrono::seconds(1) );
	//static_assert(isSecondsOrMore, "Currently only time_points with 1 second or worse accuracy is supported");

	//tm is used at the base, so we must convert from a tm structure into our chrono point
	//tm is stored in Unix/UTC time, and must be converted to a local time_t
	//unfortunatley, there is no POSIX inverse of gmtime (or variation), so we must do a bit more work here
	//the trick is knowing of tzset and the timezone global. the timezone global is the number of seconds difference between UTC and local timezones
	//NOTE: mktime will do the setting (as if we called tzset) for us, which is why we do not explicitly call it ourself anywhere here
	//So in summary:
	//Get the Unix/UTC tm structure
	//Convert it to a time_t
	//Apply the timezone offset
	//Convert to a chrono point
	//Convert chrono point to correct precision

	//Deserialize timeUnix (tm structure)
	tm timeUnix;
	deserialize(serial, timeUnix);

	std::time_t tt = mktime(&timeUnix);
	tt -= timezone;
	auto pointGeneric = Clock::from_time_t(tt);
	point = std::chrono::time_point_cast<Duration>(pointGeneric);

	//Deserialize nanoseconds within the second (nanoseconds % 1 second)
	std::chrono::nanoseconds nanosecondsThisSecond;
	deserialize(serial, nanosecondsThisSecond);
	point += std::chrono::duration_cast<Duration>(nanosecondsThisSecond);
}
