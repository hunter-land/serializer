#pragma once
#include <functional>
#include <vector>
#include <cstdint>
#include <string>
#include "endian.hpp"

//A basic serializer, but with room to grow
#if __CHAR_BIT__ != 8
	#warning serialization may fail since the system uses chars which are not 8 bits
#endif

class serialOut {
public:
	typedef std::function<void(const std::vector<uint8_t>& buffer)> flushFunction;
	static flushFunction flushToFile(std::string outFile);
	static flushFunction flushToVector(std::vector<uint8_t>& outVector);
protected:
	std::vector<uint8_t> m_outBuffer;

	size_t m_flushSize; //Once the vector is this size or greater, flush it
	flushFunction m_flushFunction;
	size_t m_bytesFlushedCounter = 0; //For use outside of the class. Track how many bytes we have flushed (only increment automatically)

	//Helpers
	void postSerialize();

	template<typename T>
	void serailizeIntegral(T integer) {
		//Fix endian
		integer = hostToNetwork(integer);

		//Copy from integral into buffer
		for (size_t i = 0; i < sizeof(integer); i++) {
			uint8_t* bytePointer = (uint8_t*)&integer;
			m_outBuffer.push_back(*(bytePointer + i));
		}

		//Flush if large enough buffer
		postSerialize();
	}
public:
	serialOut(flushFunction flusher, size_t flushSize = 0x400);
	~serialOut();

	//Plain-Old-Data
	friend void serialize(serialOut& serial, bool bit);
	friend void serialize(serialOut& serial, uint8_t uint8);
	friend void serialize(serialOut& serial, int8_t int8);
	friend void serialize(serialOut& serial, uint16_t uint16);
	friend void serialize(serialOut& serial, int16_t int16);
	friend void serialize(serialOut& serial, uint32_t uint32);
	friend void serialize(serialOut& serial, int32_t int32);
	friend void serialize(serialOut& serial, uint64_t uint64);
	friend void serialize(serialOut& serial, int64_t int64);

	//Flush the internal buffer to its destination
	void flush();

	const std::vector<uint8_t>& internalOutBuffer() const;
	size_t outCounter() const;
	void resetOutCounter();
};

void serialize(serialOut& serial, bool bit);
void serialize(serialOut& serial, uint8_t uint8);
void serialize(serialOut& serial, int8_t int8);
void serialize(serialOut& serial, uint16_t uint16);
void serialize(serialOut& serial, int16_t int16);
void serialize(serialOut& serial, uint32_t uint32);
void serialize(serialOut& serial, int32_t int32);
void serialize(serialOut& serial, uint64_t uint64);
void serialize(serialOut& serial, int64_t int64);
void serialize(serialOut& serial, float float32);
void serialize(serialOut& serial, double float64);
