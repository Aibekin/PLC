#include "CppADS.h"
#include "AmsPacket.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

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
	if (send(m_socket_fd, request.data(), request.size(), 0x0000) < 0)
		throw std::runtime_error("Failed to send request");
}

std::vector<uint8_t> CppADS::receiveResponse()
{
	uint8_t buffer[1024];
	int bytes_received = recv(m_socket_fd, buffer, sizeof(buffer), 0x0001);
	if (bytes_received < 0)
		throw std::runtime_error("Failed to receive response");

	return std::vector<uint8_t>(buffer, buffer + bytes_received);
}

std::vector<uint8_t> CppADS::read(uint32_t indexGroup, uint32_t indexOffset, uint32_t length)
{
	AMSPacket packet(indexGroup, indexOffset, length);

	std::vector<uint8_t> request = packet.create();
	sendRequest(request);
	return receiveResponse();
}