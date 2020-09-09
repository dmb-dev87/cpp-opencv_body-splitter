#pragma once
#include "EULADialog.h"
class EULAOverride :
	public EULA
{
public:
	EULAOverride(wxWindow* parent);
	~EULAOverride();

	void OnDisagree(wxCommandEvent& event);
	void OnAgree(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnCheck(wxCommandEvent& event);
};

