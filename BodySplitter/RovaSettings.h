#pragma once
/* Main settings dialog box */
#include "newSettings.h"
class SettingValues;

#include "colour.h"
#include <mutex>

class RovaSettings : public settingsDialog
{
private:
	SettingValues *sliceValues;
	bool changesNeedingSaving = false;
	void boxesToSettingValues(SettingValues *sliceValuesOut);

	static SettingValues *defaultSettings;
	static void fillDefaultSettings();
	void toggleExperimental(bool isShow);
	static std::mutex defaultSettingWriter;

	void fillCheck( wxCheckBox  *box, const std::string &key, const SettingValues *sliceValuesIn) const;
	void fillNumber( wxTextCtrl  *box, const std::string &key, const SettingValues *sliceValuesIn) const;
	void fillNumber(wxTextCtrl* box, const std::string &key, const SettingValues* sliceValuesIn, int decimals) const;
	void fillNumber( wxSpinCtrl  *box, const std::string &key, const SettingValues *sliceValuesIn) const;
	void fillComboBoxNumber(wxComboBox *box, const std::string &key, const SettingValues *sliceValuesIn,
		const std::string &prefix = "", const std::string &postfix = "") const;
	void fillString( wxTextCtrl  *box, const std::string &key, const SettingValues *sliceValuesIn) const;
	void fillColour( wxColourPickerCtrl  *box, const std::string &key, const SettingValues *sliceValuesIn) const;
	void fillChoiceBoxSelection( wxChoice *box, const std::string &key, const SettingValues *sliceValuesIn) const;
	void fillChoiceBoxString(wxChoice *box, const std::string &key, const SettingValues *sliceValuesIn) const;

	void fillFeedrate(wxTextCtrl *box, const std::string &key, const SettingValues *sliceValuesIn) const;
	void fillFeedrate(wxSpinCtrl *box, const std::string &key, const SettingValues *sliceValuesIn) const;
	double readFeedrate(wxTextCtrl *box) const;
	double readFeedrate(wxSpinCtrl *box) const;

	virtual void OnValueChanged(wxCommandEvent& event) override;
	virtual void OnValueChanged(wxSpinEvent& event) override;

	void fillRetractionSettings(SettingValues *sliceValuesIn);
	void saveRetractionSettings(SettingValues *sliceValuesOut);

	/* Has changed a value needing apply to be activated*/
	void madeChange();
	/* Check if changes need to be saved*/
	bool hasMadeChanges() const;
public:
	RovaSettings(SettingValues *sliceValuesIn);
	~RovaSettings();
	void populateBoxes(SettingValues *sliceValuesIn);
	void saveSettings();
	virtual void SettingsClose(wxCloseEvent& event);
	
	virtual void SwitchRovaCompat(wxCommandEvent& event);
	virtual void kMeansCheck(wxCommandEvent& event);
	virtual void travisSimplificationCheck(wxCommandEvent& event);
	virtual void exportSettings(wxCommandEvent& event);
	virtual void importSettings(wxCommandEvent& event);
	virtual void applyButton(wxCommandEvent& event);
	virtual void ShowHideExperimental(wxCommandEvent& event);
	virtual void onPurgeSoftwareToggle(wxCommandEvent& event);
	virtual void OnUsePurgeBinPrime(wxCommandEvent& event);
	virtual void OnUseCoolingFanCheck(wxCommandEvent& event);
	virtual void reducePrintSpeedOnSmallLayers(wxCommandEvent& event);
	virtual void OnHeadChoice(wxCommandEvent& event);
	virtual void OnUseSyringeLift(wxCommandEvent& event);
	virtual void OnCheckPauseFullBin(wxCommandEvent& event);
	virtual void CMYWhiteSub(wxColourPickerEvent& event);

	//Support
	virtual void OverrideBaseInterfaceCheck(wxCommandEvent& event) override;
	virtual void UpdateInterfaceCount(wxSpinEvent& event) override;

	void LoadDefaultSettings(wxCommandEvent& event);

	
	static Colour getDefaultColour(const std::string &key);
	static double getDefaultDouble(const std::string &key);
	static bool getDefaultBool(const std::string& key);
	static const std::string &getDefaultString(const std::string &key);
};

