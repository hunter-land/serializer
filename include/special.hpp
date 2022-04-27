#pragma once
#include <chrono>
#include "serialOut.hpp"
#include "serialIn.hpp"
#include <ctime>
#include <system_error>

//NOTE: TODO: These functions are only accurate to 1 second, anything less will be lost and change observed data
//NOTE: A clock's epoch can be gotten with `std::chrono::time_point<std::chrono::Clock>{}`

void serialize(serialOut& serial, tm time);

template <typename Clock, typename Duration>
void serialize(serialOut& serial, std::chrono::time_point<Clock, Duration> point) {
	constexpr bool isSecondsOrMore = ( Duration(1) >= std::chrono::seconds(1) );
	static_assert(isSecondsOrMore, "Currently only time_points with 1 second or worse accuracy is supported");
	//tm struct with UTC zone is desired
	//tm can be gotten in Unix/UTC from gmtime (or variation)
	//gmtime needs time_t
	//time_t can be got

	std::time_t tt = Clock::to_time_t(point);
	tm timeUnix;
	tm* e = gmtime_r(&tt, &timeUnix);
	if (e == nullptr) {
		//Error
		std::error_code ec = std::make_error_code(static_cast<std::errc>(errno));
		throw std::system_error(ec);
	}

	//We can now serialize timeUnix (tm structure)
	serialize(serial, timeUnix);
}

void deserialize(serialIn& serial, tm& time);

template <typename Clock, typename Duration>
void deserialize(serialIn& serial, std::chrono::time_point<Clock, Duration>& point) {
	constexpr bool isSecondsOrMore = ( Duration(1) >= std::chrono::seconds(1) );
	static_assert(isSecondsOrMore, "Currently only time_points with 1 second or worse accuracy is supported");
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
}
