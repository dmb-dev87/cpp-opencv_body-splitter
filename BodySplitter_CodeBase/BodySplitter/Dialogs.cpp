#include "Dialogs.h"

ScaleDialog::ScaleDialog() : wxDialog(nullptr, -1, "Scale Mesh")
{
	wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	wxStaticText *xScale = new wxStaticText(this, -1, "X:",
		wxPoint(0, 0));
	textControlX = new wxTextCtrl(this, -1, "1", wxPoint(15, 0),wxSize(60,40));
	sizer->Add(xScale);
	sizer->Add(textControlX);
	
	wxStaticText *yScale = new wxStaticText(this, -1, "Y:",
		wxPoint(0, 40));
	textControlY = new wxTextCtrl(this, -1, "1", wxPoint(15, 40), wxSize(60, 40));
	sizer->Add(yScale);
	sizer->Add(textControlY);

	wxStaticText *zScale = new wxStaticText(this, -1, "Z:",
		wxPoint(0, 80));
	textControlZ = new wxTextCtrl(this, -1, "1", wxPoint(15, 80), wxSize(60, 40));
	sizer->Add(zScale);
	sizer->Add(textControlZ);


	wxButton * b = new wxButton(this, wxID_OK, _("OK"),wxPoint(120,0), wxDefaultSize);
	wxButton * c = new wxButton(this, wxID_CANCEL, _("Cancel"), wxPoint(120,110), wxDefaultSize);
	sizer->Add(b);
	sizer->Add(c);
}

ScaleDialog::~ScaleDialog()
{

}

double ScaleDialog::getXScale()const
{
	double out;
	textControlX->GetLineText(0).ToCDouble(&out);
	return out;
}
double ScaleDialog::getYScale()const
{
	double out;
	textControlY->GetLineText(0).ToCDouble(&out);
	return out;
}
double ScaleDialog::getZScale()const
{
	double out;
	textControlZ->GetLineText(0).ToCDouble(&out);
	return out;
}

/*
class RotationDialog : public wxDialog
{
private:
	wxTextCtrl* rotationAmount;

public:
	RotationDialog(int axis);
	~RotationDialog();
	double getRotationAmount()const;
};
*/

using namespace BodySplitter;
RotationDialog::RotationDialog(AXIS axis) : wxDialog(nullptr, -1, "Rotate Mesh")
{
	if (axis == AXIS::X_AXIS)
		this->SetTitle("Rotate Mesh X");
	if (axis ==	AXIS::Y_AXIS)
		this->SetTitle("Rotate Mesh Y");
	else
		this->SetTitle("Rotate Mesh Z");
	wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	wxStaticText *xScale = new wxStaticText(this, -1, "Rotation:",
		wxPoint(0, 0));
	rotationAmount = new wxTextCtrl(this, -1, "1", wxPoint(15, 0), wxSize(60, 40));
	sizer->Add(xScale);
	sizer->Add(rotationAmount);
	wxButton * b = new wxButton(this, wxID_OK, _("OK"), wxPoint(120, 0), wxDefaultSize);
	wxButton * c = new wxButton(this, wxID_CANCEL, _("Cancel"), wxPoint(120, 110), wxDefaultSize);
	sizer->Add(b);
	sizer->Add(c);
}
//output to rads for internal use...
double RotationDialog::getRotationAmount() const
{
	double out;
	rotationAmount->GetLineText(0).ToCDouble(&out);
	out = (out / 360.0)*(2.0*3.141592654);
	return out;
}

RotationDialog::~RotationDialog()
{

}

MeshControl::MeshControl(bool isTextured,bool hasTexture,const Colour &col) : slicableMenu(nullptr)
{
	if (!hasTexture)
		m_useTexture->Enable(false);
	{
		m_useTexture->SetValue(isTextured);
		m_objectColour->Enable(!isTextured);
	}
	if (col)
	{
		m_objectColour->SetColour(wxColor(col.getR<uint8_t>(),
			col.getG<uint8_t>(),
			col.getB<uint8_t>()));
	}
}

void MeshControl::UseTextureCheck(wxCommandEvent& event)
{
	m_objectColour->Enable(!m_useTexture->GetValue());
}

MeshControl::~MeshControl()
{

}

Colour MeshControl::getColour()
{
	return Colour(m_objectColour->GetColour().Red(),
		m_objectColour->GetColour().Green(),
		m_objectColour->GetColour().Blue());
}

bool MeshControl::getUsingTexture()
{
	return m_useTexture->GetValue();
}