#include <algorithm>
#include <cstdint>
extern "C" {
	#include <endian.h>
}

#ifdef _WIN32
	//Windows x86/x64 is always little endian, but does not define `BYTE_ORDER` or `LITTLE_ENDIAN`, so it is done here explicitly
	#define LITTLE_ENDIAN 7
	#define BYTE_ORDER LITTLE_ENDIAN
#endif
//Network byte order is big endian
//Typically (but not always) host byte order is little endian

template<typename T>
T hostToNetwork(T data) {
	#if BYTE_ORDER == BIG_ENDIAN
		//Already in network order, nothing to do
	#elif BYTE_ORDER == LITTLE_ENDIAN
		//Need to reverse order from host order (little) to network order (big)
		std::reverse((uint8_t*)&data, (uint8_t*)&data + sizeof(data));
	#else
		#error Please define BYTE_ORDER in serializer.cpp to either LITTLE_ENDIAN or BIG_ENDIAN
	#endif

	return data;
}

template<typename T>
T networkToHost(T data) {
	//Reversing a reversal is just reversing, so the same function can be used
	//This is a seperate function for readability where it is used
	return hostToNetwork(data);
}
