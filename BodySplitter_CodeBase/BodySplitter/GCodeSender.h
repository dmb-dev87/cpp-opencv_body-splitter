#pragma once
#include <deque>
#include <thread>
#include <string>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <boost/circular_buffer.hpp>

/* Mostly works, need to write some logic for timeout, ie message send and no ok recivd */

namespace BodySplitter {
	using SentCommand = std::pair<int, std::string>;
	class GCodeSender
	{
	private:
		// Line number to avoid com errors
		int lineNumber = -1;
		unsigned int estimatedPrintLayer = 0;
		unsigned int checkSum(const std::string &line) const;
		// Commands to send in order, usually with checksums: IE printing gcode
		std::deque<std::string> commandsToSend;
		std::size_t totalFileLines = 0;
		// Commands to send asap, without checksums: IE jog controls, m105 etc
		std::deque<std::string> immediateCommands;
		// Store only ordered commands with checksum
		boost::circular_buffer <SentCommand> sentCommands;
		bool readyToSend = true;
		bool isPrinting = false;
		bool useChecksums = false;
		bool isConnected = false;

		mutable std::mutex waitingForOk;
		mutable std::mutex commandBuffer;
		mutable std::mutex sentLineLock;
		mutable std::mutex connectLock;

		std::thread m_runThread;

		std::function<void(const std::string &)> sendFunction;
		std::vector<std::function<void(double layerZ)>> newLayerFunctions;

		GCodeSender(const GCodeSender &) = delete;
		GCodeSender* operator=(GCodeSender &) = delete;
	public:
		GCodeSender(std::function<void(const std::string &)> sender);
		~GCodeSender();
		// Not exception safe
		void runningThread() noexcept;
		// Add single command to back of queue
		void queueCommands(const std::string &command);
		// Add multiple commands to the back of queue
		void queueCommands(const std::vector<std::string> &commands);
		// Attemp to add command to front of queue
		void forceCommand(const std::string &command);
		// Clear the queued commands - Example new print loaded
		void clearQueued();

		void getOK();
		void resendLine(int failedNum);

		double getProgress() const;
		unsigned int getLayerPrinting() const {
			return estimatedPrintLayer;
		}

		void startLineNumberChecksum();
		void endLineNumberChecksum();
		void pausePrint();
		void startPrint();

		void connect();
		void disconnect();

		void addLayerCompleteCallback(std::function<void(double)> layerFunction);
	};

}// namepsace BodySplitter