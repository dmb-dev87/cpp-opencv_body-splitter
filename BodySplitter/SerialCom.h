#pragma once
/* Serial com to printer, doesnt support rova3d baudrates in linux... */
#include <boost/asio.hpp>
#include <vector>
#include <string>
#include <memory>
#include <unordered_set>
#include "SerialPort.h"
#include "Singleton.h"

class SerialCom : public Singleton<SerialCom>
{
private:
	std::unordered_set<std::string> connectedPorts;
	SerialCom(const SerialCom &) = delete;
	SerialCom* operator=(SerialCom &) = delete;
public:
	std::vector<std::string> getPortNames() const;
	std::unique_ptr<SerialPort> connectPort(const std::string &name, unsigned int baudRate = 250000);
	SerialCom();
	~SerialCom();
	void disconnect(const std::string &name);
	
	
};

