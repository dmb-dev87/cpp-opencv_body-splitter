#pragma once
/* Handles the print control tab, not extensively tested.. */

#include <memory>
#include <thread>
#include <mutex>
#include "controlPanel.h"
#include <boost/circular_buffer.hpp>
#include <fstream>

namespace BodySplitter {
	class GCodeSender;
}
class SerialPort;

class machineControl : public controlPanel
{
private:
	std::unique_ptr<SerialPort> connectedPort;
	std::unique_ptr<BodySplitter::GCodeSender> gcodeSender;
	std::mutex connectLock;

	std::vector<std::string > fileToPrint;
	boost::circular_buffer<std::string> textBoxCommandHistory;
	// Needs to be reset after each command added
	boost::circular_buffer<std::string>::reverse_iterator selectedCommand;
	bool isConnected = false;
	bool connectSerial(const std::string &portName);
	bool verbose = false;

	

	void disconnectSerial();

	void OnUpdateGCodeConsole(wxCommandEvent& event);

	void sendToSerialPort(wxCommandEvent& event);
	void getTemperatureCommand(wxCommandEvent &event);
	void progressUpdate(wxCommandEvent &event);

	void keepAliveThread();
	
protected:
	void ConnectToggle(wxCommandEvent& event);
	void sendCommand(wxCommandEvent& event);
	void sendCommandEnter(wxCommandEvent& event);
	void SetMixerTemp(wxCommandEvent& event);
	void OffMixerTemp(wxCommandEvent& event);
	void SetSupportTemp(wxCommandEvent& event);
	void OffSupportTemp(wxCommandEvent& event);
	void SetFlexibleTemp(wxCommandEvent& event);
	void OffFlexibleTemp(wxCommandEvent& event);
	void SetBedTemp(wxCommandEvent& event);
	void SetOffTemp(wxCommandEvent& event);
	void StartPrint(wxCommandEvent& event);
	void LoadGCodeFile(wxCommandEvent& event);
	void PuaseToggle(wxCommandEvent& event);
	void SetPrintSpeed(wxCommandEvent& event);
	void ExtMultiplier(wxCommandEvent& event);
	void VerboseToggle(wxCommandEvent& event);
	void Abort(wxCommandEvent& event);
	void RefreshPorts(wxCommandEvent& event);
	void DownHistory(wxKeyEvent& event);

	void OnScrollPrintSpeed(wxScrollEvent& event);
	void OnScrollExtMultiplier(wxScrollEvent& event);
	void OnSetExtrusionSpinner(wxFocusEvent& event);
	void OnSetPrintSpeedSpinner(wxFocusEvent& event);

	void OnPrintSpeedSpinCtrl(wxSpinEvent& event);
	void OnExtrusionSpinCtrl(wxSpinEvent& event);

	void Plus10Y(wxCommandEvent& event);
	void Minus10X(wxCommandEvent& event);
	void Home(wxCommandEvent& event);
	void Plus10X(wxCommandEvent& event);
	void Minus10Y(wxCommandEvent& event);
public:
	void queueUIMessage(const std::string &message);
	void addSerialMessage(const std::string &message);
	machineControl(wxWindow* parent);
	~machineControl();

	bool autoConnect();
	void queueLines(std::stringstream &stream);
	void queueLines(const std::string &line);

	wxDECLARE_EVENT_TABLE();
};

