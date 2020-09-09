#include "SerialPort.h"
#include <boost/thread.hpp>

SerialPort::SerialPort(const std::string &prettyName, unsigned int baudRate) : portName(prettyName)
{
	boost::system::error_code ec;

	port = std::make_unique<boost::asio::serial_port>(io);

	port->open(prettyName, ec);
	if (ec) {
		throw "Port cannot be opened";
	}

	// option settings...
	try{
	port->set_option(boost::asio::serial_port_base::baud_rate(baudRate));
	port->set_option(boost::asio::serial_port_base::character_size(8));
	port->set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
	port->set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
	port->set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

	ioThread = boost::thread(boost::bind(&boost::asio::io_service::run, &io));

	async_read_some_();

	}
	catch (...)
	{
	  std::cerr << "Failed to set serial port options\n";
	}
	isGood = true;
}

void SerialPort::disconnect()
{
	try {
		port->cancel();
		
	}
	catch (...)
	{

	}
	io.stop();
	ioThread.join();
	try {
		port->close();
	}
	catch (...)
	{
		// Cant disconnect port
		replyHandler("Cant disconnect");
	}
	replyHandler("Disconnected!");
	isGood = false;
}

void SerialPort::async_read_some_()
{
	if (port.get() == NULL || !port->is_open()) return;

	port->async_read_some(
		boost::asio::buffer(incomingBuffer, SERIAL_PORT_READ_BUF_SIZE),
		boost::bind(
			&SerialPort::on_receive_,
			this, boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void SerialPort::on_receive_(const boost::system::error_code& ec, size_t bytes_transferred)
{
	boost::mutex::scoped_lock look(mutex_);

	if (port.get() == NULL || !port->is_open()) return;
	if (ec) {
		async_read_some_();
		return;
	}

	for (unsigned int i = 0; i < bytes_transferred; ++i) {
		char c = incomingBuffer[i];
		if (c == '\n') {
			this->on_receive_(read_buf_str_);
			read_buf_str_.clear();
		}
		else {
			read_buf_str_ += c;
		}
	}

	async_read_some_();
}

void SerialPort::on_receive_(const std::string &data)
{
	//MessageHandler::getSingleton().addMessage(data);
	//bufferedLines.push_back(data);
	if (replyHandler)
	{
		replyHandler(data);
	}
	else
	{
		throw;
	}
}

void SerialPort::registerReplyHandler(std::function<void(const std::string&)> in)
{
	replyHandler = in;
}


SerialPort::~SerialPort()
{
	disconnect();
}

bool SerialPort::isConnected() const
{
	return isGood;
}

int SerialPort::write_some(const std::string &buf)
{
	char tempString[256];
	memset(tempString, 0, 256);
	sprintf(tempString, "%s\n\r", buf.c_str());
	auto length = strlen(tempString);
	return write_some(tempString, strlen(tempString));
}

int SerialPort::write_some(const char *buf, const int &size)
{
	boost::system::error_code ec;

	if (!port) return -1;
	if (size == 0) return 0;
	return port->write_some(boost::asio::buffer(buf, size), ec);
}