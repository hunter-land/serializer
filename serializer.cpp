#include "include/serializer.hpp"

serialDuplex::serialDuplex::serialDuplex(serialIn::fillFunction filler, serialOut::flushFunction flusher, size_t flushSize) : serialIn(filler), serialOut(flusher, flushSize) {
	/*All construction handled by inhereted types*/
}
