#include <iostream>
#include <vector>
#include <stdexcept>
#include <netinet/in.h>
#include "CppADS.h"
#include "AMS/ReadResponse.h"

// IP-address: 169.254.39.115
// Port: 48898

int main()
{
	try
	{
		CppADS plc("169.254.39.115", 48898);
		plc.connect();

		uint32_t indexGroup = 0xF021;
		uint32_t indexOffset = 0x180;
		uint32_t length = 0x1;

		std::vector<uint8_t> response = plc.read(indexGroup, indexOffset, length);

		ReadResponse res = ReadResponse::fromBytes(response);

		std::cout << response.size() << std::endl;
		std::cout << res.result << std::endl;
		std::cout << res.length << std::endl;
		for (auto i : res.data)
			std::cout << i << std::endl;
	}
	catch (const std::exception &ex)
	{
		std::cerr << "Error: " << ex.what() << std::endl;
	}

	return 0;
}