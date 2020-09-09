#include "EULAOverride.h"



EULAOverride::EULAOverride(wxWindow* parent) : EULA(parent)
{

}


EULAOverride::~EULAOverride()
{
}

void EULAOverride::OnDisagree(wxCommandEvent& event)
{
	this->EndModal(wxID_CLOSE);
}
void EULAOverride::OnAgree(wxCommandEvent& event)
{
	this->EndModal(wxID_YES);
}
void EULAOverride::OnClose(wxCloseEvent& event)
{
	this->EndModal(wxID_CLOSE);
}

void EULAOverride::OnCheck(wxCommandEvent& event)
{
	m_Agree->Enable(m_AgreeCheck->GetValue());
}