///////////////////////////////////////////////////////////////////////////
#include "colourSubstitionWX.h"
#include "Settings.h"
///////////////////////////////////////////////////////////////////////////

colourSubDialog::colourSubDialog(wxWindow* parent, const std::vector<Colour> &colours, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	this->colours = colours;
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);
	auto m_panel1 = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxALWAYS_SHOW_SB | wxVSCROLL);
	m_panel1->SetScrollRate(5, 5);
	//bSizer1->Add(m_panel1, 1, wxEXPAND | wxALL, 5);
	//m_panel1 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxVSCROLL);
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxVERTICAL);
	if (Settings::getSingleton().getStringValue("headChoice") == "RoVa4D - CMYK+WT")
		m_colourSubHolder = new wxFlexGridSizer(0, 13, 0, 0);
	else if(Settings::getSingleton().getStringValue("headChoice") == "Diamond Full Colour - CMYKW")
		m_colourSubHolder = new wxFlexGridSizer(0, 11, 0, 0);
	else
		m_colourSubHolder = new wxFlexGridSizer(0, 7, 0, 0);
	m_colourSubHolder->SetFlexibleDirection(wxBOTH);
	m_colourSubHolder->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	for (auto it : this->colours)
	{
		auto cmykw = it.getCMYKW();
		if (Settings::getSingleton().getStringValue("headChoice") == "Diamond Hotend - CMY")
		{
			auto cmy = it.getCMY();
			cmykw.c = cmy.c;
			cmykw.m = cmy.m;
			cmykw.y = cmy.y;
			cmykw.w = 0;
			cmykw.k = 0;
		}
		if (Settings::GetSingleton().hasColourSubs())
		{
			cmykw.c = Settings::GetSingleton().getColourSub(it).c;
			cmykw.m = Settings::GetSingleton().getColourSub(it).m;
			cmykw.y = Settings::GetSingleton().getColourSub(it).y;
			cmykw.k = Settings::GetSingleton().getColourSub(it).k;
			cmykw.w = Settings::GetSingleton().getColourSub(it).w;
		}
		m_colour.push_back(new wxColourPickerCtrl(m_panel1, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE));
		m_colour.back()->SetMaxSize(wxSize(50, -1));
		m_colour.back()->SetColour(wxColor(it.getR<uint8_t>(), it.getG<uint8_t>(), it.getB<uint8_t>()));
		m_colourSubHolder->Add(m_colour.back(), 0, wxALL, 5);

		m_staticText1.push_back(new wxStaticText(m_panel1, wxID_ANY, wxT("C:"), wxDefaultPosition, wxDefaultSize, 0));
		m_staticText1.back()->Wrap(-1);
		//m_staticText1.back()->SetMinSize(wxSize(10, -1));
		//m_staticText1.back()->SetMaxSize(wxSize(10, -1));
		m_colourSubHolder->Add(m_staticText1.back(), 0, wxALL, 5);

		m_C.push_back(new wxSpinCtrl(m_panel1, wxID_ANY, wxT("100"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0));
		m_C.back()->SetValue(cmykw.c);
		m_C.back()->SetMaxSize(wxSize(45, -1));

		m_colourSubHolder->Add(m_C.back(), 0, wxALL, 5);

		m_staticText2.push_back(new wxStaticText(m_panel1, wxID_ANY, wxT("M:"), wxDefaultPosition, wxDefaultSize, 0));
		m_staticText2.back()->Wrap(-1);
		//m_staticText2.back()->SetMinSize(wxSize(10, -1));
		//m_staticText2.back()->SetMaxSize(wxSize(10, -1));
		m_colourSubHolder->Add(m_staticText2.back(), 0, wxALL, 5);

		m_M.push_back(new wxSpinCtrl(m_panel1, wxID_ANY, wxT("100"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0));
		m_M.back()->SetValue(cmykw.m);
		m_M.back()->SetMaxSize(wxSize(45, -1));

		m_colourSubHolder->Add(m_M.back(), 0, wxALL, 5);

		m_staticText3.push_back(new wxStaticText(m_panel1, wxID_ANY, wxT("Y:"), wxDefaultPosition, wxDefaultSize, 0));
		m_staticText3.back()->Wrap(-1);
		//m_staticText3.back()->SetMinSize(wxSize(10, -1));
		//m_staticText3.back()->SetMaxSize(wxSize(10, -1));
		m_colourSubHolder->Add(m_staticText3.back(), 0, wxALL, 5);

		m_Y.push_back(new wxSpinCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0));
		m_Y.back()->SetValue(cmykw.y);
		m_Y.back()->SetMaxSize(wxSize(45, -1));

		m_colourSubHolder->Add(m_Y.back(), 0, wxALL, 5);

		if(Settings::getSingleton().getStringValue("headChoice") == "RoVa4D - CMYK+WT" || Settings::getSingleton().getStringValue("headChoice") == "Diamond Full Colour - CMYKW")
		{
			m_staticText4.push_back(new wxStaticText(m_panel1, wxID_ANY, wxT("K:"), wxDefaultPosition, wxDefaultSize, 0));
			m_staticText4.back()->Wrap(-1);
			//m_staticText4.back()->SetMinSize(wxSize(10, -1));
			//m_staticText4.back()->SetMaxSize(wxSize(10, -1));
			m_colourSubHolder->Add(m_staticText4.back(), 0, wxALL, 5);

			m_K.push_back(new wxSpinCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0));
			m_K.back()->SetValue(cmykw.k);
			m_K.back()->SetMaxSize(wxSize(45, -1));

			m_colourSubHolder->Add(m_K.back(), 0, wxALL, 5);

			m_staticText5.push_back(new wxStaticText(m_panel1, wxID_ANY, wxT("W:"), wxDefaultPosition, wxDefaultSize, 0));
			m_staticText5.back()->Wrap(-1);
			//m_staticText5.back()->SetMinSize(wxSize(10, -1));
			//m_staticText5.back()->SetMaxSize(wxSize(10, -1));
			m_colourSubHolder->Add(m_staticText5.back(), 0, wxALL, 5);

			m_W.push_back(new wxSpinCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0));
			m_W.back()->SetValue(cmykw.w);
			m_W.back()->SetMaxSize(wxSize(45, -1));

			m_colourSubHolder->Add(m_W.back(), 0, wxALL, 5);
			if (Settings::getSingleton().getStringValue("headChoice") == "RoVa4D - CMYK+WT") {
				m_staticText6.push_back(new wxStaticText(m_panel1, wxID_ANY, wxT("T:"), wxDefaultPosition, wxDefaultSize, 0));
				m_staticText6.back()->Wrap(-1);
				//m_staticText6.back()->SetMinSize(wxSize(10, -1));
				//m_staticText6.back()->SetMaxSize(wxSize(10, -1));
				m_colourSubHolder->Add(m_staticText6.back(), 0, wxALL, 5);

				m_T.push_back(new wxSpinCtrl(m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0));
				m_T.back()->SetMaxSize(wxSize(45, -1));
				if (Settings::getSingleton().hasColourSubs())
					m_T.back()->SetValue(Settings::GetSingleton().getColourSub(it).t);
				m_colourSubHolder->Add(m_T.back(), 0, wxALL, 5);
			}
		}
	}

	bSizer2->Add(m_colourSubHolder, 1, wxEXPAND, 5);


	m_panel1->SetSizer(bSizer2);
	m_panel1->Layout();
	bSizer2->Fit(m_panel1);
	bSizer1->Add(m_panel1, 1, wxEXPAND | wxALL, 5);



	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer(wxVERTICAL);

	m_RestoreColours = new wxButton(this, wxID_ANY, wxT("Restore Colours"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer3->Add(m_RestoreColours, 0, wxALIGN_RIGHT | wxALL, 5);


	bSizer1->Add(bSizer3, 0, wxALIGN_BOTTOM | wxEXPAND, 5);

	m_sdbSizer2 = new wxStdDialogButtonSizer();
	m_sdbSizer2OK = new wxButton(this, wxID_OK);
	m_sdbSizer2->AddButton(m_sdbSizer2OK);
	m_sdbSizer2Cancel = new wxButton(this, wxID_CANCEL);
	m_sdbSizer2->AddButton(m_sdbSizer2Cancel);
	m_sdbSizer2->Realize();

	bSizer1->Add(m_sdbSizer2, 0, wxEXPAND, 5);


	this->SetSizer(bSizer1);
	this->Layout();

	this->Centre(wxBOTH);

	m_RestoreColours->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(colourSubDialog::OnRestoreColours), NULL, this);
}

colourSubDialog::~colourSubDialog()
{
	m_RestoreColours->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(colourSubDialog::OnRestoreColours), NULL, this);
}

void colourSubDialog::OnRestoreColours(wxCommandEvent& event)
{
	Settings::getSingleton().resetColourSub();
	EndModal(wxID_CANCEL);
}

void colourSubDialog::saveColourSub()
{
	Settings::GetSingleton().resetColourSub();
	for (std::size_t i = 0; i < m_C.size(); i++)
	{
		Colour::ColourSpace::CMYKWT col;
		col.c = m_C[i]->GetValue();
		col.m = m_M[i]->GetValue();
		col.y = m_Y[i]->GetValue();
		if (Settings::getSingleton().getStringValue("headChoice") == "RoVa4D - CMYK+WT" ||
			Settings::getSingleton().getStringValue("headChoice") == "Diamond Full Colour - CMYKW")
		{
			col.k = m_K[i]->GetValue();
			col.w = m_W[i]->GetValue();
			if (Settings::getSingleton().getStringValue("headChoice") == "RoVa4D - CMYK+WT")
				col.t = m_T[i]->GetValue();
		}
		Colour newColour(m_colour[i]->GetColour().Red(), m_colour[i]->GetColour().Green(), m_colour[i]->GetColour().Blue());
		Settings::GetSingleton().addColourSub(newColour, col);
	}
}