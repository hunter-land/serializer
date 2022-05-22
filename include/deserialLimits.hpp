#pragma once
#include <limits>
#include <cstdint>
#include <vector>

//This structure is used to mitigate attacks from deserialization sources
//The primary approach is to limit container sizes (vectors can have a max of X, strings can't be longer than Y, etc)
struct deserializationLimits {
	deserializationLimits(size_t max = std::numeric_limits<std::size_t>::max()) {
		this->max = max;
	};

	//Limits for this container/object
	std::size_t max; //How many elements should be allowed

	//Limits for the elements of this container/object
	std::vector<deserializationLimits> childLimits; //If this limit is for something with children (eg: vector of strings), define limits of the children (eg strings) here

	deserializationLimits childLimitOrSelf(size_t i) {
		if (childLimits.size() > i) {
			return childLimits[i];
		} else {
			return *this;
		}
	}
};

const static deserializationLimits defaultDeserializationLimits;
