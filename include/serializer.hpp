#pragma once
#include "serialOut.hpp"
#include "serialIn.hpp"

//A basic serializer, but with room to grow

class serialDuplex : public serialIn, public serialOut {
public:
	serialDuplex(fillFunction filler, flushFunction flusher, size_t flushSize = 0x400);
	~serialDuplex() = default;
};
