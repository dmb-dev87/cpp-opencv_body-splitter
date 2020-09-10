#include "Rova5Compat.h"
#include "colour.h"
#include "Settings.h"

#include <wx/choice.h>
#include <wx/clrpicker.h>

Rova5Compat::Rova5Compat(wxWindow* parent, const std::vector<Colour> &colours) :  MultiExtCompat(parent)
{
	wxString *choiceChoices;// = { wxT("T0"), wxT("T1") };
	choiceChoices = new wxString[(int)GET_DOUBLE("compatExtruderCount")];
	for (int i = 0; i < (int)GET_DOUBLE("compatExtruderCount"); i++)
	{
		choiceChoices[i] = "T" + std::to_string(i);
	}
	int i = 0;
	for (auto it : colours)
	{
		// Object is deleted when window is closed
		wxColourPickerCtrl* tempColour = new wxColourPickerCtrl(m_scrolledWindow1, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE);
		tempColour->SetColour(wxColour(it.getR<uint8_t>(), it.getG<uint8_t>(), it.getB<uint8_t>()));
		addNozzlesHere->Add(tempColour, 0, wxALL, 5);
		coloursOnPanel.push_back(tempColour);
		
		int m_choice2NChoices = (int)GET_DOUBLE("compatExtruderCount");
		wxChoice* tempChoice = new wxChoice(m_scrolledWindow1, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice2NChoices, choiceChoices, 0);
		tempChoice->SetSelection(i% (int)GET_DOUBLE("compatExtruderCount"));
		addNozzlesHere->Add(tempChoice, 0, wxALL, 5);
		nozzleChoices.push_back(tempChoice);
		i++;
	}
	delete[] choiceChoices;
}

void Rova5Compat::populateSettingsToolMap()
{
	assert(coloursOnPanel.size() == nozzleChoices.size());
	Settings::getSingleton().resetColourToolMap();
	for (int i = 0; i < nozzleChoices.size(); i++)
	{
		Settings::getSingleton().addColourToolPair(Colour(coloursOnPanel[i]->GetColour().Red(),
			coloursOnPanel[i]->GetColour().Green(),
			coloursOnPanel[i]->GetColour().Blue()),nozzleChoices[i]->GetSelection());
	}
}

Rova5Compat::~Rova5Compat()
{
}
