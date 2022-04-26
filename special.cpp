#include "include/special.hpp"
#include <chrono>
#include "include/serialOut.hpp"
#include "include/serialIn.hpp"
#include <ctime>

/*void setbits(void* data, size_t first, size_t last, size_t value) {
	//How many bits of value to use
	size_t width = last - first;
	size_t valueMask = 0;
	for (size_t i = 0; i < width; i++) {
		valueMask <<= 1; //Shift any other bits up
		valueMask |= 0b1; //Set this bit to true
	}
	value &= valueMask;

	size_t byteIndex = first / 8;
	size_t bitOfByte = first % 8;
	uint8_t* dataByte = (uint8_t*)data;
	dataByte += byteIndex;
}*/

void serialize(serialOut& serial, tm time) {
	/*tm structure mapping (all integers):
	| tm_sec   | seconds          [0, 60] | 6  bits |
	| tm_min   | minutes          [0, 59] | 6  bits |
	| tm_hour  | hours            [0, 23] | 5  bits |

	| tm_mday  | day of month     [1, 31] | 5  bits |
	| tm_mon   | months           [0, 11] | 4  bits |
	| tm_year  | years since 1900         | 32 bits |

	| tm_wday  | days since sun   [0, 6]  | 3  bits |
	| tm_yday  | day of the year [0, 365] | 9  bits |
	| tm_isdst | daylight savings [0, 1]  | 1  bit  |
	*/
	//total of 6+6  5+5  4+3  9+1  32
	//         12   10   7    10   32
	//         22        17        32
	//         39                  32
	//         71 bits to store each field with minimum bits
	//Each field as a multiple of 8-bits leads to 96 bits (12 bytes) instead of 72 (9 bytes)
	//We can shave off 1/4 of size if we want by stacking fields end-to-end

	//For now we dont do anything bit-wise, and just use the smallest integer type (bool exception)
	serialize(serial, (uint8_t)time.tm_sec);
	serialize(serial, (uint8_t)time.tm_min);
	serialize(serial, (uint8_t)time.tm_hour);
	serialize(serial, (uint8_t)time.tm_mday);
	serialize(serial, (uint8_t)time.tm_mon);
	serialize(serial, (int32_t)time.tm_year);

	serialize(serial, (uint8_t)time.tm_wday);
	serialize(serial, (uint8_t)time.tm_yday);
	serialize(serial, (bool)time.tm_isdst);
}

void deserialize(serialIn& serial, tm& time) {
	uint8_t byte;
	uint32_t uint32;
	bool boolean;

	deserialize(serial, byte);
	time.tm_sec = byte;
	deserialize(serial, byte);
	time.tm_min = byte;
	deserialize(serial, byte);
	time.tm_hour = byte;

	deserialize(serial, byte);
	time.tm_mday = byte;
	deserialize(serial, byte);
	time.tm_mon = byte;
	deserialize(serial, uint32);
	time.tm_year = uint32;

	deserialize(serial, byte);
	time.tm_wday = byte;
	deserialize(serial, byte);
	time.tm_yday = byte;
	deserialize(serial, boolean);
	time.tm_isdst = boolean ? 1 : 0;
}
