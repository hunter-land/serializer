#pragma once
#include <functional>
#include <vector>
#include <cstdint>
#include <string>
#include "endian.hpp"
#include "deserialLimits.hpp"

//A basic serializer, but with room to grow
#if __CHAR_BIT__ != 8
	#warning serialization may fail since the system uses chars which are not 8 bits
#endif

class serialIn {
public:
	typedef std::function<std::vector<uint8_t>(size_t bytesToFill)> fillFunction;
	static fillFunction fillFromFile(std::string inFile);
	static fillFunction fillFromVector(std::vector<uint8_t>& inVector);
protected:
	std::vector<uint8_t> m_inBuffer;

	fillFunction m_fillFunction;

	//Helpers
	void preDeserialize(size_t dataSize);

	template<typename T>
	void deserailizeIntegral(T& integer) {
		size_t valueWidth = sizeof(integer);
		integer = 0;
		//Make sure we have the number of bytes we need
		preDeserialize(valueWidth);

		//Remove data from buffer and place into the value
		uint8_t* byte = (uint8_t*)&integer;
		for (uint8_t i = 0; i < valueWidth; i++, byte++) {
			*byte = m_inBuffer[0];
			m_inBuffer.erase(m_inBuffer.begin());
		}

		//Fix endian
		integer = networkToHost(integer);
	}
public:
	serialIn(fillFunction filler);
	~serialIn() = default;

	///Plain-Old-Data///
	friend void deserialize(serialIn& serial, bool& bit, deserializationLimits limits);
	friend void deserialize(serialIn& serial, uint8_t& uint8, deserializationLimits limits);
	friend void deserialize(serialIn& serial, int8_t& int8, deserializationLimits limits);
	friend void deserialize(serialIn& serial, uint16_t& uint16, deserializationLimits limits);
	friend void deserialize(serialIn& serial, int16_t& int16, deserializationLimits limits);
	friend void deserialize(serialIn& serial, uint32_t& uint32, deserializationLimits limits);
	friend void deserialize(serialIn& serial, int32_t& int32, deserializationLimits limits);
	friend void deserialize(serialIn& serial, uint64_t& uint64, deserializationLimits limits);
	friend void deserialize(serialIn& serial, int64_t& int64, deserializationLimits limits);

	//Put more bytes into the internal buffer from the source
	//Returns actual number of bytes added
	size_t fill(size_t bytesToRequest);
};

void deserialize(serialIn& serial, bool& bit, deserializationLimits limits = defaultDeserializationLimits);
void deserialize(serialIn& serial, uint8_t& uint8, deserializationLimits limits = defaultDeserializationLimits);
void deserialize(serialIn& serial, int8_t& int8, deserializationLimits limits = defaultDeserializationLimits);
void deserialize(serialIn& serial, uint16_t& uint16, deserializationLimits limits = defaultDeserializationLimits);
void deserialize(serialIn& serial, int16_t& int16, deserializationLimits limits = defaultDeserializationLimits);
void deserialize(serialIn& serial, uint32_t& uint32, deserializationLimits limits = defaultDeserializationLimits);
void deserialize(serialIn& serial, int32_t& int32, deserializationLimits limits = defaultDeserializationLimits);
void deserialize(serialIn& serial, uint64_t& uint64, deserializationLimits limits = defaultDeserializationLimits);
void deserialize(serialIn& serial, int64_t& int64, deserializationLimits limits = defaultDeserializationLimits);
void deserialize(serialIn& serial, float& float32, deserializationLimits limits = defaultDeserializationLimits);
void deserialize(serialIn& serial, double& float64, deserializationLimits limits = defaultDeserializationLimits);
