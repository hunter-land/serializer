#pragma once
#include <functional>
#include <vector>
#include <cstdint>
#include <string>

//A basic serializer, but with room to grow
#if __CHAR_BIT__ != 8
	#warning serialization may fail since the system uses chars which are not 8 bits
#endif

class serialIn;

template<typename T>
T pop(serialIn&);

class serialIn {
public:
	typedef std::function<std::vector<uint8_t>(size_t bytesToFill)> fillFunction;
	static fillFunction fillFromFile(std::string inFile);
	static fillFunction fillFromVector(std::vector<uint8_t>& inVector);
protected:
	std::vector<uint8_t> m_inBuffer;

	fillFunction m_fillFunction;

	//Helpers
	void prePop(size_t dataSize);
public:
	serialIn(fillFunction filler);
	~serialIn() = default;

	//Plain-Old-Data
	friend void pop(serialIn& serial, bool& bit);
	friend void pop(serialIn& serial, uint8_t& uint8);
	friend void pop(serialIn& serial, uint16_t& uint16);
	friend void pop(serialIn& serial, uint32_t& uint32);
	friend void pop(serialIn& serial, uint64_t& uint64);
	friend void pop(serialIn& serial, float& float32);
	friend void pop(serialIn& serial, double& float64);

	//Put more bytes into the internal buffer from the source
	//Returns actual number of bytes added
	size_t fill(size_t bytesToRequest);
};

void pop(serialIn& serial, bool& bit);
void pop(serialIn& serial, uint8_t& uint8);
void pop(serialIn& serial, uint16_t& uint16);
void pop(serialIn& serial, uint32_t& uint32);
void pop(serialIn& serial, uint64_t& uint64);
void pop(serialIn& serial, float& float32);
void pop(serialIn& serial, double& float64);
