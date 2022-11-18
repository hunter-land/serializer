#pragma once
#include "serialOut.hpp"
#include "serialIn.hpp"

#if __has_include(<optional>)
#include <optional>

template<typename T>
void serialize(serialOut& serial, const std::optional<T>& optional) {
	bool hasValue = optional.has_value();
	serialize(serial, hasValue);

	if (hasValue) {
		serialize(serial, optional.value());
	}
}

template<typename T>
void deserialize(serialIn& serial, std::optional<T>& optional, deserializationLimits limits = defaultDeserializationLimits) {
	bool hasValue;
	deserialize(serial, hasValue);

	if (!hasValue) {
		//No value
		optional = std::nullopt;
	} else {
		T value;
		deserialize(serial, value);
		optional = std::make_optional(value);
	}
}
#endif
