#pragma once
#include "serialOut.hpp"
#include "serialIn.hpp"
#include "deserialLimits.hpp"
#include <limits>
#include <stdexcept>
//Supported containers
#include <vector> //vector
#include <string> //basic_string
#include <map> //map

/*
 *	This file has extra definitions for non-pod/primitive STL datatypes
 *
 *	Classes such as vectors, maps, and strings have their serialization defined in this file
 */

//max lengths and types
#ifndef serialSize_t
typedef uint32_t serialSize_t; //Width of unsigned integer used for container sizes
#endif
const uintmax_t serialSize_Max = std::numeric_limits<serialSize_t>::max(); //Maximum number of elements a container can have (size of contained type does not matter)

/*
 *	std::vector is seralized as below:
 *
 *	1) serialize the size as a serialSize_t
 *	2) serialize each element in order from indexes [0,n)
 **/
template<typename T>
void serialize(serialOut& serial, const std::vector<T>& vec) {
	//Make sure container is short enough
	if (vec.size() > serialSize_Max) {
		throw std::runtime_error("Cannot serialize containers longer than serialSize_Max");
	}

	//#1: Serialize size as serialSize_t
	serialSize_t size = vec.size();
	serialize(serial, size);

	//#2: Serialize all elements from indexes [0,n)
	for (size_t i = 0; i < vec.size(); i++) {
		serialize(serial, vec[i]);
	}
}

template<typename T>
void deserialize(serialIn& serial, std::vector<T>& vec, deserializationLimits limits = defaultDeserializationLimits) {
	//#1: Deserialize size as serialSize_t
	serialSize_t size;
	deserialize(serial, size);

	//#1.1: Limit elements to save (if we need to)
	size_t saveSize = std::min((size_t)size, limits.max); //Only save up to `max` elements, discard the rest

	//#1.2: Set container size
	vec.resize(saveSize);

	//#2: Deserialize all elements from indexes [0,n)
	size_t i = 0;
	for (; i < saveSize; i++) {
		deserialize(serial, vec[i], limits.childLimitOrSelf(0));
	}

	//#2.1: Pop any remaining elements to keep alignment
	for (; i < size; i++) {
		T tmp;
		deserialize(serial, tmp, deserializationLimits(0));
	}
}

/*
 *	std::basic_string is seralized as if it were a vector
 **/
template<typename T>
void serialize(serialOut& serial, const std::basic_string<T>& str) {
	const std::vector<uint8_t> vec(str.begin(), str.end());
	serialize(serial, vec);
}
template<typename T>
void deserialize(serialIn& serial, std::basic_string<T>& str, deserializationLimits limits = defaultDeserializationLimits) {
	std::vector<uint8_t> vec(str.begin(), str.end());
	deserialize(serial, vec, limits);
	str = std::basic_string<T>(vec.begin(), vec.end());
}

/*
 *	std::map is seralized as below:
 *
 *	1) serialize the size as a serialSize_t
 *	2) serialize each pair
 *	2.1) serialize the key
 *	2.2) serialize the value
 **/
template<typename T, typename U>
void serialize(serialOut& serial, const std::map<T, U>& map) {
	//Make sure container is short enough
	if (map.size() > serialSize_Max) {
		throw std::runtime_error("Cannot serialize containers longer than serialSize_Max");
	}

	//#1: Serialize size as serialSize_t
	serialSize_t size = map.size();
	serialize(serial, size);

	//#2: Serialize all pairs
	for (const auto& pair : map) {
		serialize(serial, pair.first);
		serialize(serial, pair.second);
	}
}

template<typename T, typename U>
void deserialize(serialIn& serial, std::map<T, U>& map, deserializationLimits limits = defaultDeserializationLimits) {
	//#1: Deserialize size as serialSize_t
	serialSize_t size;
	deserialize(serial, size);

	//#1.1: Limit elements to save (if we need to)
	size_t saveSize = std::min((size_t)size, limits.max); //Only save up to `max` elements, discard the rest

	//#1.1: Clear container
	map.clear();

	//#2: Deserialize all pairs and store in map
	size_t i = 0;
	for (; i < saveSize; i++) {
		T key;
		U value;
		deserialize(serial, key, limits.childLimitOrSelf(0));
		deserialize(serial, value, limits.childLimitOrSelf(1));

		map[key] = value;
	}

	//#2.1: Pop any remaining elements to keep alignment
	for (; i < size; i++) {
		T tmpKey;
		U tmpValue;
		deserialize(serial, tmpKey, deserializationLimits(0));
		deserialize(serial, tmpValue, deserializationLimits(0));
	}
}
