#include "include/endian.hpp"
#include <algorithm> //std::reverse
#include <cstdint> //uint8_t
extern "C" {
	#include <endian.h>
}

void hostToNetwork(void* data, size_t len) {
	#if BYTE_ORDER == BIG_ENDIAN
		//Already in network order, nothing to do
	#elif BYTE_ORDER == LITTLE_ENDIAN
		//Need to reverse order from host order (little) to network order (big)
		std::reverse((uint8_t*)data, (uint8_t*)data + len);
	#else
		#error Please define BYTE_ORDER in serializer.cpp to either LITTLE_ENDIAN or BIG_ENDIAN
	#endif
}

void networkToHost(void* data, size_t len) {
	//Reversing a reversal is just reversing, so the same function can be used
	//This is a seperate function for readability where it is used
	return hostToNetwork(data, len);
}
