#include "include/serialIn.hpp"
#include <cstdint>
#include "endian.cpp"
#include <limits>

serialIn::serialIn(fillFunction filler) {
	m_fillFunction = filler;
}

void serialIn::prePop(size_t dataSize) {
	if (m_inBuffer.size() < dataSize) {
		fill(dataSize - m_inBuffer.size());
	}
}

void pop(serialIn& serial, bool& bit) {
	size_t valueWidth = sizeof(bit);

	//Make sure we have the number of bytes we need
	serial.prePop(valueWidth);

	//Remove data from buffer and place into the value
	uint8_t byte = serial.m_inBuffer.front();
	bit = byte == 0 ? false : true;
}

void pop(serialIn& serial, uint8_t& valueOut) {
	size_t valueWidth = sizeof(valueOut);

	//Make sure we have the number of bytes we need
	serial.prePop(valueWidth);

	//Remove data from buffer and place into the value
	uint8_t* byte = (uint8_t*)&valueOut;
	for (uint8_t i = 0; i < valueWidth; i++, byte++) {
		*byte = serial.m_inBuffer[0];
		serial.m_inBuffer.erase(serial.m_inBuffer.begin());
	}

	//Fix endian
	valueOut = networkToHost(valueOut);
}

void pop(serialIn& serial, uint16_t& valueOut) {
	size_t valueWidth = sizeof(valueOut);

	//Make sure we have the number of bytes we need
	serial.prePop(valueWidth);

	//Remove data from buffer and place into the value
	uint8_t* byte = (uint8_t*)&valueOut;
	for (uint8_t i = 0; i < valueWidth; i++, byte++) {
		*byte = serial.m_inBuffer[0];
		serial.m_inBuffer.erase(serial.m_inBuffer.begin());
	}

	//Fix endian
	valueOut = networkToHost(valueOut);
}

void pop(serialIn& serial, uint32_t& valueOut) {
	size_t valueWidth = sizeof(valueOut);

	//Make sure we have the number of bytes we need
	serial.prePop(valueWidth);

	//Remove data from buffer and place into the value
	uint8_t* byte = (uint8_t*)&valueOut;
	for (uint8_t i = 0; i < valueWidth; i++, byte++) {
		*byte = serial.m_inBuffer[0];
		serial.m_inBuffer.erase(serial.m_inBuffer.begin());
	}

	//Fix endian
	valueOut = networkToHost(valueOut);
}

void pop(serialIn& serial, uint64_t& valueOut) {
	size_t valueWidth = sizeof(valueOut);

	//Make sure we have the number of bytes we need
	serial.prePop(valueWidth);

	//Remove data from buffer and place into the value
	uint8_t* byte = (uint8_t*)&valueOut;
	for (uint8_t i = 0; i < valueWidth; i++, byte++) {
		*byte = serial.m_inBuffer[0];
		serial.m_inBuffer.erase(serial.m_inBuffer.begin());
	}

	//Fix endian
	valueOut = networkToHost(valueOut);
}

void pop(serialIn& serial, float& float32) {
	//We expect floats of IEEE-754/IEC60559 definition with 2 radix and 32-bits long
	constexpr bool isIEEE754 = std::numeric_limits<float>::is_iec559 && std::numeric_limits<float>::radix == 2 && sizeof(float) == 4;
	static_assert(isIEEE754, "System is not using IEEE754 floating points of 32 bits");

	//We can do a simple conversion of the float to a uint32_t and use that overload
	uint32_t intFloat;
	pop(serial, intFloat);
	float32 = *(float*)&intFloat;
}

void pop(serialIn& serial, double& float64) {
	//We expect floats of IEEE-754/IEC60559 definition with 2 radix and 64-bits long
	constexpr bool isIEEE754 = std::numeric_limits<double>::is_iec559 && std::numeric_limits<double>::radix == 2 && sizeof(double) == 8;
	static_assert(isIEEE754, "System is not using IEEE754 (double) floating points of 64 bits");

	//We can do a simple conversion of the float to a uint64_t and use that overload
	uint64_t intFloat;
	pop(serial, intFloat);
	float64 = *(double*)&intFloat;
}

#include <fstream>
#include <memory>
serialIn::fillFunction serialIn::fillFromFile(std::string inFile) {
	std::shared_ptr<std::ifstream> ifstream(new std::ifstream(inFile, std::ios::binary));

	return [ifstream](size_t bytesToFill) {
		//Read and return at least bytesToFill
		std::vector<uint8_t> buffer(bytesToFill);
		ifstream->read((char*)buffer.data(), buffer.size());
		buffer.resize(ifstream->gcount()); //Resize buffer to actual amount read
		return buffer;
	};
}
serialIn::fillFunction fillFromVector(std::vector<uint8_t>& inVector) {

	return [&inVector](size_t bytesToFill) {
		//return bytes from inVec
		std::vector<uint8_t> requestedBytes;

		for (size_t i = 0; i < bytesToFill && inVector.size() > 0; i++) {
			requestedBytes.push_back(inVector.front());
			inVector.erase(inVector.begin());
		}

		return requestedBytes;
	};
}
