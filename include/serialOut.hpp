#pragma once
#include <functional>
#include <vector>
#include <cstdint>
#include <string>

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

	//Helpers
	void postPush();
public:
	serialOut(flushFunction flusher, size_t flushSize = 0x400);
	~serialOut();

	//Plain-Old-Data
	friend void push(serialOut& serial, bool bit);
	friend void push(serialOut& serial, uint8_t byte);
	friend void push(serialOut& serial, uint16_t uint16);
	friend void push(serialOut& serial, uint32_t uint32);
	friend void push(serialOut& serial, uint64_t uint64);
	friend void push(serialOut& serial, float float32);
	friend void push(serialOut& serial, double float64);

	//Flush the internal buffer to its destination
	void flush();
};

void push(serialOut& serial, bool bit);
void push(serialOut& serial, uint8_t byte);
void push(serialOut& serial, uint16_t uint16);
void push(serialOut& serial, uint32_t uint32);
void push(serialOut& serial, uint64_t uint64);
void push(serialOut& serial, float float32);
void push(serialOut& serial, double float64);
