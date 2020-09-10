#pragma once
#include "AboutDialog.h"
class AboutDialogOverride :
	public AboutDialog
{
public:
	void OnClose(wxCommandEvent& event);
	AboutDialogOverride(wxWindow* parent);
	~AboutDialogOverride();
};

