#pragma once
#include <string>
#include <deque>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#define SERIAL_PORT_READ_BUF_SIZE 2048


class SerialPort
{
private:
	boost::mutex mutex_;
	std::deque<std::string> bufferedLines;
	char incomingBuffer[SERIAL_PORT_READ_BUF_SIZE];
	boost::asio::io_service io;
	std::unique_ptr<boost::asio::serial_port> port;
	std::string portName;
	bool isGood = false;
	std::string read_buf_str_;
	boost::thread ioThread;
	void async_read_some_();
	void on_receive_(const boost::system::error_code& ec, size_t bytes_transferred);
	int write_some(const char *buf, const int &size);
	std::function<void(const std::string&)> replyHandler;
	SerialPort(const SerialPort&) = delete;
	SerialPort* operator=(SerialPort &) = delete;
public:
	std::string friendlyName;
	SerialPort(const std::string &prettyName, unsigned int baudRate);
	~SerialPort();

	bool isConnected() const;
	void disconnect();
	virtual void on_receive_(const std::string &data);
	int write_some(const std::string &buf);
	void registerReplyHandler(std::function<void(const std::string&)> replier);
};

