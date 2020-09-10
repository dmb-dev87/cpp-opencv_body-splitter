#include "GCodeSender.h"

namespace BodySplitter {

	GCodeSender::GCodeSender(std::function<void(const std::string &)> sender) : sendFunction(sender)
	{

	}


	GCodeSender::~GCodeSender()
	{

	}

	void GCodeSender::runningThread() noexcept
	{
		std::string lastCommand; // Use incase the ok times out
		std::chrono::time_point<std::chrono::system_clock> sentTime;
		while (1)
		{
			waitingForOk.lock();
			if (readyToSend)
			{
				waitingForOk.unlock();
				commandBuffer.lock();
				{
					if (commandsToSend.empty() && immediateCommands.empty())
					{
						commandBuffer.unlock();
						// Wait? wait for commandToSend not empty?
					}
					else
					{
						if (!immediateCommands.empty()) // Process immediate commands first
						{
							sendFunction(immediateCommands.front());
							lastCommand = immediateCommands.front();
							immediateCommands.pop_front();

							commandBuffer.unlock();
							waitingForOk.lock();
							readyToSend = false;
							sentTime = std::chrono::system_clock::now();
							waitingForOk.unlock();
						}
						else if (useChecksums && isPrinting && !commandsToSend.empty())
						{
							std::string message = "N" + std::to_string(lineNumber) + " " + commandsToSend.front().substr(0, commandsToSend.front().find(';'));
							sendFunction(message + "*" + std::to_string(checkSum(message)));
							lastCommand = message + "*" + std::to_string(checkSum(message));
							sentLineLock.lock();
							sentCommands.push_back(SentCommand(lineNumber, commandsToSend.front()));
							sentTime = std::chrono::system_clock::now();
							// Check if its a layer
							if (!newLayerFunctions.empty())
							{
								if (commandsToSend.front().find("G1 ") != std::string::npos && commandsToSend.front().find("Z") != std::string::npos)
								{
									// This is assumed to be a layer change
									double newHeight = std::stod(commandsToSend.front().substr(commandsToSend.front().find("Z") + 1));
									for (auto &it : newLayerFunctions)
										it(newHeight);
								}
							}
							sentLineLock.unlock();
							commandsToSend.pop_front();
							lineNumber++;

							commandBuffer.unlock();
							waitingForOk.lock();
							readyToSend = false;
							waitingForOk.unlock();
						}
						else if (isPrinting && !commandsToSend.empty())
						{
							sendFunction(commandsToSend.front());	// needs checksum and line numbers still
							lastCommand = commandsToSend.front();
							commandsToSend.pop_front();

							sentTime = std::chrono::system_clock::now();
							commandBuffer.unlock();
							waitingForOk.lock();
							readyToSend = false;
							waitingForOk.unlock();
						}
						else
							commandBuffer.unlock();
						
					}
				}
			}
			else
			{
				// Test timeout
				std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - sentTime;
				if (elapsed_seconds.count() > 35.0)
				{
					sentTime = std::chrono::system_clock::now();
					//sendFunction(lastCommand);
					readyToSend = true;
				}
				try {
					waitingForOk.unlock();
				}
				catch (...)
				{

				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(20)); //umm hmm
			{
				try {
					std::lock_guard<std::mutex> lock(connectLock);

					//connectLock.lock();
					if (!isConnected)
					{
						//connectLock.unlock();
						return;
					}
				}
				catch (...)
				{

				}
				//connectLock.unlock();
			}
		}	//end of while(1)
	}

	unsigned int GCodeSender::checkSum(const std::string &command) const
	{
		unsigned int messageCheckSum = 0;
		for (std::size_t i = 0; i < command.size(); i++)
		{
			messageCheckSum ^= command[i];
		}
		return messageCheckSum;
	}

	void GCodeSender::queueCommands(const std::string &command)
	{
		std::lock_guard<std::mutex> lock(commandBuffer);
		commandsToSend.push_back(command);
		totalFileLines++;
	}

	void GCodeSender::queueCommands(const std::vector<std::string> &commands)
	{
		std::lock_guard<std::mutex> lock(commandBuffer);
		commandsToSend.insert(commandsToSend.end(), commands.begin(), commands.end());
		totalFileLines += commandsToSend.size();
	}

	void GCodeSender::forceCommand(const std::string &command)
	{
		std::lock_guard<std::mutex> lock(commandBuffer);
		immediateCommands.push_front(command);
	}

	void GCodeSender::clearQueued()
	{
		std::lock_guard<std::mutex> lock(commandBuffer);
		commandsToSend.clear();
		totalFileLines = 0;
	}

	void GCodeSender::resendLine(int failedNum)
	{
		//Find the line
		std::lock_guard<std::mutex> lock(sentLineLock);
		//if (failedNum == sentCommands.back().first() + 1)
		//{

		//}
		for (std::size_t i = 0; i < sentCommands.size(); i++)
		{
			if (sentCommands.at(i).first == failedNum)
			{
				std::string message = "N"+std::to_string(failedNum) + " " + sentCommands.at(i).second;
				sendFunction(message+"*"+std::to_string(checkSum(message)));
			}
		}
	}

	void GCodeSender::getOK()
	{
		waitingForOk.lock();
		readyToSend = true;
		waitingForOk.unlock();
	}

	void GCodeSender::startLineNumberChecksum()
	{
		std::lock_guard<std::mutex> lock(sentLineLock);
		sentCommands.clear();
		sentCommands.set_capacity(100);
		useChecksums = true;
		lineNumber = 0;
		std::string resetMessage = "M110";
		commandBuffer.lock();
		commandsToSend.push_front(resetMessage);
		commandBuffer.unlock();
	}

	void GCodeSender::endLineNumberChecksum()
	{
		useChecksums = false;
	}

	void GCodeSender::pausePrint()
	{
		// Lock the queued commands so we can flip the pause flag
		std::lock_guard<std::mutex> lock(commandBuffer);
		isPrinting = false;
	}

	void GCodeSender::startPrint()
	{
		// Lock the queued commands so we can flip the pause flag
		std::lock_guard<std::mutex> lock(commandBuffer);
		isPrinting = true;
	}

	void GCodeSender::connect()
	{
		std::lock_guard<std::mutex> lock(connectLock);
		isConnected = true;
		m_runThread = std::thread(&GCodeSender::runningThread, this);
		m_runThread.detach();
	}

	void GCodeSender::disconnect()
	{
		connectLock.lock();
		isConnected = false;
		connectLock.unlock();
		if(m_runThread.joinable())
			m_runThread.join();
	}
	double GCodeSender::getProgress() const
	{
		if (totalFileLines == 0)
			return 1.0;
		else
			return ((double)totalFileLines -(double)commandsToSend.size()) / (double)totalFileLines;
	}

	void GCodeSender::addLayerCompleteCallback(std::function<void(double)> newLayer)
	{
		newLayerFunctions.push_back(newLayer);
	}

} // namepsace RovaSlcie