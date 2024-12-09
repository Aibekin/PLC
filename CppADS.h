#ifndef CPPADS_H
#define CPPADS_H

#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>

class CppADS
{
public:
	CppADS(const std::string &ip, uint16_t port);
	~CppADS();

	void connect();
	void disconnect();

	std::vector<uint8_t> read(uint32_t indexGroup, uint32_t indexOffset, uint32_t length);
	// void write(uint32_t indexGroup, uint32_t indexOffset, const std::vector<uint8_t> &data);

private:
	std::string m_ip;
	uint16_t m_port;
	int m_socket_fd;

	void sendRequest(const std::vector<uint8_t> &request);
	std::vector<uint8_t> receiveResponse();
};

#endif
