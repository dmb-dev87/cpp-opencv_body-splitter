#include<boost/tokenizer.hpp>

#include "machineControl.h"
#include "SerialCom.h"
#include "SerialPort.h"
#include "GCodeSender.h"
#include "Settings.h"
#include "GCodeViewPane.h"
#include <wx/wx.h>

#ifndef _CONSOLE

const int MACHINE_GCODE_UPDATE = 9000;
const int MACHINE_GCODE_SEND = 9001;
const int MACHINE_TEMPERATURE_COMMAND = 9002;
const int MACHINE_PROGRESS_UPDATE = 9003;
const int MACHHINE_START_PRINT = 9004;

machineControl::machineControl(wxWindow* parent) : controlPanel(parent)
{
	// Update serial ports
	SerialCom *cerealCom = new SerialCom();
	auto portName = cerealCom->getPortNames();

	m_SerialPort->Clear();
	for(auto it : portName)
		m_SerialPort->Append(it);
	m_SerialPort->SetSelection(m_SerialPort->GetCount() - 1);
	gcodeSender = std::make_unique<BodySplitter::GCodeSender>(std::bind(&machineControl::addSerialMessage, this, std::placeholders::_1));
	gcodeSender->addLayerCompleteCallback(std::bind(&GCodeViewPane::setPrintingLayer, GCodeViewPane::GetSingletonPtr(), std::placeholders::_1));
	textBoxCommandHistory.set_capacity(50);
	textBoxCommandHistory.push_back(std::string());
	selectedCommand = textBoxCommandHistory.rbegin();
}

bool machineControl::connectSerial(const std::string &str)
{
	std::string baudRateStr = m_baudRate->GetStringSelection();
	unsigned int baudRate = std::stoul(m_baudRate->GetStringSelection().ToStdString());
	try {
		connectedPort = SerialCom::getSingleton().connectPort(str,baudRate);
		
		connectedPort->registerReplyHandler(std::bind(&machineControl::queueUIMessage,this,std::placeholders::_1));
	}
	catch (...)
	{
		queueUIMessage("Couldn't connect to port: " + str);
		return false;
	}
	return true;
}

void machineControl::disconnectSerial()
{
	connectedPort->disconnect();
	SerialCom::GetSingleton().disconnect(connectedPort->friendlyName);
	connectedPort.reset();
}

machineControl::~machineControl()
{
}

void machineControl::SetPrintSpeed(wxCommandEvent& event)
{
	gcodeSender->forceCommand("M220 S" + std::to_string(m_PrintSpeed->GetValue()));
}

void machineControl::ExtMultiplier(wxCommandEvent& event)
{
	gcodeSender->forceCommand("M221 S" + std::to_string(m_ExtSpeed->GetValue()));
}

void machineControl::keepAliveThread()
{
	// wait for the connection to settle
	int lastProgresss = 0;
	std::this_thread::sleep_for(std::chrono::seconds((int)GET_DOUBLE("keepAliveTime")));
	while (1)
	{
		connectLock.lock();
		if (isConnected == false)
		{
			connectLock.unlock();
			break;
		}
		connectLock.unlock();
		// Send temperature update every 30? seconds
		gcodeSender->forceCommand("M105");
		std::this_thread::sleep_for(std::chrono::seconds((int)GET_DOUBLE("keepAliveTime")));
		double progress = gcodeSender->getProgress();
		if (progress * 100 != lastProgresss)
		{
			wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, MACHINE_PROGRESS_UPDATE);
			event.SetInt(progress * 100);
			lastProgresss = progress * 100;
			this->GetEventHandler()->AddPendingEvent(event);
		}
	}
}

void machineControl::ConnectToggle(wxCommandEvent &ev)
{
	std::lock_guard<std::mutex> lock(connectLock);
	if (!isConnected)
	{
		if (connectSerial(m_SerialPort->GetValue().ToStdString()))
		{
			gcodeSender->connect();
			m_Connect->SetLabel("Disconnect");
			isConnected = true;
			std::thread keepAlive(&machineControl::keepAliveThread, this);
			keepAlive.detach();
		}
		else
		{

		}
	}
	else
	{
		gcodeSender->disconnect();
		disconnectSerial();
		isConnected = false;
		m_Connect->SetLabel("Connect");
	}
}

void machineControl::SetMixerTemp(wxCommandEvent &event)
{
	// Mixer is T0
	if (isConnected)
	{
		gcodeSender->forceCommand("M104 S" + m_T0_Temp->GetValue().ToStdString());
	}
}

void machineControl::SetSupportTemp(wxCommandEvent &event)
{
	// Mixer is T0
	if (isConnected)
	{
		gcodeSender->forceCommand("M104 T1 S" + m_T1_Temp->GetValue().ToStdString());
	}
}

void machineControl::SetFlexibleTemp(wxCommandEvent &event)
{
	// Mixer is T0
	if (isConnected)
	{
		gcodeSender->forceCommand("M104 T2 S" + m_T2_Temp->GetValue().ToStdString());
	}
}

void machineControl::SetBedTemp(wxCommandEvent &event)
{
	// Mixer is T0
	if (isConnected)
	{
		gcodeSender->forceCommand("M140 S" + m_Bed_Temp->GetValue().ToStdString());
	}
}

void machineControl::OffMixerTemp(wxCommandEvent &event)
{
	// Mixer is T0
	if (isConnected)
	{
		gcodeSender->forceCommand("M104 T0 S0");
	}
}
void machineControl::OffSupportTemp(wxCommandEvent &event)
{
	// Mixer is T0
	if (isConnected)
	{
		gcodeSender->forceCommand("M104 T1 S0");
	}
}
void machineControl::OffFlexibleTemp(wxCommandEvent &event)
{
	// Mixer is T0
	if (isConnected)
	{
		gcodeSender->forceCommand("M104 T2 S0");
	}
}

void machineControl::SetOffTemp(wxCommandEvent& event)
{
	if (isConnected)
	{
		gcodeSender->forceCommand("M140 S0");
	}
}

void machineControl::sendCommand(wxCommandEvent &ev)
{
	if (isConnected)
	{
		std::string toSend = m_StringToSend->GetValue().ToStdString();
		textBoxCommandHistory.back() = toSend;
		textBoxCommandHistory.push_back(std::string());
		selectedCommand = textBoxCommandHistory.rbegin();
		std::transform(toSend.begin(), toSend.end(), toSend.begin(), std::ptr_fun<int, int>(std::toupper));
		gcodeSender->forceCommand(toSend);
		if(verbose)
			queueUIMessage(toSend);
		else if(toSend.find('*') == std::string::npos)
			queueUIMessage(toSend);
	}
	m_StringToSend->Clear();
}

void machineControl::DownHistory(wxKeyEvent& event)
{
	
	if (event.GetKeyCode() == WXK_UP)
	{
		if (selectedCommand == textBoxCommandHistory.rend())
			selectedCommand = textBoxCommandHistory.rbegin();

			m_StringToSend->SetValue(*selectedCommand);
			selectedCommand++;
			m_StringToSend->SetSelection(0, m_StringToSend->GetLineLength(0));
		
	}
	else if (event.GetKeyCode() == WXK_DOWN)
	{
		
		if (selectedCommand == textBoxCommandHistory.rend())
			selectedCommand = textBoxCommandHistory.rbegin();

			m_StringToSend->SetValue(*selectedCommand);
			if (selectedCommand == textBoxCommandHistory.rbegin())
				selectedCommand = textBoxCommandHistory.rend();
			selectedCommand--;
			m_StringToSend->SetSelection(0, m_StringToSend->GetLineLength(0));

	}
	else
		event.Skip();
}

void machineControl::sendCommandEnter(wxCommandEvent& event)
{
	sendCommand(event);
}

void machineControl::progressUpdate(wxCommandEvent& event)
{
	m_Progress->SetValue(event.GetInt());
}

void machineControl::addSerialMessage(const std::string& message)
{
	if (message.empty())
		return;
	wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, MACHINE_GCODE_SEND);
	event.SetString(message);
	this->GetEventHandler()->AddPendingEvent(event);
}

void machineControl::queueUIMessage(const std::string &message)
{
	static bool ignoreNextOk = false;
	if (message.empty())
		return;
	if (message.find("Resend: ") != std::string::npos)
	{
		gcodeSender->resendLine(std::stoi(message.substr(8)));
		ignoreNextOk = true;
	}
	if (message.find("ok T:") != std::string::npos)
	{
		wxCommandEvent tempEvent(wxEVT_COMMAND_TEXT_UPDATED, MACHINE_TEMPERATURE_COMMAND);
		tempEvent.SetString(message);
		this->GetEventHandler()->AddPendingEvent(tempEvent);
	}
	if (message.substr(0, 2) == "ok")
	{
		if (ignoreNextOk)
		{
			ignoreNextOk = false;
		}
		else
		{
			gcodeSender->getOK();
			if (!verbose)
				return;
		}
	}
	wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, MACHINE_GCODE_UPDATE);
	event.SetString(message);
	this->GetEventHandler()->AddPendingEvent(event);
}

void machineControl::sendToSerialPort(wxCommandEvent& event)
{
	if (event.GetString().empty())
		return;
	connectedPort->write_some(event.GetString().ToStdString());
	if (verbose)
	{
		wxCommandEvent outEvent(wxEVT_COMMAND_TEXT_UPDATED, MACHINE_GCODE_UPDATE);
		outEvent.SetString("->" + event.GetString());
		this->GetEventHandler()->AddPendingEvent(outEvent);
	}
}

void machineControl::OnUpdateGCodeConsole(wxCommandEvent &ev)
{
	m_GCodeConsole->AppendText("\n"+ev.GetString());
}

void machineControl::StartPrint(wxCommandEvent& event)
{
	gcodeSender->startLineNumberChecksum();
	gcodeSender->queueCommands(fileToPrint);
	gcodeSender->startPrint();
	m_PausePrint->Enable();
	m_Abort->Enable();
	m_StartPrint->Enable(false);
}

void machineControl::PuaseToggle(wxCommandEvent& event)
{
	if (m_PausePrint->GetLabelText() == "Resume")
	{
		gcodeSender->startPrint();
		m_PausePrint->SetLabel("Pause");
	}
	else
	{
		gcodeSender->pausePrint();
		m_PausePrint->SetLabel("Resume");
	}
}

void machineControl::VerboseToggle(wxCommandEvent& event)
{
	verbose = m_Verbose->GetValue();
}

bool machineControl::autoConnect()
{
	if (isConnected)
	{
		wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, MACHHINE_START_PRINT);
		this->GetEventHandler()->AddPendingEvent(event);
		return true;
	}
	for (auto it : SerialCom::getSingleton().getPortNames())
	{
		if (connectSerial(it))
		{
			gcodeSender->connect();
			isConnected = true;
			std::thread keepAlive(&machineControl::keepAliveThread, this);
			keepAlive.detach();

			wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, MACHHINE_START_PRINT);
			
			this->GetEventHandler()->AddPendingEvent(event);

			return true;
		}
	}
	return false;
}

void machineControl::Abort(wxCommandEvent& event)
{
	gcodeSender->clearQueued();
	m_StartPrint->Enable();
	m_Abort->Enable(false);
	m_PausePrint->SetLabel("Pause");
	m_PausePrint->Enable(false);
}

void machineControl::getTemperatureCommand(wxCommandEvent &ev)
{
	//ok T:18.7 /0.0 B:19.8 /0.0 T0:18.7 /0.0 @:0.00% B@:0.00%
	std::string messageString = ev.GetString().ToStdString();
	double t0 = std::stod(messageString.substr(5));
	double t0Set = std::stod(messageString.substr(messageString.find('/')+1));
	double bedTemp = std::stod(messageString.substr(messageString.find("B:")+2));
	double bedSet = std::stod(messageString.substr(messageString.find('/', messageString.find("B:")+2)+1));
	
	std::stringstream t0Output;
	t0Output.setf(std::ios::fixed, std::ios::floatfield);
	t0Output << std::setprecision(1) << "Mixer Tool: "<<t0<<"/"<<t0Set;
	std::stringstream bedOut;
	bedOut.setf(std::ios::fixed, std::ios::floatfield);
	bedOut << std::setprecision(1) << "Bed Temp: " << bedTemp << "/" << bedSet;
	m_BedTempReadout->SetLabel(bedOut.str());
	m_MixerTempReadout->SetLabel(t0Output.str());
}

void machineControl::Plus10Y(wxCommandEvent& event)
{
	if (isConnected)
	{
		gcodeSender->forceCommand("G91"); //switch to relative positioning
		gcodeSender->forceCommand("G1 Y10 F2000");
		gcodeSender->forceCommand("G92");
	}
}

void machineControl::Minus10X(wxCommandEvent& event)
{
	if (isConnected)
	{
		gcodeSender->forceCommand("G91"); //switch to relative positioning
		gcodeSender->forceCommand("G1 X-10 F2000");
		gcodeSender->forceCommand("G92");
	}
}

void machineControl::Home(wxCommandEvent& event)
{
	if (isConnected)
	{
		gcodeSender->forceCommand("G28");
	}
}

void machineControl::Plus10X(wxCommandEvent& event)
{
	if (isConnected)
	{
		gcodeSender->forceCommand("G91"); //switch to relative positioning
		gcodeSender->forceCommand("G1 X10 F2000");
		gcodeSender->forceCommand("G92");
	}
}

void machineControl::Minus10Y(wxCommandEvent& event)
{
	if (isConnected)
	{
		gcodeSender->forceCommand("G91"); //switch to relative positioning
		gcodeSender->forceCommand("G1 Y-10 F2000");
		gcodeSender->forceCommand("G92");
	}
}


#include <fstream>

void machineControl::LoadGCodeFile(wxCommandEvent& event)
{
	
	wxFileDialog
		openFileDialog(this, _("Open GCode file"), "", "",
			"gcode file (*.gcode)|*.gcode", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
	{
		return;
	}
	
	std::ifstream gcodeloader(openFileDialog.GetPath().ToStdString());
	std::string currentLine;
	
	while (!gcodeloader.eof())
	{
		std::getline(gcodeloader, currentLine);
		// Skip blank lines, either only a comment or empty
		if (currentLine.size() == 0)
		{

		}
		else if (currentLine[0] == ';' || currentLine[0] == '\n' || currentLine[0] == '\0')
		{

		}
		else
		{
			fileToPrint.push_back(currentLine.substr(0, currentLine.find(';')));
		}
	}
	//gcodeSender->queueCommands(fileToPrint);
	m_StartPrint->Enable(true);
}

void machineControl::RefreshPorts(wxCommandEvent& event)
{
	auto portName = SerialCom::GetSingleton().getPortNames();

	m_SerialPort->Clear();
	for (auto it : portName)
		m_SerialPort->Append(it);
	m_SerialPort->SetSelection(m_SerialPort->GetCount() - 1);
}

void machineControl::queueLines(const std::string &line)
{
	boost::char_separator<char> sep("\n");
	boost::tokenizer<boost::char_separator<char> > tokens(line, sep);

	for (auto it : tokens)
	{
		if (!it.empty() && it.at(0) != ';')
			gcodeSender->queueCommands(it);
	}
}

void machineControl::queueLines(std::stringstream &ss)
{
	std::string line;
	std::vector<std::string> vec;
	while (std::getline(ss, line))
	{
		if(!line.empty() && line.at(0) != ';')
			vec.push_back(line); 
	}
	gcodeSender->queueCommands(vec);

}

void machineControl::OnScrollPrintSpeed(wxScrollEvent& event)
{
	m_printSpeedSpinCtrl->SetValue(m_PrintSpeed->GetValue());
}
void machineControl::OnScrollExtMultiplier(wxScrollEvent& event)
{
	m_extSpeedSpinCtrl->SetValue(m_ExtSpeed->GetValue());
}

void machineControl::OnSetExtrusionSpinner(wxFocusEvent& event)
{
	m_ExtSpeed->SetValue(m_extSpeedSpinCtrl->GetValue()); 
	event.Skip();
}
void machineControl::OnSetPrintSpeedSpinner(wxFocusEvent& event)
{
	m_PrintSpeed->SetValue(m_printSpeedSpinCtrl->GetValue());
	event.Skip();
}

void machineControl::OnPrintSpeedSpinCtrl(wxSpinEvent& event)
{
	m_PrintSpeed->SetValue(m_printSpeedSpinCtrl->GetValue());
}

void machineControl::OnExtrusionSpinCtrl(wxSpinEvent& event)
{
	m_ExtSpeed->SetValue(m_extSpeedSpinCtrl->GetValue());
}

wxBEGIN_EVENT_TABLE(machineControl, wxPanel)
EVT_COMMAND(MACHINE_GCODE_UPDATE, wxEVT_COMMAND_TEXT_UPDATED, machineControl::OnUpdateGCodeConsole)
EVT_COMMAND(MACHINE_GCODE_SEND, wxEVT_COMMAND_TEXT_UPDATED, machineControl::sendToSerialPort)
EVT_COMMAND(MACHINE_TEMPERATURE_COMMAND, wxEVT_COMMAND_TEXT_UPDATED, machineControl::getTemperatureCommand)
EVT_COMMAND(MACHINE_PROGRESS_UPDATE, wxEVT_COMMAND_TEXT_UPDATED, machineControl::progressUpdate)
EVT_COMMAND(MACHHINE_START_PRINT, wxEVT_COMMAND_TEXT_UPDATED, machineControl::StartPrint)
wxEND_EVENT_TABLE()

#endif