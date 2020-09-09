#include "SerialCom.h"
//Needs to be written for each OS :S
std::vector<std::string> SelectComPort();

SerialCom::SerialCom()
{
}

std::unique_ptr<SerialPort> SerialCom::connectPort(const std::string &name, unsigned int baudRate)
{
	if (connectedPorts.count(name) > 0)
		throw "Port Already Connected";
	
	std::unique_ptr<SerialPort> newPort = std::make_unique<SerialPort>(name,baudRate);
	if (newPort->isConnected())
	{
		connectedPorts.insert(name);
	}
	else
		throw;
	newPort->friendlyName = name;
	return newPort;
}

void SerialCom::disconnect(const std::string &name)
{
	connectedPorts.erase(name);
}

SerialCom::~SerialCom()
{
}

std::vector<std::string> SerialCom::getPortNames() const
{
	return SelectComPort();
}
#ifdef _WIN32
#include <Windows.h>
//#include <atlstr.h>
std::vector<std::string> SelectComPort() //added function to find the present serial 
{
	std::vector<std::string> output;
	char lpTargetPath[1000]; // buffer to store the path of the COMPORTS
	DWORD test;
	bool gotPort = 0; // in case the port is not found

	for (int i = 0; i<255; i++) // checking ports from COM0 to COM255
	{
		char comNameSimple[8];
		sprintf(comNameSimple, "COM%d", i);
		//CString str;
		//str.Format(_T("%d"), i);
		//CString ComName = CString("COM") + CString(str); // converting to COM0, COM1, COM2
		test = QueryDosDeviceA(comNameSimple, lpTargetPath, 5000);

		// Test the return value and error if any
		if (test != 0) //QueryDosDevice returns zero if it didn't find an object
		{
			output.push_back(std::string(comNameSimple));
			gotPort = 1; // found port
		}

		if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			lpTargetPath[10000]; // in case the buffer got filled, increase size of the buffer.
			continue;
		}

	}
	return output;

//	if (!gotPort) // if not port
		//m_MyPort.AddString((CString)"No Active Ports Found"); // to display error message incase no ports found

}
#else
std::vector<std::string> SelectComPort() //added function to find the present serial 
{
  std::string port;
  int fd;
  std::vector<std::string> output;
  for(int i = 0; i < 256; ++i)
  {
      port.clear();
      port.append("/dev/ttyUSB");
      port.append(std::to_string(i));
      fd = open(port.c_str(), O_RDWR | O_NOCTTY);
      if(fd != -1)
      {
	  output.push_back(port);
      }
  }
  return output;
}
#endif