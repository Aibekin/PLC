#ifndef CPPADS_H
#define CPPADS_H

#include <string>
#include <vector>

class CppADS
{
public:
	CppADS(const std::string &ip, uint16_t port); // constructor {ip: IP-address of PLC, port: TCP port (standart - 48898)}
	~CppADS();									  // destructor

	void connect();	   // connection to PLC
	void disconnect(); // disconnection from PLC

	std::vector<uint8_t> read(uint32_t indexGroup, uint32_t indexOffset, uint32_t length); // reading data from variable by indexGroup, indexOffset, length (all info about variable is in TwinCAT System Manager, -> variable data)

	// void write(uint32_t indexGroup, uint32_t indexOffset, const std::vector<uint8_t> &data); // writing data in variable

private:
	std::string m_ip; // IP-address of PLC
	uint16_t m_port;  // TCP Port
	int m_socket_fd;  // Socket

	void sendRequest(const std::vector<uint8_t> &request); // Req
	std::vector<uint8_t> receiveResponse();				   // Res
};

#endif
