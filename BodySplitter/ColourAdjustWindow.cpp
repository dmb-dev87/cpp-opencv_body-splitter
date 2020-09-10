#include <wx/utils.h>

#include "ColourAdjustWindow.h"

#include "BodySplitterTexture.h"
#include "PrintBed.h"
#include "iSlicable.h"
#include "iDrawable.h"
#include "wxImagePanel.h"
#include "Settings.h"

ColourAdjustWindow::ColourAdjustWindow(wxWindow* parent) : ColourAdjustDialog(parent)
{
	updateBitmapImage();
	populateListBox();
	wxCommandEvent fakeEvent;
	ChangeMesh(fakeEvent);
}

void ColourAdjustWindow::populateListBox()
{
	std::unordered_map<std::string, int> lastValueCounter;
	auto drawables = PrintBed::GetSingleton().getDrawables();
	for (auto it : drawables)
	{
		if (it->hasTexture() == false)
			continue;
		auto drawable = std::dynamic_pointer_cast<iDrawable>(it);
		if (drawableMap.count(it->getName()) > 0)
		{
			lastValueCounter[it->getName()] += 1;
			drawableMap[std::string(it->getName() + " " + std::to_string(lastValueCounter[it->getName()]))] = drawable;
		}
		else
		{
			drawableMap[std::string(it->getName())] = drawable;
			lastValueCounter[it->getName()] = 0;
		}
	}
	for (auto it : drawableMap)
	{
		m_MeshChoice->Insert(it.first,0);
	}
	m_MeshChoice->SetSelection(0);
}

void ColourAdjustWindow::ChangeMesh(wxCommandEvent &event)
{
	workingTexture = drawableMap[m_MeshChoice->GetStringSelection().ToStdString()]->getBodySplitterTexture();
	std::string fileName = workingTexture->getFileName();
	m_currentWidth->SetLabel(std::to_string(workingTexture->getXSize()));
	m_currentHeight->SetLabel(std::to_string(workingTexture->getYSize()));
	m_PixWidth->SetValue(workingTexture->getXSize());
	m_PixHeight->SetValue(workingTexture->getYSize());
	sourceX = workingTexture->getSourceXSize();
	sourceY = workingTexture->getSourceYSize();
	m_ColourCount->SetValue(workingTexture->getColours().size());
	updateBitmapImage();
	if(workingTexture->getColours().size() > 99)
		m_currentColours->SetLabel(">99");
	else
		m_currentColours->SetLabel(std::to_string(workingTexture->getColours().size()));
}

ColourAdjustWindow::~ColourAdjustWindow()
{
}

void ColourAdjustWindow::UpdatePreviewImage(wxCommandEvent& event)
{
  if(!GET_BOOL("useKMeans") && workingTexture->getColours().size() != m_ColourCount->GetValue())
  {
    //Not using k-means, therefore colour changes are useless
    // Pop up dialog to enable the colour changes
    int answer = wxMessageBox("Colour simplification for the entire mesh is current off, adjusting the colour here will have no effect.\
     Would you like to enable colour simplification by entire mesh now?", "Colour Simplification", wxYES_NO);
    if (answer == wxYES)
    {
		// Should we save this to the ini file?
	    Settings::getSingleton().setBoolValue("useKMeans",true);
    }
  }
	wxBusyCursor wait;
	workingTexture->adjustColoursAndRes(m_ColourCount->GetValue(), m_PixWidth->GetValue(), m_PixHeight->GetValue());
	m_currentWidth->SetLabel(std::to_string(workingTexture->getTargetXSize()));
	m_currentHeight->SetLabel(std::to_string(workingTexture->getTargetYSize()));
	m_ColourCount->SetValue(workingTexture->getColours().size());
	updateBitmapImage();
	m_currentColours->SetLabel(std::to_string(workingTexture->getColours().size()));
}

void ColourAdjustWindow::updateBitmapImage()
{   
	if (workingTexture)
	{
		unsigned char *imgRawData;
		imgRawData = workingTexture->getRawImageData();
		wxImage tempWxImage(workingTexture->getXSize(), workingTexture->getYSize(), imgRawData, TRUE);
		tempWxImage = tempWxImage.Mirror(false); //Flip vertically cause opencv or was it opengl...
		//source image is GL_BGR_EXT, destination is rgb
		for (int x = 0; x < tempWxImage.GetWidth(); ++x)
		{
			for (int y = 0; y < tempWxImage.GetHeight(); ++y)
			{
				tempWxImage.SetRGB(x, y, tempWxImage.GetBlue(x, y),
					tempWxImage.GetGreen(x, y),
					tempWxImage.GetRed(x, y));
			}
		}
		m_PreviewImage->setImage(tempWxImage);
	}
}

void ColourAdjustWindow::OnPreserveRatio(wxCommandEvent& event)
{
	// not sure why i bothered with this event...
}

void ColourAdjustWindow::OnWidthChange(wxFocusEvent& event)
{
	if (m_HoldRatio->GetValue() == true)
	{
		double ratio = (double)m_PixWidth->GetValue()/ (double)sourceX;
		m_PixHeight->SetValue((double)sourceY*ratio);
	}
	event.Skip();
}

void ColourAdjustWindow::OnWidthChange(wxCommandEvent& event)
{
	if (m_HoldRatio->GetValue() == true)
	{
		double ratio = (double)m_PixWidth->GetValue() / (double)sourceX;
		m_PixHeight->SetValue((double)sourceY*ratio);
	}
}

void ColourAdjustWindow::OnHeightChange(wxFocusEvent& event)
{
	if (m_HoldRatio->GetValue() == true)
	{
		double ratio = (double)m_PixHeight->GetValue() / (double)sourceY;
		m_PixWidth->SetValue((double)sourceX*ratio);
	}
	event.Skip();
}

void ColourAdjustWindow::OnHeightChange(wxCommandEvent& event)
{
	if (m_HoldRatio->GetValue() == true)
	{
		double ratio = (double)m_PixHeight->GetValue() / (double)sourceY;
		m_PixWidth->SetValue((double)sourceX*ratio);
	}
}

void ColourAdjustWindow::OnRevertToSource(wxCommandEvent& event)
{
	workingTexture->removeSimplified();
	updateBitmapImage();
}

void ColourAdjustWindow::OnOKClick(wxCommandEvent& event)
{
	if (!GET_BOOL("useKMeans") && workingTexture->getColours().size() != m_ColourCount->GetValue())
	{
		//Not using k-means, therefore colour changes are useless
		// Pop up dialog to enable the colour changes
		int answer = wxMessageBox("Colour simplification for the entire mesh is current off, adjusting the colour here will have no effect.\
     Would you like to enable colour simplification by entire mesh now?", "Colour Simplification", wxYES_NO);
		if (answer == wxYES)
		{
			// Should we save this to the ini file?
			Settings::getSingleton().setBoolValue("useKMeans", true);
		}
	}
	wxBusyCursor wait;
	workingTexture->adjustColoursAndRes(m_ColourCount->GetValue(), m_PixWidth->GetValue(), m_PixHeight->GetValue());
	m_currentWidth->SetLabel(std::to_string(workingTexture->getTargetXSize()));
	m_currentHeight->SetLabel(std::to_string(workingTexture->getTargetYSize()));
	m_ColourCount->SetValue(workingTexture->getColours().size());
	m_currentColours->SetLabel(std::to_string(workingTexture->getColours().size()));
	event.Skip();
}

