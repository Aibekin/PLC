// class PLCReader
// {
// public:
// 	PLCReader(const std::string &ip, int port) : ip_(ip), port_(port), sockfd_(-1) {}

// 	void connectToPLC()
// 	{
// 		sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
// 		if (sockfd_ < 0)
// 			throw std::runtime_error("Failed to create socket");

// 		sockaddr_in serverAddr{};
// 		serverAddr.sin_family = AF_INET;
// 		serverAddr.sin_port = htons(port_);
// 		inet_pton(AF_INET, ip_.c_str(), &serverAddr.sin_addr);

// 		if (connect(sockfd_, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
// 			throw std::runtime_error("Connection to PLC failed");

// 		std::cout << "Connected to PLC" << std::endl;
// 	}

// 	std::vector<uint8_t> readVariable(uint32_t indexGroup, uint32_t indexOffset, uint32_t length)
// 	{
// 		std::vector<uint8_t> request = buildADSRequest(indexGroup, indexOffset, length);
// 		send(sockfd_, request.data(), request.size(), 0);

// 		uint8_t response[1024];
// 		int received = recv(sockfd_, response, sizeof(response), 0);
// 		if (received < 0)
// 			throw std::runtime_error("Failed to receive response");

// 		return std::vector<uint8_t>(response, response + received);
// 	}

// 	~PLCReader()
// 	{
// 		if (sockfd_ >= 0)
// 			close(sockfd_);
// 	}

// private:
// 	std::string ip_;
// 	int port_;
// 	int sockfd_;

// 	void sendData(const std::string &data)
// 	{
// 		if (sockfd_ < 0)
// 		{
// 			throw std::runtime_error("Not connected to PLC");
// 		}
// 		send(sockfd_, data.c_str(), data.size(), 0);
// 		std::cout << "Sent data: " << data << std::endl;
// 	}

// 	std::string receiveData()
// 	{
// 		if (sockfd_ < 0)
// 		{
// 			throw std::runtime_error("Not connected to PLC");
// 		}
// 		char buffer[1024] = {0};
// 		int bytesRead = recv(sockfd_, buffer, sizeof(buffer), 0);
// 		if (bytesRead < 0)
// 		{
// 			throw std::runtime_error("Failed to receive data");
// 		}
// 		return std::string(buffer, bytesRead);
// 	}

// 	std::vector<uint8_t> buildADSRequest(uint32_t indexGroup, uint32_t indexOffset, uint32_t length)
// 	{
// 		std::vector<uint8_t> request(38, 0); // 32 байта заголовка AMS + 6 байт данных ADS

// 		// --- AMS Header ---
// 		uint8_t targetNetId[6] = {5, 114, 111, 44, 1, 1};
// 		std::memcpy(&request[0], targetNetId, 6);

// 		uint16_t targetPort = htons(48898);
// 		std::memcpy(&request[6], &targetPort, 2);

// 		uint8_t sourceNetId[6] = {192, 168, 254, 165, 1, 1};
// 		std::memcpy(&request[8], sourceNetId, 6);

// 		uint16_t sourcePort = htons(33025);
// 		std::memcpy(&request[14], &sourcePort, 2);

// 		uint16_t commandId = htons(0x0002); // READ
// 		std::memcpy(&request[16], &commandId, 2);

// 		uint16_t stateFlags = htons(0x0004);
// 		std::memcpy(&request[18], &stateFlags, 2);

// 		uint32_t dataLength = htonl(12); // Длина ADS Data
// 		std::memcpy(&request[20], &dataLength, 4);

// 		uint32_t invokeId = htonl(1); // Уникальный ID
// 		std::memcpy(&request[28], &invokeId, 4);

// 		// --- ADS Data ---
// 		uint32_t group = htonl(indexGroup);
// 		std::memcpy(&request[32], &group, 4);

// 		uint32_t offset = htonl(indexOffset);
// 		std::memcpy(&request[36], &offset, 4);

// 		uint32_t len = htonl(length);
// 		std::memcpy(&request[40], &len, 4);

// 		return request;
// 	}
// };

// void processResponse(const std::vector<uint8_t> &response)
// {
// 	if (response.size() < 38)
// 	{
// 		throw std::runtime_error("Invalid response size");
// 	}

// 	uint32_t errorCode;
// 	std::memcpy(&errorCode, &response[22], 4);
// 	errorCode = ntohl(errorCode);

// 	if (errorCode != 0)
// 	{
// 		throw std::runtime_error("PLC returned error code: " + std::to_string(errorCode));
// 	}

// 	uint32_t dataLength;
// 	std::memcpy(&dataLength, &response[30], 4);
// 	dataLength = ntohl(dataLength);

// 	std::cout << "Data Length: " << dataLength << " bytes" << std::endl;

// 	if (response.size() < 38 + dataLength)
// 	{
// 		throw std::runtime_error("Incomplete data in response");
// 	}

// 	std::cout << "Data: ";
// 	for (size_t i = 38; i < 38 + dataLength; ++i)
// 	{
// 		std::cout << std::hex << static_cast<int>(response[i]) << " ";
// 	}
// 	std::cout << std::dec << std::endl;
// }

#include <iostream>
#include <cstring>
#include <vector>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "CppADS.h"

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

		std::cout << response.size();
	}
	catch (const std::exception &ex)
	{
		std::cerr << "Error: " << ex.what() << std::endl;
	}

	return 0;
}