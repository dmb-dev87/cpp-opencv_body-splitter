#include "AboutDialogOverride.h"
#include "images\rsimages.h"


AboutDialogOverride::AboutDialogOverride(wxWindow* parent) : AboutDialog(parent)
{
	//delete m_bitmap1;
	//m_bitmap1 = new wxStaticBitmap(this, wxID_ANY, wxBITMAP_PNG_FROM_DATA(bin2c_rightchevron), wxDefaultPosition, wxDefaultSize, 0);
	//bSizer2->Layout();
}


AboutDialogOverride::~AboutDialogOverride()
{
}

void AboutDialogOverride::OnClose(wxCommandEvent &event)
{
	EndModal(wxID_CLOSE);
}