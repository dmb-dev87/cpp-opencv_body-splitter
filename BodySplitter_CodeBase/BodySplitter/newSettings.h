///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __NEWSETTINGS_H__
#define __NEWSETTINGS_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/clrpicker.h>
#include <wx/choice.h>
#include <wx/listbook.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class settingsDialog
///////////////////////////////////////////////////////////////////////////////
class settingsDialog : public wxDialog
{
private:
	wxPanel* sliceSettings;
	wxStaticText* m_staticLayerThickness;
	wxStaticText* m_staticText33;
	wxStaticText* m_staticText3;
	wxStaticText* m_staticText34;
	wxStaticText* m_staticLayerThickness1;
	wxStaticText* m_staticText31;
	wxStaticText* m_staticText8;
	wxStaticText* m_staticText9;
	wxStaticText* m_staticText36;
	wxPanel* m_coasterSettings;
	wxStaticText* m_staticText21;
	wxStaticText* m_staticText221;
	wxStaticText* m_staticText23;
	wxStaticText* m_staticText24;
	wxStaticText* m_staticText25;
	wxStaticText* m_staticText15;
	wxStaticText* m_staticText29;
	wxStaticText* m_staticText17;
	wxStaticText* m_staticText30;
	wxStaticText* m_staticText19;
	wxStaticText* m_staticText311;
	wxStaticText* m_staticText20;
	wxStaticText* m_staticText32;
	wxStaticText* m_staticText291;
	wxStaticText* m_staticText301;
	wxStaticText* m_staticText321;
	wxStaticText* m_staticText331;
	wxStaticText* m_staticText341;
	wxStaticText* m_staticText35;
	wxStaticText* m_staticText59;
	wxStaticText* m_staticText60;
	wxStaticText* m_staticText64;
	wxStaticText* m_staticText66;
	wxStaticText* m_staticText68;
	wxStaticText* m_staticText69;
	wxStaticText* m_staticText73;
	wxStaticText* m_staticText591;
	wxStaticText* m_staticText601;
	wxStaticText* m_staticText641;
	wxStaticText* m_staticText661;
	wxStaticText* m_staticText681;
	wxStaticText* m_staticText691;
	wxStaticText* m_staticText732;
	wxStaticText* m_staticText592;
	wxStaticText* m_staticText602;
	wxStaticText* m_staticText642;
	wxStaticText* m_staticText662;
	wxStaticText* m_staticText682;
	wxStaticText* m_staticText692;
	wxStaticText* m_staticText733;

protected:
	wxNotebook* m_notebook1;
	wxPanel* m_layerSettings;
	wxTextCtrl* m_layerThickness;
	wxTextCtrl* m_firstLayerThickbess;
	wxStaticText* m_staticText1472;
	wxSpinCtrl* m_firstLayerExtMultiplier;
	wxStaticText* m_staticText1482;
	wxSpinCtrl* m_BottomSolidLayers;
	wxSpinCtrl* m_topSolidLayers;
	wxStaticText* m_advanced5;
	wxTextCtrl* m_infillOverlap;
	wxStaticText* m_advanced6;
	wxComboBox* m_infillPercentage;
	wxStaticText* m_staticText245;
	wxSpinCtrl* m_InfillExtrusionMultiplier;
	wxStaticText* m_staticText246;
	wxSpinCtrl* m_PerimeterNumber;
	wxTextCtrl* m_XYOffset;
	wxCheckBox* m_infillFirst;
	wxCheckBox* m_insideOut;
	wxCheckBox* m_NoSequenceIslands;
	wxPanel* m_colourSettings;
	wxCheckBox* m_fullModelSimplify;
	wxSpinCtrl* m_kMeansColours;
	wxSpinCtrl* m_coasterBase;
	wxColourPickerCtrl* m_coasterBaseColour;
	wxComboBox* m_baseOpacity;
	wxSpinCtrl* m_surfaceLayers;
	wxComboBox* m_surfaceOpacity;
	wxPanel* m_Support;
	wxCheckBox* m_checkSupport;
	wxChoice* m_SupportExt;
	wxChoice* m_InterfaceSupport;
	wxSpinCtrl* m_InterfaceCount;
	wxSpinCtrl* m_EmptySupportLayers;
	wxStaticText* m_staticText252;
	wxSpinCtrl* m_baseMaterialInterface;
	wxStaticText* m_staticText95;
	wxComboBox* m_SupportDensity;
	wxComboBox* m_InterfaceDensity;
	wxColourPickerCtrl* m_SupportColour;
	wxCheckBox* m_OverrideBaseInterface;
	wxChoice* m_BaseInterfaceTool;
	wxTextCtrl* m_maxSupportAngle;
	wxStaticText* m_staticText92;
	wxTextCtrl* m_SupportOffset;
	wxStaticText* m_staticText250;
	wxTextCtrl* m_supportHoleArea;
	wxStaticText* m_staticText251;
	wxCheckBox* m_PerciseCutSupport;
	wxPanel* m_printSettings;
	wxListbook* m_listbook2;
	wxPanel* m_panel15;
	wxTextCtrl* m_t0Diameter;
	wxTextCtrl* m_t1Diameter;
	wxTextCtrl* m_t2Diameter;
	wxTextCtrl* m_t0ExtWidth;
	wxTextCtrl* m_t1ExtWidth;
	wxTextCtrl* m_t2ExtWidth;
	wxTextCtrl* m_t0MinExtWidth;
	wxTextCtrl* m_t1MinExtWidth;
	wxTextCtrl* m_t2MinExtWidth;
	wxTextCtrl* m_t0FilamentD;
	wxTextCtrl* m_t1FilamentD;
	wxTextCtrl* m_t2FilamentD;
	wxTextCtrl* m_t0ExtMultiplier;
	wxTextCtrl* m_t1ExtMultiplier;
	wxTextCtrl* m_t2ExtMultiplier;
	wxCheckBox* m_purgeBinPrime;
	wxStaticText* m_staticText1241;
	wxTextCtrl* m_purgePrimeDistance;
	wxStaticText* m_staticText125;
	wxCheckBox* m_changeToolOverBin;
	wxCheckBox* m_purgeOnEachToolChange;
	wxStaticText* m_staticText218;
	wxTextCtrl* m_toolChangePurgeDistance;
	wxStaticText* m_staticText219;
	wxStaticText* m_staticText220;
	wxTextCtrl* m_toolChangePurgeFeedrate;
	wxStaticText* m_staticText2211;
	wxPanel* m_panel16;
	wxTextCtrl* m_T0_Temp;
	wxTextCtrl* m_T1_TEMP;
	wxTextCtrl* m_T2_TEMP;
	wxTextCtrl* m_Bed_Temp;
	wxCheckBox* m_heatAndWait;
	wxCheckBox* m_useCoolingFan;
	wxCheckBox* m_fanAlwaysOn;
	wxSpinCtrl* m_firstLayerFanSpeed;
	wxStaticText* m_staticText1131;
	wxSpinCtrl* m_minFanSpeed;
	wxStaticText* m_staticText1141;
	wxStaticText* m_staticText1111;
	wxSpinCtrl* m_fanSpeed;
	wxStaticText* m_staticText1121;
	wxCheckBox* m_reducePrintSpeed;
	wxStaticText* m_staticText1151;
	wxSpinCtrl* m_minLayerTime;
	wxStaticText* m_staticText116;
	wxStaticText* m_staticText117;
	wxSpinCtrl* m_minLayerTimeFan;
	wxStaticText* m_staticText118;
	wxStaticText* m_staticText119;
	wxSpinCtrl* m_minSlowDownFeedrate;
	wxStaticText* m_staticText120;
	wxPanel* m_panel17;
	wxCheckBox* m_AlwaysPurge;
	wxCheckBox* m_useSoftwarePurge;
	wxStaticText* m_staticText100;
	wxTextCtrl* m_purgeLength;
	wxStaticText* m_staticText101;
	wxStaticText* m_staticText112;
	wxTextCtrl* m_purgeFeedrate;
	wxStaticText* m_staticText113;
	wxStaticText* m_staticText102;
	wxSpinCtrl* m_purgeMixerSpeed;
	wxStaticText* m_staticText103;
	wxStaticText* m_staticText104;
	wxTextCtrl* m_purgeWait;
	wxStaticText* m_staticText105;
	wxStaticText* m_staticText106;
	wxTextCtrl* m_purgeLocationX;
	wxTextCtrl* m_purgeLocationY;
	wxStaticText* m_staticText107;
	wxTextCtrl* m_purgeWidth;
	wxTextCtrl* m_purgeHeight;
	wxStaticText* m_staticText108;
	wxTextCtrl* m_purgwWipeX;
	wxStaticText* m_staticText109;
	wxStaticText* m_staticText110;
	wxTextCtrl* m_purgeWipeWidth;
	wxStaticText* m_staticText111;
	wxStaticText* m_staticText168;
	wxTextCtrl* m_purgeWipeSpeed;
	wxStaticText* m_staticText169;
	wxCheckBox* m_purgeForceRetract;
	wxCheckBox* m_fullPurgeBinPause;
	wxStaticText* m_staticText1491;
	wxTextCtrl* m_purgeBinCapacity;
	wxStaticText* m_staticText150;
	wxPanel* m_panel18;
	wxCheckBox* m_LookAheadColourTrans;
	wxCheckBox* m_purgeExcessColourChange;
	wxStaticText* m_staticText1191;
	wxTextCtrl* m_DistanceToColourChange;
	wxStaticText* m_staticText1201;
	wxStaticText* m_staticText121;
	wxTextCtrl* m_ColourChangeMix;
	wxStaticText* m_staticText122;
	wxPanel* m_panel20;
	wxStaticText* m_staticText1291;
	wxChoice* m_firmwareChoice;
	wxPanel* m_panel201;
	wxStaticText* m_staticText131;
	wxTextCtrl* m_colourCost;
	wxStaticText* m_staticText132;
	wxStaticText* m_staticText135;
	wxTextCtrl* m_colourDiameter;
	wxStaticText* m_staticText136;
	wxStaticText* m_staticText139;
	wxTextCtrl* m_colourDensity;
	wxStaticText* m_staticText140;
	wxStaticText* m_staticText133;
	wxTextCtrl* m_supportCost;
	wxStaticText* m_staticText134;
	wxStaticText* m_staticText137;
	wxTextCtrl* m_supportDiameter;
	wxStaticText* m_staticText138;
	wxStaticText* m_staticText141;
	wxTextCtrl* m_supportDensity;
	wxStaticText* m_staticText142;
	wxStaticText* m_staticText144;
	wxTextCtrl* m_flexCost;
	wxStaticText* m_staticText145;
	wxStaticText* m_staticText146;
	wxTextCtrl* m_flexDiameter;
	wxStaticText* m_staticText147;
	wxStaticText* m_staticText148;
	wxTextCtrl* m_flexDensity;
	wxStaticText* m_staticText149;
	wxPanel* m_panel21;
	wxCheckBox* m_RovaCompat;
	wxSpinCtrl* m_ExtCount;
	wxStaticText* m_staticText1251;
	wxChoice* m_headChoice;
	wxColourPickerCtrl* m_CMYWhiteSub;
	wxStaticText* m_staticText1451;
	wxSpinCtrl* m_mixerSpeed;
	wxStaticText* m_staticText1461;
	wxCheckBox* m_stopMixerNotInUse;
	wxCheckBox* m_useServo;
	wxStaticText* m_staticText166;
	wxSpinCtrl* m_syringeServoIndex;
	wxStaticText* m_staticText1471;
	wxTextCtrl* m_ServoLift;
	wxStaticText* m_staticText1481;
	wxTextCtrl* m_ServoIssue;
	wxStaticText* m_staticText1441;
	wxSpinCtrl* m_syringeLiftTime;
	wxStaticText* m_staticText1452;
	wxStaticText* m_staticText1462;
	wxSpinCtrl* m_syringeIssueTime;
	wxPanel* m_panel22;
	wxStaticText* m_staticText152;
	wxTextCtrl* m_firstLayerPrintSpeed;
	wxStaticText* m_staticText153;
	wxStaticText* m_staticText154;
	wxTextCtrl* m_perimSpeed;
	wxStaticText* m_staticText155;
	wxStaticText* m_staticText156;
	wxTextCtrl* m_infillSpeed;
	wxStaticText* m_staticText157;
	wxStaticText* m_staticText158;
	wxTextCtrl* m_travelSpeed;
	wxStaticText* m_staticText159;
	wxStaticText* m_staticText1581;
	wxTextCtrl* m_firstLayerMixerSpeed;
	wxStaticText* m_staticText1591;
	wxStaticText* m_staticText160;
	wxTextCtrl* m_PerimeterMixerSpeed;
	wxStaticText* m_staticText161;
	wxStaticText* m_staticText164;
	wxTextCtrl* m_InfillMixerSpeed;
	wxStaticText* m_staticText165;
	wxStaticText* m_staticText162;
	wxTextCtrl* m_TravelMixerSpeed;
	wxStaticText* m_staticText163;
	wxPanel* m_panel25;
	wxCheckBox* m_EnableGCodeSplitUp;
	wxStaticText* m_staticText176;
	wxTextCtrl* m_GCodeFileTime;
	wxStaticText* m_staticText177;
	wxCheckBox* m_PuasePurgeBin;
	wxCheckBox* m_purgeResume;
	wxPanel* m_UISettings;
	wxCheckBox* m_simpleGCodeView;
	wxTextCtrl* m_gridWidth;
	wxTextCtrl* m_gridDepth;
	wxStaticText* m_staticText123;
	wxTextCtrl* m_gridHeight;
	wxStaticText* m_staticText124;
	wxTextCtrl* m_gridSpacing;
	wxCheckBox* m_showAxis;
	wxColourPickerCtrl* m_T1Colour;
	wxColourPickerCtrl* m_T2Colour;
	wxCheckBox* m_checkExperimental;
	wxPanel* m_AdvancedPanel;
	wxStaticText* m_advanced1;
	wxTextCtrl* m_minLineLength;
	wxStaticText* m_advanced2;
	wxStaticText* m_advanced3;
	wxTextCtrl* m_endZ;
	wxStaticText* m_advanced4;
	wxStaticText* m_advanced8;
	wxTextCtrl* m_topSurfaceInfill;
	wxStaticText* m_advanced9;
	wxStaticText* m_advanced10;
	wxTextCtrl* m_SurfaceInfillDensity;
	wxStaticText* m_advanced11;
	wxStaticText* m_advanced12;
	wxTextCtrl* m_BottomLayerInfill;
	wxStaticText* m_advanced13;
	wxStaticText* m_advanced14;
	wxTextCtrl* m_mmPerPixel;
	wxStaticText* m_advanced15;
	wxStaticText* m_advanced16;
	wxTextCtrl* m_ThreadCount;
	wxCheckBox* m_ColourPreviewByType;
	wxCheckBox* m_savePerimsToTest;
	wxCheckBox* m_ConsiderSurfaces;
	wxPanel* mRetraction;
	wxCheckBox* m_fwRetraction;
	wxCheckBox* m_retractLayerChange;
	wxCheckBox* m_retractInfillCrossPerimOnly;
	wxCheckBox* m_onyRetractCrossingPerimeters;
	wxNotebook* m_notebook3;
	wxPanel* m_perToolRetraction;
	wxTextCtrl* m_ZLift0;
	wxStaticText* m_staticText232;
	wxTextCtrl* m_ZLiftFeed0;
	wxStaticText* m_staticText233;
	wxTextCtrl* m_TravelWithoutRetract0;
	wxTextCtrl* m_CoastLength0;
	wxTextCtrl* m_RetractAmt0;
	wxTextCtrl* m_IssueAmt0;
	wxTextCtrl* m_RetractFeed0;
	wxTextCtrl* m_Issue0;
	wxTextCtrl* m_Wipe0;
	wxStaticText* m_staticText1561;
	wxTextCtrl* m_wipeFeedrate0;
	wxStaticText* m_staticText1571;
	wxTextCtrl* m_ToolChangeRetractT0;
	wxTextCtrl* m_toolChangeIssueT0;
	wxStaticText* m_staticText243;
	wxTextCtrl* m_PrintDistanceRetractT0;
	wxPanel* m_perToolRetraction1;
	wxTextCtrl* m_ZLift1;
	wxStaticText* m_staticText234;
	wxTextCtrl* m_ZLiftFeed1;
	wxStaticText* m_staticText235;
	wxTextCtrl* m_TravelWithoutRetract1;
	wxTextCtrl* m_CoastLength1;
	wxTextCtrl* m_RetractAmt1;
	wxTextCtrl* m_IssueAmt1;
	wxTextCtrl* m_RetractFeed1;
	wxTextCtrl* m_Issue1;
	wxTextCtrl* m_Wipe1;
	wxStaticText* m_staticText15611;
	wxTextCtrl* m_wipeFeedrate1;
	wxStaticText* m_staticText15711;
	wxStaticText* m_staticText239;
	wxTextCtrl* m_ToolChangeRetractT1;
	wxTextCtrl* m_toolChangeIssueT1;
	wxTextCtrl* m_PrintDistanceRetractT1;
	wxPanel* m_perToolRetraction2;
	wxTextCtrl* m_ZLift2;
	wxStaticText* m_staticText236;
	wxTextCtrl* m_ZLiftFeed2;
	wxStaticText* m_staticText237;
	wxTextCtrl* m_TravelWithoutRetract2;
	wxTextCtrl* m_CoastLength2;
	wxTextCtrl* m_RetractAmt2;
	wxTextCtrl* m_IssueAmt2;
	wxTextCtrl* m_RetractFeed2;
	wxTextCtrl* m_Issue2;
	wxTextCtrl* m_Wipe2;
	wxStaticText* m_staticText15612;
	wxTextCtrl* m_wipeFeedrate2;
	wxStaticText* m_staticText15712;
	wxStaticText* m_staticText240;
	wxTextCtrl* m_ToolChangeRetractT2;
	wxTextCtrl* m_toolChangeIssueT2;
	wxTextCtrl* m_PrintDistanceRetractT2;
	wxPanel* m_gcodeStrings;
	wxNotebook* m_notebook2;
	wxPanel* m_panel10;
	wxTextCtrl* m_OnStart;
	wxPanel* m_panel11;
	wxTextCtrl* m_OnEndGCode;
	wxPanel* m_panel12;
	wxTextCtrl* m_BeforeZMove;
	wxTextCtrl* m_OnAfterZMove;
	wxPanel* m_panel13;
	wxTextCtrl* m_OnRetractStrings;
	wxTextCtrl* m_OnIssue;
	wxPanel* m_panel23;
	wxTextCtrl* m_OnSuspendStrings;
	wxTextCtrl* m_OnResumeStrings;
	wxButton* m_saveToFile;
	wxButton* m_importSettings;
	wxButton* m_DefaultSetting;
	wxStdDialogButtonSizer* m_sdbSizer1;
	wxButton* m_sdbSizer1OK;
	wxButton* m_sdbSizer1Cancel;

	// Virtual event handlers, overide them in your derived class
	virtual void OnValueChanged(wxCommandEvent& event) { event.Skip(); }
	virtual void OnValueChanged(wxSpinEvent& event) { event.Skip(); }
	virtual void kMeansCheck(wxCommandEvent& event) { event.Skip(); }
	virtual void OnValueChanged(wxColourPickerEvent& event) { event.Skip(); }
	virtual void MainExtruderChoose(wxCommandEvent& event) { event.Skip(); }
	virtual void UpdateInterfaceCount(wxSpinEvent& event) { event.Skip(); }
	virtual void OverrideBaseInterfaceCheck(wxCommandEvent& event) { event.Skip(); }
	virtual void OnUsePurgeBinPrime(wxCommandEvent& event) { event.Skip(); }
	virtual void OnUseCoolingFanCheck(wxCommandEvent& event) { event.Skip(); }
	virtual void FanMaximumAlways(wxCommandEvent& event) { event.Skip(); }
	virtual void reducePrintSpeedOnSmallLayers(wxCommandEvent& event) { event.Skip(); }
	virtual void onPurgeSoftwareToggle(wxCommandEvent& event) { event.Skip(); }
	virtual void OnCheckPauseFullBin(wxCommandEvent& event) { event.Skip(); }
	virtual void OnLookAheadColourTransition(wxCommandEvent& event) { event.Skip(); }
	virtual void SwitchRovaCompat(wxCommandEvent& event) { event.Skip(); }
	virtual void OnHeadChoice(wxCommandEvent& event) { event.Skip(); }
	virtual void CMYWhiteSub(wxColourPickerEvent& event) { event.Skip(); }
	virtual void OnUseSyringeLift(wxCommandEvent& event) { event.Skip(); }
	virtual void ShowHideExperimental(wxCommandEvent& event) { event.Skip(); }
	virtual void FW_Retract(wxCommandEvent& event) { event.Skip(); }
	virtual void exportSettings(wxCommandEvent& event) { event.Skip(); }
	virtual void importSettings(wxCommandEvent& event) { event.Skip(); }
	virtual void LoadDefaultSettings(wxCommandEvent& event) { event.Skip(); }


public:
	wxListbook* m_listbook1;

	settingsDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("BodySplitter Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(728, 596), long style = wxDEFAULT_DIALOG_STYLE);
	~settingsDialog();

};

#endif //__NEWSETTINGS_H__
