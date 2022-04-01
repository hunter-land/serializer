#include "include/serializer.hpp"
#include <iostream>

int main() {
	std::string filename = "./test.serial";
	std::vector<uint8_t> serialLoopback;

	serialOut::flushToVector(serialLoopback)({ 0, 2, 4 });

	serialDuplex duplex(serialIn::fillFromVector(serialLoopback), serialOut::flushToVector(serialLoopback));
	for (size_t i = 0; i < 5; i++) {
		uint16_t someData = i * 0xB0b;
		duplex << someData;
		uint16_t someDataBack;
		duplex >> someDataBack;

		std::cout << "Serialied " << someData << std::endl;
		std::cout << "Got " << someDataBack << std::endl;
	}

	uint64_t data64 = 0xA00AFF00;
	double dataPi = 3.1415926535897;

	{
		auto flushFunc = serialOut::flushToFile(filename);
		serialOut serialToFile(flushFunc);

		serialToFile << data64 << dataPi;
	}

	uint64_t dataBack64 = 0;
	double dataBackPi = 0;
	{
		auto fillFunc = serialIn::fillFromFile(filename);
		serialIn serialFromFile(fillFunc);

		serialFromFile >> dataBack64;
		std::cout << "Original data: " << data64 << std::endl;
		std::cout << "Read back data: " << dataBack64 << std::endl;
		std::cout << "Match: " << (data64 == dataBack64) << std::endl;

		serialFromFile >> dataBackPi;
		std::cout << "Original data: " << dataPi << std::endl;
		std::cout << "Read back data: " << dataBackPi << std::endl;
		std::cout << "Match: " << (dataPi == dataBackPi) << std::endl;

		std::cout << "Attempting to read another byte" << std::endl;
		uint8_t nonExistantByte = 5;
		serialFromFile >> nonExistantByte;
		std::cout << "nonExistantByte is " << (int)nonExistantByte << std::endl;
	}
}
