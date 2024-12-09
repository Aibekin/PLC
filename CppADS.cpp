#include "CppADS.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

class ADSPacket
{
private:
	uint32_t m_indexGroup;
	uint32_t m_indexOffset;
	uint32_t m_length;

public:
	ADSPacket(uint32_t indexGroup, uint32_t indexOffset, uint32_t length) : m_indexGroup(indexGroup), m_indexOffset(indexOffset), m_length(length) {}
	std::vector<uint8_t> create()
	{
		std::vector<uint8_t> request(38);
		uint32_t packet_length = 32 + 12;
		request[2] = packet_length & 0xFF;
		request[3] = (packet_length >> 8) & 0xFF;
		request[4] = (packet_length >> 16) & 0xFF;
		request[5] = (packet_length >> 24) & 0xFF;

		uint8_t targetNetId[6] = {5, 114, 111, 44, 1, 1};
		std::memcpy(&request[0], targetNetId, 6);

		uint16_t targetPort = htons(801);
		std::memcpy(&request[6], &targetPort, 2);

		uint8_t sourceNetId[6] = {192, 168, 254, 165, 1, 1};
		std::memcpy(&request[8], sourceNetId, 6);

		uint16_t sourcePort = htons(33025);
		std::memcpy(&request[14], &sourcePort, 2);

		uint16_t commandId = htons(0x0002);
		std::memcpy(&request[16], &commandId, 2);

		uint16_t stateFlags = htons(0x0004);
		std::memcpy(&request[18], &stateFlags, 2);

		uint32_t dataLength = htonl(12);
		std::memcpy(&request[20], &dataLength, 4);

		uint32_t invokeId = htonl(1);
		std::memcpy(&request[28], &invokeId, 4);

		uint32_t group = htonl(m_indexGroup);
		std::memcpy(&request[32], &group, 4);

		uint32_t offset = htonl(m_indexOffset);
		std::memcpy(&request[36], &offset, 4);

		uint32_t len = htonl(m_length);
		std::memcpy(&request[37], &len, 4);
		return request;
	}
};

CppADS::CppADS(const std::string &ip, uint16_t port)
	: m_ip(ip), m_port(port), m_socket_fd(-1) {}

CppADS::~CppADS()
{
	if (m_socket_fd >= 0)
		disconnect();
}

void CppADS::connect()
{
	m_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket_fd < 0)
		throw std::runtime_error("Failed to create socket");

	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(m_port);

	if (inet_pton(AF_INET, m_ip.c_str(), &serverAddr.sin_addr) <= 0)
		throw std::runtime_error("Invalid IP address");

	if (::connect(m_socket_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
		throw std::runtime_error("Connection failed to PLC");
}

void CppADS::disconnect()
{
	close(m_socket_fd);
}

void CppADS::sendRequest(const std::vector<uint8_t> &request)
{
	if (send(m_socket_fd, request.data(), request.size(), 0) < 0)
		throw std::runtime_error("Failed to send request");
}

std::vector<uint8_t> CppADS::receiveResponse()
{
	uint8_t buffer[1024];
	int bytes_received = recv(m_socket_fd, buffer, sizeof(buffer), 0);
	if (bytes_received < 0)
		throw std::runtime_error("Failed to receive response");

	return std::vector<uint8_t>(buffer, buffer + bytes_received);
}

std::vector<uint8_t> CppADS::read(uint32_t indexGroup, uint32_t indexOffset, uint32_t length)
{
	ADSPacket packet(indexGroup, indexOffset, length);

	std::vector<uint8_t> request = packet.create();
	sendRequest(request);
	return receiveResponse();
}