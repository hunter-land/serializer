#include "include/serialOut.hpp"
#include <cstdint>
#include <limits>

serialOut::serialOut(flushFunction flusher, size_t flushSize) {
	m_flushFunction = flusher;
	m_flushSize = flushSize;
}

serialOut::~serialOut() {
	flush(); //Flush any remaining data
}

void serialOut::postSerialize() {
	if (m_outBuffer.size() >= m_flushSize) {
		//Flush buffer out
		m_flushFunction(m_outBuffer);
	}
}

void serialize(serialOut& serial, bool bit) {
	//Copy from integral into buffer
	serial.m_outBuffer.push_back(bit == true ? 1 : 0);

	//Flush if large enough buffer
	serial.postSerialize();
}
void serialize(serialOut& serial, uint8_t uint8) {
	serial.serailizeIntegral(uint8);
}
void serialize(serialOut& serial, int8_t int8) {
	serial.serailizeIntegral(int8);
}
void serialize(serialOut& serial, uint16_t uint16) {
	serial.serailizeIntegral(uint16);
}
void serialize(serialOut& serial, int16_t int16) {
	serial.serailizeIntegral(int16);
}
void serialize(serialOut& serial, uint32_t uint32) {
	serial.serailizeIntegral(uint32);
}
void serialize(serialOut& serial, int32_t int32) {
	serial.serailizeIntegral(int32);
}
void serialize(serialOut& serial, uint64_t uint64) {
	serial.serailizeIntegral(uint64);
}
void serialize(serialOut& serial, int64_t int64) {
	serial.serailizeIntegral(int64);
}

void serialize(serialOut& serial, float float32) {
	//We expect floats of IEEE-754/IEC60559 definition with 2 radix and 32-bits long
	constexpr bool isIEEE754 = std::numeric_limits<float>::is_iec559 && std::numeric_limits<float>::radix == 2 && sizeof(float) == 4;
	static_assert(isIEEE754, "System is not using IEEE754 floating points of 32 bits");

	//We can do a simple conversion of the float to a uint32_t and use that overload
	uint32_t* floatPtr = (uint32_t*)&float32;
	serialize(serial, *floatPtr);
}

void serialize(serialOut& serial, double float64) {
	//We expect floats of IEEE-754/IEC60559 definition with 2 radix and 64-bits long
	constexpr bool isIEEE754 = std::numeric_limits<double>::is_iec559 && std::numeric_limits<double>::radix == 2 && sizeof(double) == 8;
	static_assert(isIEEE754, "System is not using IEEE754 (double) floating points of 64 bits");

	//We can do a simple conversion of the float to a uint32_t and use that overload
	uint64_t* floatPtr = (uint64_t*)&float64;
	serialize(serial, *floatPtr);
}

void serialOut::flush() {
	m_flushFunction(m_outBuffer);
	m_outBuffer.clear();
}

#include <fstream>
#include <memory>
serialOut::flushFunction serialOut::flushToFile(std::string outFile) {
	std::shared_ptr<std::ofstream> ofstream(new std::ofstream(outFile, std::ios::binary));

	return [ofstream](const std::vector<uint8_t>& buffer) {
		//Write entire buffer to file
		ofstream->write((const char*)buffer.data(), buffer.size());
	};
}
serialOut::flushFunction serialOut::flushToVector(std::vector<uint8_t>& outVector) {
	return [&outVector](const std::vector<uint8_t>& buffer) {
		//Append buffer to outVector
		outVector.insert(outVector.end(), buffer.begin(), buffer.end());
	};
}
