#pragma once
/* Popup window when printing a colourd object in multiextruder compat */
#include "CompatForm.h"

#include <vector>

class wxColourPickerCtrl;
class wxChoice;
class Colour;

class Rova5Compat :
	public MultiExtCompat
{
private:
	std::vector<wxColourPickerCtrl*> coloursOnPanel;
	std::vector<wxChoice*> nozzleChoices;
public:
	void populateSettingsToolMap();
	Rova5Compat(wxWindow* parent, const std::vector<Colour> &colours);
	~Rova5Compat();
};

