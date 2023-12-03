#pragma once
#include "Resource.h"
#include "ResourceBase64.h"
#include "Structs.h"
#include "Functions.h"
#include "InputForm.h"
#include "InfoBox.h"

namespace HaloCEGametypeEditor {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
    using namespace System::Drawing::Text;
	using namespace System::IO;
    using namespace Microsoft::Win32;

	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
            timer_caret = gcnew Timer();
            timer_caret->Interval = 700;
            timer_caret->Tick += gcnew EventHandler(this, &MainForm::timer_caret_tick);
            timer_updatecheck = gcnew Timer();
            timer_updatecheck->Interval = 333;
            timer_updatecheck->Tick += gcnew EventHandler(this, &MainForm::timer_updatecheck_tick);
            timer_browser = gcnew Timer();
            timer_browser->Interval = 55;
            timer_browser->Tick += gcnew EventHandler(this, &MainForm::timer_browser_tick);
            try {
                // Open the registry key for reading
                RegistryKey^ key = Registry::CurrentUser->OpenSubKey(sRegistryKey, true);

                if (key != nullptr) { } else {
                    // Handle the case where the key doesn't exist
                    try {
                        DateTime currentDateTime = DateTime::Now;
                        String^ currentDateTimeString = currentDateTime.ToString();
                        iUserThemeIndex = 0;
                        key = Registry::CurrentUser->CreateSubKey(sRegistryKey);
                        key->SetValue("UserThemeIndex", iUserThemeIndex, RegistryValueKind::DWord);
                        key->SetValue("BackupSetting", iBackupIndex, RegistryValueKind::DWord);
                        key->SetValue("UpdateNotify", 1, RegistryValueKind::DWord);
                        key->SetValue("OpenCount", 1, RegistryValueKind::DWord);
                        key->SetValue("SaveCount", 0, RegistryValueKind::DWord);
                        key->SetValue("LoadCount", 0, RegistryValueKind::DWord);
                        key->SetValue("ShowDebugConsole", 0, RegistryValueKind::DWord);
                        key->SetValue("FirstRun", currentDateTimeString, RegistryValueKind::String);
                        key->SetValue("LastRun", currentDateTimeString, RegistryValueKind::String);
                        key->SetValue("InformationURL", "https://github.com/nocti1/HaloCE-Gametype-Editor", RegistryValueKind::String);
                        key->Close();
                        std::cout << "Successfully saved user configuration to registry." << std::endl;
                        combo_theme->SelectedIndex = iUserThemeIndex;
                    }
                    catch (Exception^ ex) {
                        std::cerr << "Error saving configuration to registry." << std::endl;
                    }
                }
            }
            catch (Exception^ ex) {
                // Handle any exceptions that may occur during registry access
                std::cerr << "Exception while trying to create registry key." << std::endl;
            }

            try {
                // Open the registry key for reading
                RegistryKey^ key = Registry::CurrentUser->OpenSubKey(sRegistryKey, true);

                if (key != nullptr) {
                    // Read the current DWORD value ShowDebugConsole
                    Object^ ShowDebugConsoleValue = key->GetValue("ShowDebugConsole");

                    // Check if the value exists and is an integer
                    if (ShowDebugConsoleValue != nullptr && ShowDebugConsoleValue->GetType() == int::typeid) {
                        if (safe_cast<int>(ShowDebugConsoleValue) == 1) {
                            bDebug = true;
                            bDebugPrompted = true;
                        }
                        else {
                            bDebug = false;
                        }
                        check_debug->Checked = bDebug;
                        std::cout << "ShowDebugConsoleValue: " << bDebug << std::endl;;
                    }
                    else {
                        // If the value doesn't exist or is not an integer, set it to 0
                        key->SetValue("ShowDebugConsole", 0, RegistryValueKind::DWord);
                    }

                    // Try to read the DWORD value UserThemeIndex
                    Object^ value = key->GetValue("UserThemeIndex");
                    DateTime currentDateTime = DateTime::Now;
                    String^ currentDateTimeString = currentDateTime.ToString();
                    key->SetValue("LastRun", currentDateTimeString, RegistryValueKind::String);

                    // Read the current DWORD value
                    Object^ BackupSettingValue = key->GetValue("BackupSetting");

                    // Check if the value exists and is an integer
                    if (BackupSettingValue != nullptr && BackupSettingValue->GetType() == int::typeid) {
                        iBackupIndex = safe_cast<int>(BackupSettingValue);
                        std::cout << "BackupSettingValue: " << iBackupIndex << std::endl;;
                        switch (iBackupIndex) {
                        case 0:
                            combo_backup->SelectedIndex = 2;
                            break;
                        case 1:
                            combo_backup->SelectedIndex = 0;
                            break;
                        case 2:
                            combo_backup->SelectedIndex = 1;
                            break;
                        }
                    }
                    else {
                        // If the value doesn't exist or is not an integer, set it to 0
                        key->SetValue("ShowDebugConsole", 0, RegistryValueKind::DWord);
                    }

                    // Read the current DWORD value UpdateNotify
                    Object^ UpdateNotifyValue = key->GetValue("UpdateNotify");

                    // Check if the value exists and is an integer
                    if (UpdateNotifyValue != nullptr && UpdateNotifyValue->GetType() == int::typeid) {
                        if (safe_cast<int>(UpdateNotifyValue) == 1) {
                            bUpdateNotify = true;
                        }
                        else {
                            bUpdateNotify = false;
                        }
                        check_updatenotify->Checked = bUpdateNotify;
                        std::cout << "UpdateNotifyValue: " << bUpdateNotify << std::endl;;
                    }
                    else {
                        // If the value doesn't exist or is not an integer, set it to 0
                        key->SetValue("UpdateNotify", 1, RegistryValueKind::DWord);
                    }

                    // Read the current DWORD value
                    Object^ openCountValue = key->GetValue("OpenCount");

                    // Check if the value exists and is an integer
                    if (openCountValue != nullptr && openCountValue->GetType() == int::typeid) {
                        // Increment the value
                        int currentCount = safe_cast<int>(openCountValue);
                        int newCount = currentCount + 1;

                        // Save the updated value back to the registry
                        key->SetValue("OpenCount", newCount, RegistryValueKind::DWord);
                    }
                    else {
                        // If the value doesn't exist or is not an integer, set it to 1
                        key->SetValue("OpenCount", 1, RegistryValueKind::DWord);
                    }


                    if (value != nullptr) {
                        // Successfully read the DWORD value, cast it to an integer
                        iUserThemeIndex = static_cast<int>(value);
                        // Now, iUserThemeIndex contains the value
                        std::cout << "Successfully read user configuration from registry." << std::endl;
                        std::cout << "UserThemeIndex: " << iUserThemeIndex << std::endl;

                        combo_theme->SelectedIndex = iUserThemeIndex;

                        // Close the key when you're done with it
                        key->Close();
                    }
                    else {
                        // Handle the case where the value exists but is not a DWORD
                        std::cerr << "UserThemeIndex unexpected data type!" << std::endl;
                    }
                }
                else {
                    std::cerr << "Configuration not saved to registry." << std::endl;
                }
            }
            catch (Exception^ ex) {
                // Handle any exceptions that may occur during registry access
                std::cerr << "Exception while trying to open registry key." << std::endl;
            }
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}

    private: HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    private: String^ sRegistryKey = "SOFTWARE\\noctilucent\\HaloCE Gametype Editor";
    private: Timer^ timer_caret;
    private: Timer^ timer_updatecheck;
    private: Timer^ timer_browser;
    private: System::String^ GtName = "New001";
    private: System::String^ SGtMode = "";
    private: System::String^ SFilePath = "";
    private: float fCurrentVersion = 1.0; // yeah it's a float who gives a fuck
    private: float fLatestVersion = 0.0; // yeah it's a float who gives a fuck
    private: bool bDebug = false;
    private: bool bDebugPrompted = false;
    private: bool bUpdatesThread = false;
    private: bool bUpdatesChecked = false;
    private: bool bUpdateNotify = true;
    private: bool bUpdateAvailable = false;
    private: bool bGitHubReachable = false;
    private: bool bBrowserReady = false;
    private: bool bBrowserLoaded = false;
    private: int iDetailsPage = 0;
    private: PrivateFontCollection^ pfc = gcnew PrivateFontCollection();
    private: System::Windows::Forms::Label^ label_warning;
    private: System::Windows::Forms::PictureBox^ picbox_notify;
    private: System::Windows::Forms::GroupBox^ group_settings;
    private: System::Windows::Forms::CheckBox^ check_debug;

    private: System::Windows::Forms::GroupBox^ group_update;
private: System::Windows::Forms::Label^ label_version;
private: System::Windows::Forms::CheckBox^ check_updatenotify;

private: System::Windows::Forms::Label^ label_theme;
private: System::Windows::Forms::ComboBox^ combo_backup;









    private: int iUserThemeIndex = 0;
private: System::Windows::Forms::GroupBox^ group_github;
private: System::Windows::Forms::Label^ label_github;
private: System::Windows::Forms::TabPage^ file_saveas;
private: System::Windows::Forms::Label^ label_save_no;
private: System::Windows::Forms::Label^ label_saveas_no;








    private: int iBackupIndex = 1;

    private: void ShowConsole() {
        AllocConsole();
        AttachConsole(GetCurrentProcessId());
        freopen("CON", "w", stdout);
        freopen("CON", "w", stderr);
        SetConsoleTitle(L"Debug Console");
    }
    private: void HideConsole() {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        //FreeConsole();
    }

    private: System::Void SetName(System::String^ SName) {
        std::string sName;
        for (int i = 0; i < SName->Length; i++) {
            sName += (char)SName[i];
        }
        text_name->Text = SName;
        label_name->Text = SName;
        list_properties->Items[0]->SubItems[1]->Text = SName;

        SetConsoleTextAttribute(consoleHandle, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
        std::cout << "Name updated: " << sName << std::endl;
        SetConsoleTextAttribute(consoleHandle, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
    private: System::Void SetInt(int iIndex, int iInt) {
        std::string sInt;
        System::String^ SiInt = System::Convert::ToString(static_cast<UInt32>(iInt));
        System::Windows::Forms::ComboBox^ SCombo;
        System::Windows::Forms::CheckBox^ SCheck;
        System::Windows::Forms::NumericUpDown^ SNum;
        System::Windows::Forms::ListViewItem::ListViewSubItem^ SSubItem;
        std::string sIndexName;
        std::string sGtMode;
        for (int i = 0; i < SGtMode->Length; i++) {
            sGtMode += (char)SGtMode[i];
        }

        switch (iIndex) {
        case 0:
            sIndexName = "Game Mode";
            SCombo = combo_gamemode;
            SSubItem = list_properties->Items[1]->SubItems[1];
            switch (iInt) {
            case 1:
                sInt = "Capture The Flag";
                break;
            case 2:
                sInt = "Slayer";
                break;
            case 3:
                sInt = "Oddball";
                break;
            case 4:
                sInt = "King Of The Hill";
                break;
            case 5:
                sInt = "Race";
                break;
            default:
                std::stringstream ss;
                ss << iInt;
                sInt = ss.str() + " (unknown)";
                break;
            }
            break;
        case 1:
            sIndexName = "Score Limit";
            SCombo = combo_scorelimit;
            SSubItem = list_properties->Items[2]->SubItems[1];
            if (sGtMode == "Capture The Flag") {
                switch (iInt) {
                case 1:
                    sInt = "1 Capture";
                    break;
                case 3:
                    sInt = "3 Captures";
                    break;
                case 5:
                    sInt = "5 Captures";
                    break;
                case 10:
                    sInt = "10 Captures";
                    break;
                case 15:
                    sInt = "15 Captures";
                    break;
                default:
                    std::stringstream ss;
                    ss << iInt;
                    sInt = ss.str() + " Captures (custom)";
                    break;
                }
            }
            else if (sGtMode == "Slayer") {
                switch (iInt) {
                case 5:
                    sInt = "5 Kills";
                    break;
                case 10:
                    sInt = "10 Kills";
                    break;
                case 15:
                    sInt = "15 Kills";
                    break;
                case 25:
                    sInt = "25 Kills";
                    break;
                case 50:
                    sInt = "50 Kills";
                    break;
                default:
                    std::stringstream ss;
                    ss << iInt;
                    sInt = ss.str() + " Kills (custom)";
                    break;
                }
            }
            else if (sGtMode == "Oddball" || sGtMode == "King Of The Hill") {
                switch (iInt) {
                case 1:
                    sInt = "1 Minute";
                    break;
                case 2:
                    sInt = "2 Minutes";
                    break;
                case 3:
                    sInt = "3 Minutes";
                    break;
                case 5:
                    sInt = "5 Minutes";
                    break;
                case 10:
                    sInt = "10 Minutes";
                    break;
                case 15:
                    sInt = "15 Minutes";
                    break;
                default:
                    std::stringstream ss;
                    ss << iInt;
                    sInt = ss.str() + " Minutes (custom)";
                    break;
                }
            }
            else if (sGtMode == "Race") {
                switch (iInt) {
                case 1:
                    sInt = "1 Lap";
                    break;
                case 3:
                    sInt = "3 Laps";
                    break;
                case 5:
                    sInt = "5 Laps";
                    break;
                case 10:
                    sInt = "10 Laps";
                    break;
                case 15:
                    sInt = "15 Laps";
                    break;
                case 25:
                    sInt = "25 Laps";
                    break;
                default:
                    std::stringstream ss;
                    ss << iInt;
                    sInt = ss.str() + " Laps (custom)";
                    break;
                }
            }
            break;
        case 2:
            sIndexName = "Time Limit";
            SCombo = combo_timelimit;
            SSubItem = list_properties->Items[3]->SubItems[1];
            switch (iInt) {
            case 0:
                sInt = "None";
                break;
            case 18000:
                sInt = "10 Minutes";
                break;
            case 27000:
                sInt = "15 Minutes";
                break;
            case 36000:
                sInt = "20 Minutes";
                break;
            case 45000:
                sInt = "25 Minutes";
                break;
            case 54000:
                sInt = "30 Minutes";
                break;
            case 81000:
                sInt = "45 Minutes";
                break;
            default:
                std::stringstream ss;
                std::string sMessage;
                if (iInt < 1800) {
                    ss << (iInt / 30);
                    sMessage = ss.str();
                    if (sMessage == "1") {
                        sInt = ss.str() + " Second (custom)";
                    }
                    else {
                        sInt = ss.str() + " Seconds (custom)";
                    }
                }
                else {
                    ss << (iInt / 30) / 60;
                    sMessage = ss.str();
                    if (sMessage == "1") {
                        sInt = ss.str() + " Minute (custom)";
                    }
                    else {
                        sInt = ss.str() + " Minutes (custom)";
                    }
                }
                break;
            }
            break;
        case 3:
            sIndexName = "Modifier 1";
            SCheck = check_gtmod1;
            SSubItem = list_properties->Items[4]->SubItems[1];
            break;
        case 4:
            sIndexName = "Modifier 2";
            SCheck = check_gtmod2;
            SSubItem = list_properties->Items[5]->SubItems[1];
            break;
        case 5:
            sIndexName = "Modifier 3";
            SCheck = check_gtmod3;
            SSubItem = list_properties->Items[6]->SubItems[1];
            break;
        case 6:
            sIndexName = "Modifier 4";
            SCheck = check_gtmod4;
            SSubItem = list_properties->Items[7]->SubItems[1];
            break;
        case 7:
            sIndexName = "Modifier 5";
            SCombo = combo_gtmod5;
            SSubItem = list_properties->Items[8]->SubItems[1];
            if (sGtMode == "Capture The Flag") {
                switch (iInt) {
                case 0:
                    sInt = "Off";
                    break;
                case 1800:
                    sInt = "1 Minute";
                    break;
                case 3600:
                    sInt = "2 Minutes";
                    break;
                case 5400:
                    sInt = "3 Minutes";
                    break;
                case 9000:
                    sInt = "5 Minutes";
                    break;
                case 18000:
                    sInt = "10 Minutes";
                    break;
                }
            }
            else if (sGtMode == "Oddball") {
                switch (iInt) {
                case 0:
                    sInt = "Slow";
                    break;
                case 1:
                    sInt = "Normal";
                    break;
                case 2:
                    sInt = "Fast";
                    break;
                }
            }
            else if (sGtMode == "Race") {
                switch (iInt) {
                case 0:
                    sInt = "Minimum";
                    break;
                case 1:
                    sInt = "Maximum";
                    break;
                case 2:
                    sInt = "Sum";
                    break;
                }
            }
            break;
        case 8:
            sIndexName = "Team Play";
            SCheck = check_teamplay;
            SSubItem = list_properties->Items[9]->SubItems[1];
            break;
        case 9:
            sIndexName = "Auto team balance";
            SCheck = check_teambalance;
            SSubItem = list_properties->Items[10]->SubItems[1];
            break;
        case 10:
            sIndexName = "Friendly fire";
            SCombo = combo_friendlyfire;
            SSubItem = list_properties->Items[11]->SubItems[1];
            switch (iInt) {
            case 0:
                sInt = "Off";
                break;
            case 1:
                sInt = "On";
                break;
            case 2:
                sInt = "Shields Only";
                break;
            case 3:
                sInt = "Explosives Only";
                break;
            }
            break;
        case 11:
            sIndexName = "Friendly fire penalty";
            SCombo = combo_friendlyfirepenalty;
            SSubItem = list_properties->Items[12]->SubItems[1];
            switch (iInt) {
            case 0:
                sInt = "None";
                break;
            case 150:
                sInt = "5 Seconds";
                break;
            case 300:
                sInt = "10 Seconds";
                break;
            case 450:
                sInt = "15 Seconds";
                break;
            default:
                std::stringstream ss;
                std::string sMessage;
                if (iInt < 1800) {
                    ss << (iInt / 30);
                    sMessage = ss.str();
                    if (sMessage == "1") {
                        sInt = ss.str() + " Second (custom)";
                    }
                    else {
                        sInt = ss.str() + " Seconds (custom)";
                    }
                }
                else {
                    ss << (iInt / 30) / 60;
                    sMessage = ss.str();
                    if (sMessage == "1") {
                        sInt = ss.str() + " Minute (custom)";
                    }
                    else {
                        sInt = ss.str() + " Minutes (custom)";
                    }
                }
            }
            break;
        case 12:
            sIndexName = "Number of lives";
            SCombo = combo_lives;
            SSubItem = list_properties->Items[13]->SubItems[1];
            switch (iInt) {
            case 0:
                sInt = "Infinite";
                break;
            case 1:
                sInt = "1";
                break;
            case 3:
                sInt = "3";
                break;
            case 5:
                sInt = "5";
                break;
            default:
                std::stringstream ss;
                ss << iInt;
                sInt = ss.str() + " (custom)";
                break;
            }
            break;
        case 13:
            sIndexName = "Respawn time";
            SCombo = combo_respawntime;
            SSubItem = list_properties->Items[14]->SubItems[1];
            switch (iInt) {
            case 0:
                sInt = "Instant";
                break;
            case 150:
                sInt = "5 Seconds";
                break;
            case 300:
                sInt = "10 Seconds";
                break;
            case 450:
                sInt = "15 Seconds";
                break;
            default:
                std::stringstream ss;
                std::string sMessage;
                if (iInt < 1800) {
                    ss << (iInt / 30);
                    sMessage = ss.str();
                    if (sMessage == "1") {
                        sInt = ss.str() + " Second (custom)";
                    }
                    else {
                        sInt = ss.str() + " Seconds (custom)";
                    }
                }
                else {
                    ss << (iInt / 30) / 60;
                    sMessage = ss.str();
                    if (sMessage == "1") {
                        sInt = ss.str() + " Minute (custom)";
                    }
                    else {
                        sInt = ss.str() + " Minutes (custom)";
                    }
                }
            }
            break;
        case 14:
            sIndexName = "Respawn time growth";
            SCombo = combo_respawntimegrowth;
            SSubItem = list_properties->Items[15]->SubItems[1];
            switch (iInt) {
            case 0:
                sInt = "None";
                break;
            case 150:
                sInt = "5 Seconds";
                break;
            case 300:
                sInt = "10 Seconds";
                break;
            case 450:
                sInt = "15 Seconds";
                break;
            default:
                std::stringstream ss;
                std::string sMessage;
                if (iInt < 1800) {
                    ss << (iInt / 30);
                    sMessage = ss.str();
                    if (sMessage == "1") {
                        sInt = ss.str() + " Second (custom)";
                    }
                    else {
                        sInt = ss.str() + " Seconds (custom)";
                    }
                }
                else {
                    ss << (iInt / 30) / 60;
                    sMessage = ss.str();
                    if (sMessage == "1") {
                        sInt = ss.str() + " Minute (custom)";
                    }
                    else {
                        sInt = ss.str() + " Minutes (custom)";
                    }
                }
            }
            break;
        case 15:
            sIndexName = "Suicide penalty";
            SCombo = combo_suicidepenalty;
            SSubItem = list_properties->Items[16]->SubItems[1];
            switch (iInt) {
            case 0:
                sInt = "None";
                break;
            case 150:
                sInt = "5 Seconds";
                break;
            case 300:
                sInt = "10 Seconds";
                break;
            case 450:
                sInt = "15 Seconds";
                break;
            default:
                std::stringstream ss;
                std::string sMessage;
                if (iInt < 1800) {
                    ss << (iInt / 30);
                    sMessage = ss.str();
                    if (sMessage == "1") {
                        sInt = ss.str() + " Second (custom)";
                    }
                    else {
                        sInt = ss.str() + " Seconds (custom)";
                    }
                }
                else {
                    ss << (iInt / 30) / 60;
                    sMessage = ss.str();
                    if (sMessage == "1") {
                        sInt = ss.str() + " Minute (custom)";
                    }
                    else {
                        sInt = ss.str() + " Minutes (custom)";
                    }
                }
            }
            break;
        case 16:
            sIndexName = "Shields";
            SCheck = check_shields;
            SSubItem = list_properties->Items[18]->SubItems[1];
            break;
        case 17:
            sIndexName = "Odd man out";
            SCheck = check_oddmanout;
            SSubItem = list_properties->Items[19]->SubItems[1];
            break;
        case 18:
            sIndexName = "Invisible players";
            SCheck = check_invisible;
            SSubItem = list_properties->Items[20]->SubItems[1];
            break;
        case 19:
            sIndexName = "Weapon set";
            SCombo = combo_weaponset;
            SSubItem = list_properties->Items[21]->SubItems[1];
            switch (iInt) {
            case 0:
                sInt = "Normal";
                break;
            case 1:
                sInt = "Pistols";
                break;
            case 2:
                sInt = "Rifles";
                break;
            case 3:
                sInt = "Plasma";
                break;
            case 4:
                sInt = "Snipers";
                break;
            case 5:
                sInt = "No Sniping";
                break;
            case 6:
                sInt = "Rockets";
                break;
            case 7:
                sInt = "Shotguns";
                break;
            case 8:
                sInt = "Short range";
                break;
            case 9:
                sInt = "Human";
                break;
            case 10:
                sInt = "Covenant";
                break;
            case 11:
                sInt = "Classic";
                break;
            case 12:
                sInt = "Heavy";
                break;
            default:
                sInt = std::to_string(iInt) + " (unknown)";
                break;
            }
            break;
        case 20:
            sIndexName = "Starting equipment";
            SCombo = combo_startingequipment;
            SSubItem = list_properties->Items[22]->SubItems[1];
            switch (iInt) {
            case 0:
                sInt = "Generic";
                break;
            case 1:
                sInt = "Custom";
                break;
            default:
                sInt = std::to_string(iInt) + " (unknown)";
                break;
            }
            break;
        case 21:
            sIndexName = "Infinite grenades";
            SCheck = check_infgrenades;
            SSubItem = list_properties->Items[23]->SubItems[1];
            break;
        case 22:
            sIndexName = "Vehicle respawn time";
            SCombo = combo_vehiclerespawntime;
            SSubItem = list_properties->Items[24]->SubItems[1];
            switch (iInt) {
            case 0:
                sInt = "Never";
                break;
            case 900:
                sInt = "30 Seconds";
                break;
            case 1800:
                sInt = "60 Seconds";
                break;
            case 2700:
                sInt = "90 Seconds";
                break;
            case 3600:
                sInt = "2 Minutes";
                break;
            case 5400:
                sInt = "3 Minutes";
                break;
            case 9000:
                sInt = "5 Minutes";
                break;
            default:
                std::stringstream ss;
                std::string sMessage;
                if (iInt < 1800) {
                    ss << (iInt / 30);
                    sMessage = ss.str();
                    if (sMessage == "1") {
                        sInt = ss.str() + " Second (custom)";
                    }
                    else {
                        sInt = ss.str() + " Seconds (custom)";
                    }
                }
                else {
                    ss << (iInt / 30) / 60;
                    sMessage = ss.str();
                    if (sMessage == "1") {
                        sInt = ss.str() + " Minute (custom)";
                    }
                    else {
                        sInt = ss.str() + " Minutes (custom)";
                    }
                }
            }
            break;
        case 23:
            sIndexName = "Team0 vehicle set";
            SCombo = combo_redteam_vehicleset;
            SSubItem = list_properties->Items[25]->SubItems[1];
            switch (iInt) {
            case 0:
                sInt = "Default";
                num_redteam_warthog->Value = 4;
                num_redteam_ghost->Value = 4;
                num_redteam_scorpion->Value = 4;
                num_redteam_rockethog->Value = 4;
                num_redteam_banshee->Value = 4;
                num_redteam_gunturret->Value = 4;
                list_properties->Items[26]->SubItems[1]->Text = "4";
                list_properties->Items[27]->SubItems[1]->Text = "4";
                list_properties->Items[28]->SubItems[1]->Text = "4";
                list_properties->Items[29]->SubItems[1]->Text = "4";
                list_properties->Items[30]->SubItems[1]->Text = "4";
                list_properties->Items[31]->SubItems[1]->Text = "4";
                num_redteam_warthog->Enabled = false;
                num_redteam_ghost->Enabled = false;
                num_redteam_scorpion->Enabled = false;
                num_redteam_rockethog->Enabled = false;
                num_redteam_banshee->Enabled = false;
                num_redteam_gunturret->Enabled = false;
                break;
            case 1:
                sInt = "None";
                num_redteam_warthog->Value = 0;
                num_redteam_ghost->Value = 0;
                num_redteam_scorpion->Value = 0;
                num_redteam_rockethog->Value = 0;
                num_redteam_banshee->Value = 0;
                num_redteam_gunturret->Value = 0;
                list_properties->Items[26]->SubItems[1]->Text = "0";
                list_properties->Items[27]->SubItems[1]->Text = "0";
                list_properties->Items[28]->SubItems[1]->Text = "0";
                list_properties->Items[29]->SubItems[1]->Text = "0";
                list_properties->Items[30]->SubItems[1]->Text = "0";
                list_properties->Items[31]->SubItems[1]->Text = "0";
                num_redteam_warthog->Enabled = false;
                num_redteam_ghost->Enabled = false;
                num_redteam_scorpion->Enabled = false;
                num_redteam_rockethog->Enabled = false;
                num_redteam_banshee->Enabled = false;
                num_redteam_gunturret->Enabled = false;
                break;
            case 2:
                sInt = "Warthogs";
                num_redteam_warthog->Value = 4;
                num_redteam_ghost->Value = 0;
                num_redteam_scorpion->Value = 0;
                num_redteam_rockethog->Value = 0;
                num_redteam_banshee->Value = 0;
                num_redteam_gunturret->Value = 0;
                list_properties->Items[26]->SubItems[1]->Text = "4";
                list_properties->Items[27]->SubItems[1]->Text = "0";
                list_properties->Items[28]->SubItems[1]->Text = "0";
                list_properties->Items[29]->SubItems[1]->Text = "0";
                list_properties->Items[30]->SubItems[1]->Text = "0";
                list_properties->Items[31]->SubItems[1]->Text = "0";
                num_redteam_warthog->Enabled = false;
                num_redteam_ghost->Enabled = false;
                num_redteam_scorpion->Enabled = false;
                num_redteam_rockethog->Enabled = false;
                num_redteam_banshee->Enabled = false;
                num_redteam_gunturret->Enabled = false;
                break;
            case 3:
                sInt = "Ghosts";
                num_redteam_warthog->Value = 0;
                num_redteam_ghost->Value = 4;
                num_redteam_scorpion->Value = 0;
                num_redteam_rockethog->Value = 0;
                num_redteam_banshee->Value = 0;
                num_redteam_gunturret->Value = 0;
                list_properties->Items[26]->SubItems[1]->Text = "0";
                list_properties->Items[27]->SubItems[1]->Text = "4";
                list_properties->Items[28]->SubItems[1]->Text = "0";
                list_properties->Items[29]->SubItems[1]->Text = "0";
                list_properties->Items[30]->SubItems[1]->Text = "0";
                list_properties->Items[31]->SubItems[1]->Text = "0";
                num_redteam_warthog->Enabled = false;
                num_redteam_ghost->Enabled = false;
                num_redteam_scorpion->Enabled = false;
                num_redteam_rockethog->Enabled = false;
                num_redteam_banshee->Enabled = false;
                num_redteam_gunturret->Enabled = false;
                break;
            case 4:
                sInt = "Scorpions";
                num_redteam_warthog->Value = 0;
                num_redteam_ghost->Value = 0;
                num_redteam_scorpion->Value = 4;
                num_redteam_rockethog->Value = 0;
                num_redteam_banshee->Value = 0;
                num_redteam_gunturret->Value = 0;
                list_properties->Items[26]->SubItems[1]->Text = "0";
                list_properties->Items[27]->SubItems[1]->Text = "0";
                list_properties->Items[28]->SubItems[1]->Text = "4";
                list_properties->Items[29]->SubItems[1]->Text = "0";
                list_properties->Items[30]->SubItems[1]->Text = "0";
                list_properties->Items[31]->SubItems[1]->Text = "0";
                num_redteam_warthog->Enabled = false;
                num_redteam_ghost->Enabled = false;
                num_redteam_scorpion->Enabled = false;
                num_redteam_rockethog->Enabled = false;
                num_redteam_banshee->Enabled = false;
                num_redteam_gunturret->Enabled = false;
                break;
            case 5:
                sInt = "Rocket Warthogs";
                num_redteam_warthog->Value = 0;
                num_redteam_ghost->Value = 0;
                num_redteam_scorpion->Value = 0;
                num_redteam_rockethog->Value = 4;
                num_redteam_banshee->Value = 0;
                num_redteam_gunturret->Value = 0;
                list_properties->Items[26]->SubItems[1]->Text = "0";
                list_properties->Items[27]->SubItems[1]->Text = "0";
                list_properties->Items[28]->SubItems[1]->Text = "0";
                list_properties->Items[29]->SubItems[1]->Text = "4";
                list_properties->Items[30]->SubItems[1]->Text = "0";
                list_properties->Items[31]->SubItems[1]->Text = "0";
                num_redteam_warthog->Enabled = false;
                num_redteam_ghost->Enabled = false;
                num_redteam_scorpion->Enabled = false;
                num_redteam_rockethog->Enabled = false;
                num_redteam_banshee->Enabled = false;
                num_redteam_gunturret->Enabled = false;
                break;
            case 6:
                sInt = "Banshees";
                num_redteam_warthog->Value = 0;
                num_redteam_ghost->Value = 0;
                num_redteam_scorpion->Value = 0;
                num_redteam_rockethog->Value = 0;
                num_redteam_banshee->Value = 4;
                num_redteam_gunturret->Value = 0;
                list_properties->Items[26]->SubItems[1]->Text = "0";
                list_properties->Items[27]->SubItems[1]->Text = "0";
                list_properties->Items[28]->SubItems[1]->Text = "0";
                list_properties->Items[29]->SubItems[1]->Text = "0";
                list_properties->Items[30]->SubItems[1]->Text = "4";
                list_properties->Items[31]->SubItems[1]->Text = "0";
                num_redteam_warthog->Enabled = false;
                num_redteam_ghost->Enabled = false;
                num_redteam_scorpion->Enabled = false;
                num_redteam_rockethog->Enabled = false;
                num_redteam_banshee->Enabled = false;
                num_redteam_gunturret->Enabled = false;
                break;
            case 7:
                sInt = "Gun Turrets";
                num_redteam_warthog->Value = 0;
                num_redteam_ghost->Value = 0;
                num_redteam_scorpion->Value = 0;
                num_redteam_rockethog->Value = 0;
                num_redteam_banshee->Value = 0;
                num_redteam_gunturret->Value = 4;
                list_properties->Items[26]->SubItems[1]->Text = "0";
                list_properties->Items[27]->SubItems[1]->Text = "0";
                list_properties->Items[28]->SubItems[1]->Text = "0";
                list_properties->Items[29]->SubItems[1]->Text = "0";
                list_properties->Items[30]->SubItems[1]->Text = "0";
                list_properties->Items[31]->SubItems[1]->Text = "4";
                num_redteam_warthog->Enabled = false;
                num_redteam_ghost->Enabled = false;
                num_redteam_scorpion->Enabled = false;
                num_redteam_rockethog->Enabled = false;
                num_redteam_banshee->Enabled = false;
                num_redteam_gunturret->Enabled = false;
                break;
            case 8:
                sInt = "Custom";
                num_redteam_warthog->Enabled = true;
                num_redteam_ghost->Enabled = true;
                num_redteam_scorpion->Enabled = true;
                num_redteam_rockethog->Enabled = true;
                num_redteam_banshee->Enabled = true;
                num_redteam_gunturret->Enabled = true;
                break;
            default:
                sInt = std::to_string(iInt) + " (unknown)";
                break;
            }
            break;
        case 24:
            sIndexName = "Team0 Warthogs";
            SNum = num_redteam_warthog;
            SSubItem = list_properties->Items[26]->SubItems[1];
            sInt = std::to_string(iInt);
            break;
        case 25:
            sIndexName = "Team0 Ghosts";
            SNum = num_redteam_ghost;
            SSubItem = list_properties->Items[27]->SubItems[1];
            sInt = std::to_string(iInt);
            break;
        case 26:
            sIndexName = "Team0 Scorpions";
            SNum = num_redteam_scorpion;
            SSubItem = list_properties->Items[28]->SubItems[1];
            sInt = std::to_string(iInt);
            break;
        case 27:
            sIndexName = "Team0 Rocket Warthogs";
            SNum = num_redteam_rockethog;
            SSubItem = list_properties->Items[29]->SubItems[1];
            sInt = std::to_string(iInt);
            break;
        case 28:
            sIndexName = "Team0 Banshees";
            SNum = num_redteam_banshee;
            SSubItem = list_properties->Items[30]->SubItems[1];
            sInt = std::to_string(iInt);
            break;
        case 29:
            sIndexName = "Team0 Gun Turrets";
            SNum = num_redteam_gunturret;
            SSubItem = list_properties->Items[31]->SubItems[1];
            sInt = std::to_string(iInt);
            break;
        case 30:
            sIndexName = "Team1 vehicle set";
            SCombo = combo_blueteam_vehicleset;
            SSubItem = list_properties->Items[32]->SubItems[1];
            switch (iInt) {
            case 0:
                sInt = "Default";
                num_blueteam_warthog->Value = 4;
                num_blueteam_ghost->Value = 4;
                num_blueteam_scorpion->Value = 4;
                num_blueteam_rockethog->Value = 4;
                num_blueteam_banshee->Value = 4;
                num_blueteam_gunturret->Value = 4;
                list_properties->Items[33]->SubItems[1]->Text = "4";
                list_properties->Items[34]->SubItems[1]->Text = "4";
                list_properties->Items[35]->SubItems[1]->Text = "4";
                list_properties->Items[36]->SubItems[1]->Text = "4";
                list_properties->Items[37]->SubItems[1]->Text = "4";
                list_properties->Items[38]->SubItems[1]->Text = "4";
                num_blueteam_warthog->Enabled = false;
                num_blueteam_ghost->Enabled = false;
                num_blueteam_scorpion->Enabled = false;
                num_blueteam_rockethog->Enabled = false;
                num_blueteam_banshee->Enabled = false;
                num_blueteam_gunturret->Enabled = false;
                break;
            case 1:
                sInt = "None";
                num_blueteam_warthog->Value = 0;
                num_blueteam_ghost->Value = 0;
                num_blueteam_scorpion->Value = 0;
                num_blueteam_rockethog->Value = 0;
                num_blueteam_banshee->Value = 0;
                num_blueteam_gunturret->Value = 0;
                list_properties->Items[33]->SubItems[1]->Text = "0";
                list_properties->Items[34]->SubItems[1]->Text = "0";
                list_properties->Items[35]->SubItems[1]->Text = "0";
                list_properties->Items[36]->SubItems[1]->Text = "0";
                list_properties->Items[37]->SubItems[1]->Text = "0";
                list_properties->Items[38]->SubItems[1]->Text = "0";
                num_blueteam_warthog->Enabled = false;
                num_blueteam_ghost->Enabled = false;
                num_blueteam_scorpion->Enabled = false;
                num_blueteam_rockethog->Enabled = false;
                num_blueteam_banshee->Enabled = false;
                num_blueteam_gunturret->Enabled = false;
                break;
            case 2:
                sInt = "Warthogs";
                num_blueteam_warthog->Value = 4;
                num_blueteam_ghost->Value = 0;
                num_blueteam_scorpion->Value = 0;
                num_blueteam_rockethog->Value = 0;
                num_blueteam_banshee->Value = 0;
                num_blueteam_gunturret->Value = 0;
                list_properties->Items[33]->SubItems[1]->Text = "4";
                list_properties->Items[34]->SubItems[1]->Text = "0";
                list_properties->Items[35]->SubItems[1]->Text = "0";
                list_properties->Items[36]->SubItems[1]->Text = "0";
                list_properties->Items[37]->SubItems[1]->Text = "0";
                list_properties->Items[38]->SubItems[1]->Text = "0";
                num_blueteam_warthog->Enabled = false;
                num_blueteam_ghost->Enabled = false;
                num_blueteam_scorpion->Enabled = false;
                num_blueteam_rockethog->Enabled = false;
                num_blueteam_banshee->Enabled = false;
                num_blueteam_gunturret->Enabled = false;
                break;
            case 3:
                sInt = "Ghosts";
                num_blueteam_warthog->Value = 0;
                num_blueteam_ghost->Value = 4;
                num_blueteam_scorpion->Value = 0;
                num_blueteam_rockethog->Value = 0;
                num_blueteam_banshee->Value = 0;
                num_blueteam_gunturret->Value = 0;
                list_properties->Items[33]->SubItems[1]->Text = "0";
                list_properties->Items[34]->SubItems[1]->Text = "4";
                list_properties->Items[35]->SubItems[1]->Text = "0";
                list_properties->Items[36]->SubItems[1]->Text = "0";
                list_properties->Items[37]->SubItems[1]->Text = "0";
                list_properties->Items[38]->SubItems[1]->Text = "0";
                num_blueteam_warthog->Enabled = false;
                num_blueteam_ghost->Enabled = false;
                num_blueteam_scorpion->Enabled = false;
                num_blueteam_rockethog->Enabled = false;
                num_blueteam_banshee->Enabled = false;
                num_blueteam_gunturret->Enabled = false;
                break;
            case 4:
                sInt = "Scorpions";
                num_blueteam_warthog->Value = 0;
                num_blueteam_ghost->Value = 0;
                num_blueteam_scorpion->Value = 4;
                num_blueteam_rockethog->Value = 0;
                num_blueteam_banshee->Value = 0;
                num_blueteam_gunturret->Value = 0;
                list_properties->Items[33]->SubItems[1]->Text = "0";
                list_properties->Items[34]->SubItems[1]->Text = "0";
                list_properties->Items[35]->SubItems[1]->Text = "4";
                list_properties->Items[36]->SubItems[1]->Text = "0";
                list_properties->Items[37]->SubItems[1]->Text = "0";
                list_properties->Items[38]->SubItems[1]->Text = "0";
                num_blueteam_warthog->Enabled = false;
                num_blueteam_ghost->Enabled = false;
                num_blueteam_scorpion->Enabled = false;
                num_blueteam_rockethog->Enabled = false;
                num_blueteam_banshee->Enabled = false;
                num_blueteam_gunturret->Enabled = false;
                break;
            case 5:
                sInt = "Rocket Warthogs";
                num_blueteam_warthog->Value = 0;
                num_blueteam_ghost->Value = 0;
                num_blueteam_scorpion->Value = 0;
                num_blueteam_rockethog->Value = 4;
                num_blueteam_banshee->Value = 0;
                num_blueteam_gunturret->Value = 0;
                list_properties->Items[33]->SubItems[1]->Text = "0";
                list_properties->Items[34]->SubItems[1]->Text = "0";
                list_properties->Items[35]->SubItems[1]->Text = "0";
                list_properties->Items[36]->SubItems[1]->Text = "4";
                list_properties->Items[37]->SubItems[1]->Text = "0";
                list_properties->Items[38]->SubItems[1]->Text = "0";
                num_blueteam_warthog->Enabled = false;
                num_blueteam_ghost->Enabled = false;
                num_blueteam_scorpion->Enabled = false;
                num_blueteam_rockethog->Enabled = false;
                num_blueteam_banshee->Enabled = false;
                num_blueteam_gunturret->Enabled = false;
                break;
            case 6:
                sInt = "Banshees";
                num_blueteam_warthog->Value = 0;
                num_blueteam_ghost->Value = 0;
                num_blueteam_scorpion->Value = 0;
                num_blueteam_rockethog->Value = 0;
                num_blueteam_banshee->Value = 4;
                num_blueteam_gunturret->Value = 0;
                list_properties->Items[33]->SubItems[1]->Text = "0";
                list_properties->Items[34]->SubItems[1]->Text = "0";
                list_properties->Items[35]->SubItems[1]->Text = "0";
                list_properties->Items[36]->SubItems[1]->Text = "0";
                list_properties->Items[37]->SubItems[1]->Text = "4";
                list_properties->Items[38]->SubItems[1]->Text = "0";
                num_blueteam_warthog->Enabled = false;
                num_blueteam_ghost->Enabled = false;
                num_blueteam_scorpion->Enabled = false;
                num_blueteam_rockethog->Enabled = false;
                num_blueteam_banshee->Enabled = false;
                num_blueteam_gunturret->Enabled = false;
                break;
            case 7:
                sInt = "Gun Turrets";
                num_blueteam_warthog->Value = 0;
                num_blueteam_ghost->Value = 0;
                num_blueteam_scorpion->Value = 0;
                num_blueteam_rockethog->Value = 0;
                num_blueteam_banshee->Value = 0;
                num_blueteam_gunturret->Value = 4;
                list_properties->Items[33]->SubItems[1]->Text = "0";
                list_properties->Items[34]->SubItems[1]->Text = "0";
                list_properties->Items[35]->SubItems[1]->Text = "0";
                list_properties->Items[36]->SubItems[1]->Text = "0";
                list_properties->Items[37]->SubItems[1]->Text = "0";
                list_properties->Items[38]->SubItems[1]->Text = "4";
                num_blueteam_warthog->Enabled = false;
                num_blueteam_ghost->Enabled = false;
                num_blueteam_scorpion->Enabled = false;
                num_blueteam_rockethog->Enabled = false;
                num_blueteam_banshee->Enabled = false;
                num_blueteam_gunturret->Enabled = false;
                break;
            case 8:
                sInt = "Custom";
                num_blueteam_warthog->Enabled = true;
                num_blueteam_ghost->Enabled = true;
                num_blueteam_scorpion->Enabled = true;
                num_blueteam_rockethog->Enabled = true;
                num_blueteam_banshee->Enabled = true;
                num_blueteam_gunturret->Enabled = true;
                break;
            default:
                sInt = std::to_string(iInt) + " (unknown)";
                break;
            }
            break;
        case 31:
            sIndexName = "Team1 Warthogs";
            SNum = num_blueteam_warthog;
            SSubItem = list_properties->Items[33]->SubItems[1];
            sInt = std::to_string(iInt);
            break;
        case 32:
            sIndexName = "Team1 Ghosts";
            SNum = num_blueteam_ghost;
            SSubItem = list_properties->Items[34]->SubItems[1];
            sInt = std::to_string(iInt);
            break;
        case 33:
            sIndexName = "Team1 Scorpions";
            SNum = num_blueteam_scorpion;
            SSubItem = list_properties->Items[35]->SubItems[1];
            sInt = std::to_string(iInt);
            break;
        case 34:
            sIndexName = "Team1 Rocket Warthogs";
            SNum = num_blueteam_rockethog;
            SSubItem = list_properties->Items[36]->SubItems[1];
            sInt = std::to_string(iInt);
            break;
        case 35:
            sIndexName = "Team1 Banshees";
            SNum = num_blueteam_banshee;
            SSubItem = list_properties->Items[37]->SubItems[1];
            sInt = std::to_string(iInt);
            break;
        case 36:
            sIndexName = "Team1 Gun Turrets";
            SNum = num_blueteam_gunturret;
            SSubItem = list_properties->Items[38]->SubItems[1];
            sInt = std::to_string(iInt);
            break;
        case 37:
            sIndexName = "Objectives indicator";
            SCombo = combo_objectives;
            SSubItem = list_properties->Items[39]->SubItems[1];
            sInt = std::to_string(iInt);
            switch (iInt) {
            case 0:
                sInt = "Motion Tracker";
                break;
            case 1:
                sInt = "Nav Points";
                break;
            case 2:
                sInt = "None";
                break;
            }
            break;
        case 38:
            sIndexName = "Enable motion tracker";
            SCombo = combo_radar;
            SSubItem = list_properties->Items[40]->SubItems[1];
            if (iInt == 0) {
                sInt = "No";
            } else if (iInt == 1 && list_properties->Items[41]->SubItems[1]->Text == "0") {
                sInt = "Friends";
            } else if (iInt == 1 && list_properties->Items[41]->SubItems[1]->Text == "1") {
                sInt = "All";
            }
            break;
        case 39:
            sIndexName = "All players on radar";
            SSubItem = list_properties->Items[41]->SubItems[1];
            switch (iInt) {
            case 0:
                SCombo = combo_radar;
                sInt = "Friends";
                break;
            case 1:
                SCombo = combo_radar;
                sInt = "All";
                break;
            }
            if (list_properties->Items[40]->SubItems[1]->Text == "0") {
                list_properties->Items[40]->SubItems[1]->Text = "1";
            }
            break;
        case 40:
            sIndexName = "Friendly indicators on screen";
            SCheck = check_friendindicators;
            SSubItem = list_properties->Items[42]->SubItems[1];
            sInt = std::to_string(iInt);
            break;
        }
        System::String^ SsInt = gcnew System::String(sInt.c_str());
        if (iIndex == 0) { SGtMode = SsInt; }
        if (SCombo) { SCombo->Text = SsInt; }
        if (SCheck) { SCheck->Checked = iInt; }
        if (SNum) { SNum->Value = iInt; }
        SSubItem->Text = SiInt;

        SetConsoleTextAttribute(consoleHandle, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
        std::cout << "Integer " << sIndexName << " updated: " << iInt << " (" << sInt << ") " << std::endl;
        SetConsoleTextAttribute(consoleHandle, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
    private: System::Void SetFloat(int iIndex, std::string sFloat) {
        // Convert std::string to float
        float fFloat = 0.0f;
        try {
            fFloat = std::stof(sFloat);
        }
        catch (const std::invalid_argument&) {
            std::cerr << "Error converting to float! " << sFloat << std::endl;
            return;
        }

        // Format the float value as a string
        System::String^ SfFloat = String::Format("{0:F1}", fFloat);
        System::String^ SsFloat;

        // Variables for UI controls
        System::Windows::Forms::ComboBox^ SCombo = nullptr;
        System::Windows::Forms::ListViewItem::ListViewSubItem^ SSubItem = nullptr;
        std::string sIndexName;

        switch (iIndex) {
        case 0:
            sIndexName = "Health";
            SCombo = combo_health;
            SSubItem = list_properties->Items[17]->SubItems[1];
            if (sFloat.find("0.5") == 0) {
                SsFloat = "50%";
            } else if (sFloat.find("1") == 0) {
                SsFloat = "100%";
            } else if (sFloat.find("1.5") == 0) {
                SsFloat = "150%";
            } else if (sFloat.find("2") == 0) {
                SsFloat = "200%";
            } else if (sFloat.find("3") == 0) {
                SsFloat = "300%";
            } else if (sFloat.find("4") == 0) {
                SsFloat = "400%";
            } else {
                bool isInteger = std::abs(std::fmod(fFloat, 1.0)) < 0.01;
                std::ostringstream ssHealth;
                ssHealth << std::fixed << std::setprecision(0) << (fFloat * 100) << "% (custom)";
                std::string percentageString = ssHealth.str();
                SsFloat = gcnew System::String(percentageString.c_str());
            }
            break;
        }

        // Update UI controls
        if (SCombo) { SCombo->Text = SsFloat; }
        if (SSubItem) { SSubItem->Text = SfFloat; }

        // Set console text color
        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(consoleHandle, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);

        // Output to console
        std::cout << "Float " << sIndexName << " updated: " << fFloat << " (" << sFloat << ") " << std::endl;

        // Reset console text color
        SetConsoleTextAttribute(consoleHandle, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }

    private: System::Windows::Forms::Label^ label_info;
    private: System::Windows::Forms::ListView^ list_properties;
    private: System::Windows::Forms::ColumnHeader^ prop;


    private: System::Windows::Forms::ColumnHeader^ propertyvalue;


    private: System::Windows::Forms::ColumnHeader^ type;

    private: System::Windows::Forms::ColumnHeader^ description;
private: System::Windows::Forms::Label^ label_backup;


    private: System::Windows::Forms::ComboBox^ combo_theme;

    
    private: System::Windows::Forms::GroupBox^ group_details;
    private: System::Windows::Forms::WebBrowser^ browser_details;

    

    private: System::String^ GtNameCaret = GtName;

    private: System::Void timer_caret_tick(System::Object^ sender, System::EventArgs^ e) {
        if (label_name->Text->EndsWith("|")) {
            label_name->Text = label_name->Text->Substring(0, label_name->Text->Length - 1);
        } else {
            label_name->Text = label_name->Text + "|";
        }
    }

    private: System::Void timer_updatecheck_tick(System::Object^ sender, System::EventArgs^ e) {
        std::cout << "Checking for updates..." << std::endl;
        if (bUpdated) {
            CheckForUpdateResponse();
            timer_updatecheck->Stop();
            if (bUpdateNotify && bUpdateAvailable) {
                std::cout << "Notifying user a new update is available." << std::endl;
                tab_main->SelectedIndex = 2;
                using namespace InfoBox;
                CustomInfoBox^ infoBox = gcnew CustomInfoBox();
                infoBox->StartPosition = FormStartPosition::CenterParent; // Center the dialog
                infoBox->Owner = this; // Set the owner to your main form
                //using namespace System;
                //using namespace System::Collections::Generic;
                //using namespace System::Windows::Forms;
                infoBox->SetTitle("Update available!");
                infoBox->SetMessage("A new update is available! Click the update button in the settings tab to open the download page.");
                infoBox->SetWidth(376);
                infoBox->SetIcon("Update");
                if (infoBox->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                }
            }
        } else {
            // Initialize libcurl
            curl_global_init(CURL_GLOBAL_DEFAULT);

            // Create a libcurl handle
            CURL* curl = curl_easy_init();

            if (curl) {
                // Set the API URL
                curl_easy_setopt(curl, CURLOPT_URL, "https://api.github.com/repos/nocti1/HaloCE-Gametype-Editor/releases/latest");

                // Set the User-Agent header
                System::String^ sUserAgent = "HaloCE Gametype Editor/" + String::Format("{0:F1}", fCurrentVersion);
                curl_easy_setopt(curl, CURLOPT_USERAGENT, sUserAgent);

                // Debug shit
                if (bDebug) {
                    curl_easy_setopt(curl, CURLOPT_DEBUGDATA, 1L);
                    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
                }

                // Don't verify SSL, needed for some reason
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

                // Pass the pointer to CurlData
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackUpdate);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &jsonData);

                // Perform the request
                CURLcode res = curl_easy_perform(curl);

                // Check for errors
                if (res != CURLE_OK) {
                    std::string sErr = curl_easy_strerror(res);
                    std::cout << "curl_easy_perform() failed: " << sErr << std::endl;
                    sLastError = sErr;
                }
                else {
                    // Echo result to console
                    std::cout << "Curl finished: " << jsonData << std::endl;
                }

                // Cleanup
                curl_easy_cleanup(curl);
            }
            bUpdated = true;
        }
    }

    private: System::Void timer_browser_tick(System::Object^ sender, System::EventArgs^ e) {
        if (bBrowserReady) {
            std::string htmlCode = GetDetailsPage(iDetailsPage);
            size_t pos = 0;
            const std::string oldString = "style.css";
            const std::string newString = GetTempFilePath() + "\\style.css";
            while ((pos = htmlCode.find(oldString, pos)) != std::string::npos) {
                htmlCode.replace(pos, oldString.length(), newString);
                pos += newString.length();
            }
            browser_details->DocumentText = ConvertStdStringToSystemString(htmlCode);
            bBrowserReady = false;
        }
    }

    private: System::Void UpdateBanner() {
        switch (iUserThemeIndex) {
        case 0: {
            label_name->Image = LoadPngResource(IDB_BT1);
            banner->BackgroundImage = LoadPngResource(IDB_BB1);
            label_info->Image = LoadPngResource(IDB_BI1);
            break;
            }
        case 1:
            label_name->Image = LoadPngResource(IDB_BT2);
            banner->BackgroundImage = LoadPngResource(IDB_BB2);
            label_info->Image = LoadPngResource(IDB_BI2);
            break;
        case 2:
            label_name->Image = LoadPngResource(IDB_BT3);
            banner->BackgroundImage = LoadPngResource(IDB_BB3);
            label_info->Image = LoadPngResource(IDB_BI3);
            break;
        case 3:
            label_name->Image = LoadPngResource(IDB_BT4);
            banner->BackgroundImage = LoadPngResource(IDB_BB4);
            label_info->Image = LoadPngResource(IDB_BI4);
            break;
        case 4:
            label_name->Image = LoadPngResource(IDB_BT5);
            banner->BackgroundImage = LoadPngResource(IDB_BB5);
            label_info->Image = LoadPngResource(IDB_BI5);
            break;
        case 5:
            label_name->Image = LoadPngResource(IDB_BT6);
            banner->BackgroundImage = LoadPngResource(IDB_BB6);
            label_info->Image = LoadPngResource(IDB_BI6);
            break;
        case 6:
            label_name->Image = LoadPngResource(IDB_BT7);
            banner->BackgroundImage = LoadPngResource(IDB_BB7);
            label_info->Image = LoadPngResource(IDB_BI7);
            break;
        }
    }

	private: System::Windows::Forms::Button^ button_close;
	private: System::Windows::Forms::Label^ label_mode;
	protected:





	private: System::Windows::Forms::ComboBox^ combo_gamemode;
	private: System::Windows::Forms::Label^ label_scorelimit;

    private: System::Windows::Forms::CheckBox^ check_teamplay;


   


	private: System::Windows::Forms::CheckBox^ check_oddmanout;
	private: System::Windows::Forms::TextBox^ text_name;




	private: System::Windows::Forms::Label^ label_timelimit;
    private: System::Windows::Forms::GroupBox^ group_player;


    private: System::Windows::Forms::Label^ label_lives;
    private: System::Windows::Forms::ComboBox^ combo_health;
    private: System::Windows::Forms::Label^ label_health;
    private: System::Windows::Forms::CheckBox^ check_shields;


    private: System::Windows::Forms::Label^ label_respawn;


    private: System::Windows::Forms::Label^ label_respawngrowth;
    private: System::Windows::Forms::CheckBox^ check_invisible;


    private: System::Windows::Forms::Label^ label_suicide;
    private: System::Windows::Forms::GroupBox^ group_item;

    private: System::Windows::Forms::CheckBox^ check_infgrenades;
    private: System::Windows::Forms::ComboBox^ combo_weaponset;
    private: System::Windows::Forms::Label^ label_weaponset;


    private: System::Windows::Forms::ComboBox^ combo_startingequipment;

    private: System::Windows::Forms::Label^ label_startingequipment;
    private: System::Windows::Forms::GroupBox^ group_game;
    private: System::Windows::Forms::GroupBox^ group_indicator;
    private: System::Windows::Forms::ComboBox^ combo_radar;
    private: System::Windows::Forms::Label^ label_radar;


    private: System::Windows::Forms::ComboBox^ combo_objectives;
    private: System::Windows::Forms::Label^ label_objectives;
    private: System::Windows::Forms::GroupBox^ group_teamplay;
    private: System::Windows::Forms::CheckBox^ check_friendindicators;



    private: System::Windows::Forms::Label^ label_friendlyfirepenalty;


    private: System::Windows::Forms::CheckBox^ check_teambalance;
    private: System::Windows::Forms::TabControl^ tab_main;


    private: System::Windows::Forms::TabPage^ Normal;
    private: System::Windows::Forms::TabPage^ Advanced;
private: System::Windows::Forms::TabPage^ Settings;

    private: System::Windows::Forms::TabControl^ tab_file;
private: System::Windows::Forms::TabPage^ file_open;
private: System::Windows::Forms::TabPage^ file_save;






private: System::Windows::Forms::CheckBox^ check_gtmod1;
private: System::Windows::Forms::CheckBox^ check_gtmod4;

private: System::Windows::Forms::CheckBox^ check_gtmod3;

private: System::Windows::Forms::CheckBox^ check_gtmod2;
private: System::Windows::Forms::ComboBox^ combo_gtmod5;
private: System::Windows::Forms::Label^ label_gtmod5;
private: System::Windows::Forms::ComboBox^ combo_timelimit;
private: System::Windows::Forms::ComboBox^ combo_scorelimit;
private: System::Windows::Forms::ComboBox^ combo_friendlyfirepenalty;
private: System::Windows::Forms::ComboBox^ combo_suicidepenalty;
private: System::Windows::Forms::ComboBox^ combo_respawntimegrowth;

private: System::Windows::Forms::ComboBox^ combo_respawntime;
private: System::Windows::Forms::ComboBox^ combo_lives;
private: System::Windows::Forms::ComboBox^ combo_friendlyfire;

private: System::Windows::Forms::Label^ label2;
private: System::Windows::Forms::GroupBox^ groupBox1;



private: System::Windows::Forms::ComboBox^ combo_vehiclerespawntime;
private: System::Windows::Forms::Label^ label_vehiclerespawntime;
private: System::Windows::Forms::NumericUpDown^ num_redteam_warthog;



private: System::Windows::Forms::ComboBox^ combo_redteam_vehicleset;

private: System::Windows::Forms::Label^ label_redteam_vehicleset;
private: System::Windows::Forms::Label^ label_redteam_warthog;
private: System::Windows::Forms::GroupBox^ group_redteam;




private: System::Windows::Forms::NumericUpDown^ num_redteam_scorpion;

private: System::Windows::Forms::Label^ label_redteam_scorpion;

private: System::Windows::Forms::NumericUpDown^ num_redteam_ghost;

private: System::Windows::Forms::Label^ label_redteam_ghost;
private: System::Windows::Forms::NumericUpDown^ num_redteam_gunturret;


private: System::Windows::Forms::Label^ label_redteam_gunturret;

private: System::Windows::Forms::NumericUpDown^ num_redteam_banshee;

private: System::Windows::Forms::Label^ label_redteam_banshee;

private: System::Windows::Forms::NumericUpDown^ num_redteam_rockethog;

private: System::Windows::Forms::Label^ label_redteam_rockethog;
private: System::Windows::Forms::GroupBox^ group_blueteam;


private: System::Windows::Forms::NumericUpDown^ num_blueteam_gunturret;

private: System::Windows::Forms::Label^ label_blueteam_gunturret;

private: System::Windows::Forms::NumericUpDown^ num_blueteam_banshee;

private: System::Windows::Forms::Label^ label_blueteam_banshee;

private: System::Windows::Forms::NumericUpDown^ num_blueteam_rockethog;

private: System::Windows::Forms::Label^ label_blueteam_rockethog;

private: System::Windows::Forms::NumericUpDown^ num_blueteam_scorpion;

private: System::Windows::Forms::Label^ label_blueteam_scorpion;

private: System::Windows::Forms::NumericUpDown^ num_blueteam_ghost;

private: System::Windows::Forms::Label^ label_blueteam_ghost;
private: System::Windows::Forms::Label^ label_blueteam_vehicleset;



private: System::Windows::Forms::NumericUpDown^ num_blueteam_warthog;

private: System::Windows::Forms::Label^ label_blueteam_warthog;
private: System::Windows::Forms::ComboBox^ combo_blueteam_vehicleset;
private: System::Windows::Forms::PictureBox^ banner;

private: System::Windows::Forms::Label^ label_name;
private: System::ComponentModel::IContainer^ components;























































    


	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
            System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(MainForm::typeid));
            System::Windows::Forms::ListViewGroup^ listViewGroup1 = (gcnew System::Windows::Forms::ListViewGroup(L"Game options", System::Windows::Forms::HorizontalAlignment::Left));
            System::Windows::Forms::ListViewGroup^ listViewGroup2 = (gcnew System::Windows::Forms::ListViewGroup(L"Teamplay options", System::Windows::Forms::HorizontalAlignment::Left));
            System::Windows::Forms::ListViewGroup^ listViewGroup3 = (gcnew System::Windows::Forms::ListViewGroup(L"Player options", System::Windows::Forms::HorizontalAlignment::Left));
            System::Windows::Forms::ListViewGroup^ listViewGroup4 = (gcnew System::Windows::Forms::ListViewGroup(L"Item options", System::Windows::Forms::HorizontalAlignment::Left));
            System::Windows::Forms::ListViewGroup^ listViewGroup5 = (gcnew System::Windows::Forms::ListViewGroup(L"Vehicle options", System::Windows::Forms::HorizontalAlignment::Left));
            System::Windows::Forms::ListViewGroup^ listViewGroup6 = (gcnew System::Windows::Forms::ListViewGroup(L"Indicator options", System::Windows::Forms::HorizontalAlignment::Left));
            System::Windows::Forms::ListViewItem^ listViewItem1 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Name",
                    L"New001", L"String (23 characters)", L"The name of the current gametype"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem2 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Game mode",
                    L"", L"Integer (0-5)", L"Game mode index"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem3 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Score limit",
                    L"", L"Integer (0-65536)", L"Score limit"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem4 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Time limit",
                    L"", L"Integer (0-65536)", L"Time limit in ticks (30 per second)"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem5 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Modifier 1",
                    L"", L"Integer (0/1)", L"Game mode modifier 1"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem6 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Modifier 2",
                    L"", L"Integer (0/1)", L"Game mode modifier 2"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem7 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Modifier 3",
                    L"", L"Integer (0/1)", L"Game mode modifier 3"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem8 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Modifier 4",
                    L"", L"Integer (0/1)", L"Game mode modifier 4"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem9 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Modifier 5",
                    L"", L"Integer (0-65536)", L"Game mode modifier 5"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem10 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Team play",
                    L"", L"Integer (0/1)", L"Team play"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem11 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Auto team balance",
                    L"", L"Integer (0/1)", L"Automatically keep teams balanced"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem12 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Friendly fire",
                    L"", L"Integer (0-3)", L"Friendly fire"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem13 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Friendly fire penalty",
                    L"", L"Integer (0-65536)", L"Friendly fire in ticks (30 per second)"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem14 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Number of lives",
                    L"", L"Integer (0-65536)", L"Number of lives for each player"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem15 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Respawn time",
                    L"", L"Integer (0-65536)", L"Respawn time in ticks (30 per second)"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem16 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Respawn time growth",
                    L"", L"Integer (0-65536)", L"Respawn time growth in ticks (30 per second)"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem17 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Suicide penalty",
                    L"", L"Integer (0-65536)", L"Time penalty in ticks (30 per second) when a player suicides"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem18 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Health",
                    L"", L"Float (0.0-10.0)", L"Health modifier for each player"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem19 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Shields",
                    L"", L"Integer (0/1)", L"Shields"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem20 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Odd man out",
                    L"", L"Integer (0/1)", L"Odd man out"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem21 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Invisible players",
                    L"", L"Integer (0/1)", L"Players are invisible"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem22 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Weapon set",
                    L"", L"Integer (0-12)", L"Weapon set"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem23 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Starting equipment",
                    L"", L"Integer (0/1)", L"Custom starting equipment"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem24 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Infinite grenades",
                    L"", L"Integer (0/1)", L"Infinite grenades"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem25 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Vehicle respawn time",
                    L"", L"Integer (0-65536)", L"Vehicle respawn time in ticks (30 per second)"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem26 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Team0 vehicle set",
                    L"", L"Integer (0-8)", L"Vehicle set for team 0"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem27 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Team0 Warthogs",
                    L"", L"Integer (0-4)", L"How many Warthogs to spawn for team 0"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem28 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Team0 Ghosts",
                    L"", L"Integer (0-4)", L"How many Ghosts to spawn for team 0"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem29 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Team0 Scorpions",
                    L"", L"Integer (0-4)", L"How many Scorpions to spawn for team 0"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem30 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Team0 Rocket Warthogs",
                    L"", L"Integer (0-4)", L"How many Rocket Hogs to spawn for team 0"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem31 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Team0 Banshees",
                    L"", L"Integer (0-4)", L"How many Banshees to spawn for team 0"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem32 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Team0 Gun Turrets",
                    L"", L"Integer (0-4)", L"How many Gun Turrets to spawn for team 0"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem33 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Team1 vehicle set ",
                    L"", L"Integer (0-8)", L"Vehicle set for team 1"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem34 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Team1 Warthogs",
                    L"", L"Integer (0-4)", L"How many Warthogs to spawn for team 1"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem35 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Team1 Ghosts",
                    L"", L"Integer (0-4)", L"How many Ghosts to spawn for team 1"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem36 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Team1 Scorpions",
                    L"", L"Integer (0-4)", L"How many Scorpions to spawn for team 1"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem37 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Team1 Rocket Warthogs",
                    L"", L"Integer (0-4)", L"How many Rocket Hogs to spawn for team 1"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem38 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Team1 Banshees",
                    L"", L"Integer (0-4)", L"How many Banshees to spawn for team 1"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem39 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Team1 Gun Turrets",
                    L"", L"Integer (0-4)", L"How many Gun Turrets to spawn for team 1"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem40 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Objectives indicator",
                    L"", L"Integer (0-2)", L"Objectives indicator"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem41 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Enable motion tracker",
                    L"", L"Integer (0/1)", L"Enable motion tracker"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem42 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"All players on motion tracker",
                    L"", L"Integer (0/1)", L"All players on motion tracker"
            }, -1));
            System::Windows::Forms::ListViewItem^ listViewItem43 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {
                L"Friendly indicators on screen",
                    L"", L"Integer (0/1)", L"Friendly indicators on screen"
            }, -1));
            this->button_close = (gcnew System::Windows::Forms::Button());
            this->label_mode = (gcnew System::Windows::Forms::Label());
            this->combo_gamemode = (gcnew System::Windows::Forms::ComboBox());
            this->label_scorelimit = (gcnew System::Windows::Forms::Label());
            this->check_teamplay = (gcnew System::Windows::Forms::CheckBox());
            this->check_oddmanout = (gcnew System::Windows::Forms::CheckBox());
            this->text_name = (gcnew System::Windows::Forms::TextBox());
            this->label_timelimit = (gcnew System::Windows::Forms::Label());
            this->group_player = (gcnew System::Windows::Forms::GroupBox());
            this->combo_lives = (gcnew System::Windows::Forms::ComboBox());
            this->combo_respawntimegrowth = (gcnew System::Windows::Forms::ComboBox());
            this->combo_respawntime = (gcnew System::Windows::Forms::ComboBox());
            this->combo_suicidepenalty = (gcnew System::Windows::Forms::ComboBox());
            this->check_invisible = (gcnew System::Windows::Forms::CheckBox());
            this->label_suicide = (gcnew System::Windows::Forms::Label());
            this->label_respawngrowth = (gcnew System::Windows::Forms::Label());
            this->check_shields = (gcnew System::Windows::Forms::CheckBox());
            this->label_respawn = (gcnew System::Windows::Forms::Label());
            this->combo_health = (gcnew System::Windows::Forms::ComboBox());
            this->label_health = (gcnew System::Windows::Forms::Label());
            this->label_lives = (gcnew System::Windows::Forms::Label());
            this->group_item = (gcnew System::Windows::Forms::GroupBox());
            this->combo_startingequipment = (gcnew System::Windows::Forms::ComboBox());
            this->label_startingequipment = (gcnew System::Windows::Forms::Label());
            this->combo_weaponset = (gcnew System::Windows::Forms::ComboBox());
            this->label_weaponset = (gcnew System::Windows::Forms::Label());
            this->check_infgrenades = (gcnew System::Windows::Forms::CheckBox());
            this->group_game = (gcnew System::Windows::Forms::GroupBox());
            this->combo_scorelimit = (gcnew System::Windows::Forms::ComboBox());
            this->combo_timelimit = (gcnew System::Windows::Forms::ComboBox());
            this->combo_gtmod5 = (gcnew System::Windows::Forms::ComboBox());
            this->label_gtmod5 = (gcnew System::Windows::Forms::Label());
            this->check_gtmod4 = (gcnew System::Windows::Forms::CheckBox());
            this->check_gtmod3 = (gcnew System::Windows::Forms::CheckBox());
            this->check_gtmod2 = (gcnew System::Windows::Forms::CheckBox());
            this->check_gtmod1 = (gcnew System::Windows::Forms::CheckBox());
            this->group_indicator = (gcnew System::Windows::Forms::GroupBox());
            this->check_friendindicators = (gcnew System::Windows::Forms::CheckBox());
            this->combo_radar = (gcnew System::Windows::Forms::ComboBox());
            this->label_radar = (gcnew System::Windows::Forms::Label());
            this->combo_objectives = (gcnew System::Windows::Forms::ComboBox());
            this->label_objectives = (gcnew System::Windows::Forms::Label());
            this->group_teamplay = (gcnew System::Windows::Forms::GroupBox());
            this->combo_friendlyfire = (gcnew System::Windows::Forms::ComboBox());
            this->label2 = (gcnew System::Windows::Forms::Label());
            this->combo_friendlyfirepenalty = (gcnew System::Windows::Forms::ComboBox());
            this->label_friendlyfirepenalty = (gcnew System::Windows::Forms::Label());
            this->check_teambalance = (gcnew System::Windows::Forms::CheckBox());
            this->tab_main = (gcnew System::Windows::Forms::TabControl());
            this->Normal = (gcnew System::Windows::Forms::TabPage());
            this->banner = (gcnew System::Windows::Forms::PictureBox());
            this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
            this->group_blueteam = (gcnew System::Windows::Forms::GroupBox());
            this->num_blueteam_gunturret = (gcnew System::Windows::Forms::NumericUpDown());
            this->label_blueteam_gunturret = (gcnew System::Windows::Forms::Label());
            this->num_blueteam_banshee = (gcnew System::Windows::Forms::NumericUpDown());
            this->label_blueteam_banshee = (gcnew System::Windows::Forms::Label());
            this->num_blueteam_rockethog = (gcnew System::Windows::Forms::NumericUpDown());
            this->label_blueteam_rockethog = (gcnew System::Windows::Forms::Label());
            this->num_blueteam_scorpion = (gcnew System::Windows::Forms::NumericUpDown());
            this->label_blueteam_scorpion = (gcnew System::Windows::Forms::Label());
            this->num_blueteam_ghost = (gcnew System::Windows::Forms::NumericUpDown());
            this->label_blueteam_ghost = (gcnew System::Windows::Forms::Label());
            this->label_blueteam_vehicleset = (gcnew System::Windows::Forms::Label());
            this->num_blueteam_warthog = (gcnew System::Windows::Forms::NumericUpDown());
            this->label_blueteam_warthog = (gcnew System::Windows::Forms::Label());
            this->combo_blueteam_vehicleset = (gcnew System::Windows::Forms::ComboBox());
            this->group_redteam = (gcnew System::Windows::Forms::GroupBox());
            this->num_redteam_gunturret = (gcnew System::Windows::Forms::NumericUpDown());
            this->label_redteam_gunturret = (gcnew System::Windows::Forms::Label());
            this->num_redteam_banshee = (gcnew System::Windows::Forms::NumericUpDown());
            this->label_redteam_banshee = (gcnew System::Windows::Forms::Label());
            this->num_redteam_rockethog = (gcnew System::Windows::Forms::NumericUpDown());
            this->label_redteam_rockethog = (gcnew System::Windows::Forms::Label());
            this->num_redteam_scorpion = (gcnew System::Windows::Forms::NumericUpDown());
            this->label_redteam_scorpion = (gcnew System::Windows::Forms::Label());
            this->num_redteam_ghost = (gcnew System::Windows::Forms::NumericUpDown());
            this->label_redteam_ghost = (gcnew System::Windows::Forms::Label());
            this->label_redteam_vehicleset = (gcnew System::Windows::Forms::Label());
            this->num_redteam_warthog = (gcnew System::Windows::Forms::NumericUpDown());
            this->label_redteam_warthog = (gcnew System::Windows::Forms::Label());
            this->combo_redteam_vehicleset = (gcnew System::Windows::Forms::ComboBox());
            this->combo_vehiclerespawntime = (gcnew System::Windows::Forms::ComboBox());
            this->label_vehiclerespawntime = (gcnew System::Windows::Forms::Label());
            this->Advanced = (gcnew System::Windows::Forms::TabPage());
            this->group_details = (gcnew System::Windows::Forms::GroupBox());
            this->browser_details = (gcnew System::Windows::Forms::WebBrowser());
            this->list_properties = (gcnew System::Windows::Forms::ListView());
            this->prop = (gcnew System::Windows::Forms::ColumnHeader());
            this->propertyvalue = (gcnew System::Windows::Forms::ColumnHeader());
            this->type = (gcnew System::Windows::Forms::ColumnHeader());
            this->description = (gcnew System::Windows::Forms::ColumnHeader());
            this->Settings = (gcnew System::Windows::Forms::TabPage());
            this->group_github = (gcnew System::Windows::Forms::GroupBox());
            this->label_github = (gcnew System::Windows::Forms::Label());
            this->group_update = (gcnew System::Windows::Forms::GroupBox());
            this->check_updatenotify = (gcnew System::Windows::Forms::CheckBox());
            this->label_version = (gcnew System::Windows::Forms::Label());
            this->group_settings = (gcnew System::Windows::Forms::GroupBox());
            this->label_theme = (gcnew System::Windows::Forms::Label());
            this->combo_backup = (gcnew System::Windows::Forms::ComboBox());
            this->check_debug = (gcnew System::Windows::Forms::CheckBox());
            this->label_backup = (gcnew System::Windows::Forms::Label());
            this->combo_theme = (gcnew System::Windows::Forms::ComboBox());
            this->label_info = (gcnew System::Windows::Forms::Label());
            this->label_name = (gcnew System::Windows::Forms::Label());
            this->tab_file = (gcnew System::Windows::Forms::TabControl());
            this->file_open = (gcnew System::Windows::Forms::TabPage());
            this->file_save = (gcnew System::Windows::Forms::TabPage());
            this->file_saveas = (gcnew System::Windows::Forms::TabPage());
            this->label_warning = (gcnew System::Windows::Forms::Label());
            this->picbox_notify = (gcnew System::Windows::Forms::PictureBox());
            this->label_save_no = (gcnew System::Windows::Forms::Label());
            this->label_saveas_no = (gcnew System::Windows::Forms::Label());
            this->group_player->SuspendLayout();
            this->group_item->SuspendLayout();
            this->group_game->SuspendLayout();
            this->group_indicator->SuspendLayout();
            this->group_teamplay->SuspendLayout();
            this->tab_main->SuspendLayout();
            this->Normal->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->banner))->BeginInit();
            this->groupBox1->SuspendLayout();
            this->group_blueteam->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_blueteam_gunturret))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_blueteam_banshee))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_blueteam_rockethog))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_blueteam_scorpion))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_blueteam_ghost))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_blueteam_warthog))->BeginInit();
            this->group_redteam->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_redteam_gunturret))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_redteam_banshee))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_redteam_rockethog))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_redteam_scorpion))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_redteam_ghost))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_redteam_warthog))->BeginInit();
            this->Advanced->SuspendLayout();
            this->group_details->SuspendLayout();
            this->Settings->SuspendLayout();
            this->group_github->SuspendLayout();
            this->group_update->SuspendLayout();
            this->group_settings->SuspendLayout();
            this->tab_file->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->picbox_notify))->BeginInit();
            this->SuspendLayout();
            // 
            // button_close
            // 
            this->button_close->Location = System::Drawing::Point(656, 673);
            this->button_close->Name = L"button_close";
            this->button_close->Size = System::Drawing::Size(75, 23);
            this->button_close->TabIndex = 0;
            this->button_close->Text = L"Close";
            this->button_close->UseVisualStyleBackColor = true;
            this->button_close->Click += gcnew System::EventHandler(this, &MainForm::button_close_Click);
            // 
            // label_mode
            // 
            this->label_mode->AutoSize = true;
            this->label_mode->Location = System::Drawing::Point(9, 19);
            this->label_mode->Name = L"label_mode";
            this->label_mode->Size = System::Drawing::Size(67, 13);
            this->label_mode->TabIndex = 1;
            this->label_mode->Text = L"Game mode:";
            // 
            // combo_gamemode
            // 
            this->combo_gamemode->DropDownHeight = 100;
            this->combo_gamemode->FormattingEnabled = true;
            this->combo_gamemode->IntegralHeight = false;
            this->combo_gamemode->Items->AddRange(gcnew cli::array< System::Object^  >(5) {
                L"Capture The Flag", L"Slayer", L"Oddball",
                    L"King Of The Hill", L"Race"
            });
            this->combo_gamemode->Location = System::Drawing::Point(12, 35);
            this->combo_gamemode->Name = L"combo_gamemode";
            this->combo_gamemode->Size = System::Drawing::Size(110, 21);
            this->combo_gamemode->TabIndex = 4;
            this->combo_gamemode->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::combo_gamemode_changed);
            // 
            // label_scorelimit
            // 
            this->label_scorelimit->AutoSize = true;
            this->label_scorelimit->Location = System::Drawing::Point(9, 64);
            this->label_scorelimit->Name = L"label_scorelimit";
            this->label_scorelimit->Size = System::Drawing::Size(58, 13);
            this->label_scorelimit->TabIndex = 5;
            this->label_scorelimit->Text = L"Score limit:";
            // 
            // check_teamplay
            // 
            this->check_teamplay->AutoSize = true;
            this->check_teamplay->Location = System::Drawing::Point(12, 156);
            this->check_teamplay->Name = L"check_teamplay";
            this->check_teamplay->Size = System::Drawing::Size(75, 17);
            this->check_teamplay->TabIndex = 7;
            this->check_teamplay->Text = L"Team play";
            this->check_teamplay->UseVisualStyleBackColor = true;
            this->check_teamplay->CheckedChanged += gcnew System::EventHandler(this, &MainForm::check_team_changed);
            // 
            // check_oddmanout
            // 
            this->check_oddmanout->AutoSize = true;
            this->check_oddmanout->Location = System::Drawing::Point(137, 135);
            this->check_oddmanout->Name = L"check_oddmanout";
            this->check_oddmanout->Size = System::Drawing::Size(87, 17);
            this->check_oddmanout->TabIndex = 8;
            this->check_oddmanout->Text = L"Odd man out";
            this->check_oddmanout->UseVisualStyleBackColor = true;
            this->check_oddmanout->CheckedChanged += gcnew System::EventHandler(this, &MainForm::check_oddmanout_changed);
            // 
            // text_name
            // 
            this->text_name->BackColor = System::Drawing::SystemColors::Window;
            this->text_name->Location = System::Drawing::Point(329, 74);
            this->text_name->MaxLength = 23;
            this->text_name->Name = L"text_name";
            this->text_name->Size = System::Drawing::Size(110, 20);
            this->text_name->TabIndex = 10;
            this->text_name->Text = L"New001";
            this->text_name->TextChanged += gcnew System::EventHandler(this, &MainForm::text_name_changed);
            this->text_name->Enter += gcnew System::EventHandler(this, &MainForm::label_name_enterfocus);
            this->text_name->Leave += gcnew System::EventHandler(this, &MainForm::label_name_leavefocus);
            // 
            // label_timelimit
            // 
            this->label_timelimit->AutoSize = true;
            this->label_timelimit->Location = System::Drawing::Point(9, 109);
            this->label_timelimit->Name = L"label_timelimit";
            this->label_timelimit->Size = System::Drawing::Size(53, 13);
            this->label_timelimit->TabIndex = 11;
            this->label_timelimit->Text = L"Time limit:";
            // 
            // group_player
            // 
            this->group_player->Controls->Add(this->combo_lives);
            this->group_player->Controls->Add(this->combo_respawntimegrowth);
            this->group_player->Controls->Add(this->combo_respawntime);
            this->group_player->Controls->Add(this->combo_suicidepenalty);
            this->group_player->Controls->Add(this->check_invisible);
            this->group_player->Controls->Add(this->label_suicide);
            this->group_player->Controls->Add(this->label_respawngrowth);
            this->group_player->Controls->Add(this->check_oddmanout);
            this->group_player->Controls->Add(this->check_shields);
            this->group_player->Controls->Add(this->label_respawn);
            this->group_player->Controls->Add(this->combo_health);
            this->group_player->Controls->Add(this->label_health);
            this->group_player->Controls->Add(this->label_lives);
            this->group_player->Location = System::Drawing::Point(290, 79);
            this->group_player->Name = L"group_player";
            this->group_player->Size = System::Drawing::Size(259, 183);
            this->group_player->TabIndex = 13;
            this->group_player->TabStop = false;
            this->group_player->Text = L"Player options";
            this->group_player->Enter += gcnew System::EventHandler(this, &MainForm::groupBox1_Enter);
            // 
            // combo_lives
            // 
            this->combo_lives->DropDownHeight = 100;
            this->combo_lives->FormattingEnabled = true;
            this->combo_lives->IntegralHeight = false;
            this->combo_lives->Items->AddRange(gcnew cli::array< System::Object^  >(4) { L"Infinite", L"1", L"3", L"5" });
            this->combo_lives->Location = System::Drawing::Point(12, 35);
            this->combo_lives->Name = L"combo_lives";
            this->combo_lives->Size = System::Drawing::Size(110, 21);
            this->combo_lives->TabIndex = 26;
            this->combo_lives->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::combo_lives_changed);
            // 
            // combo_respawntimegrowth
            // 
            this->combo_respawntimegrowth->DropDownHeight = 100;
            this->combo_respawntimegrowth->FormattingEnabled = true;
            this->combo_respawntimegrowth->IntegralHeight = false;
            this->combo_respawntimegrowth->Items->AddRange(gcnew cli::array< System::Object^  >(4) {
                L"None", L"5 Seconds", L"10 Seconds",
                    L"15 Seconds"
            });
            this->combo_respawntimegrowth->Location = System::Drawing::Point(137, 80);
            this->combo_respawntimegrowth->Name = L"combo_respawntimegrowth";
            this->combo_respawntimegrowth->Size = System::Drawing::Size(110, 21);
            this->combo_respawntimegrowth->TabIndex = 25;
            this->combo_respawntimegrowth->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::combo_respawntimegrowth_changed);
            // 
            // combo_respawntime
            // 
            this->combo_respawntime->DropDownHeight = 100;
            this->combo_respawntime->FormattingEnabled = true;
            this->combo_respawntime->IntegralHeight = false;
            this->combo_respawntime->Items->AddRange(gcnew cli::array< System::Object^  >(4) {
                L"Instant", L"5 Seconds", L"10 Seconds",
                    L"15 Seconds"
            });
            this->combo_respawntime->Location = System::Drawing::Point(12, 80);
            this->combo_respawntime->Name = L"combo_respawntime";
            this->combo_respawntime->Size = System::Drawing::Size(110, 21);
            this->combo_respawntime->TabIndex = 24;
            this->combo_respawntime->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::combo_respawntime_changed);
            // 
            // combo_suicidepenalty
            // 
            this->combo_suicidepenalty->DropDownHeight = 100;
            this->combo_suicidepenalty->FormattingEnabled = true;
            this->combo_suicidepenalty->IntegralHeight = false;
            this->combo_suicidepenalty->Items->AddRange(gcnew cli::array< System::Object^  >(4) {
                L"None", L"5 Seconds", L"10 Seconds",
                    L"15 Seconds"
            });
            this->combo_suicidepenalty->Location = System::Drawing::Point(137, 35);
            this->combo_suicidepenalty->Name = L"combo_suicidepenalty";
            this->combo_suicidepenalty->Size = System::Drawing::Size(110, 21);
            this->combo_suicidepenalty->TabIndex = 23;
            this->combo_suicidepenalty->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::combo_suicidepenalty_changed);
            // 
            // check_invisible
            // 
            this->check_invisible->AutoSize = true;
            this->check_invisible->Location = System::Drawing::Point(137, 155);
            this->check_invisible->Name = L"check_invisible";
            this->check_invisible->Size = System::Drawing::Size(100, 17);
            this->check_invisible->TabIndex = 21;
            this->check_invisible->Text = L"Invisible players";
            this->check_invisible->UseVisualStyleBackColor = true;
            this->check_invisible->CheckedChanged += gcnew System::EventHandler(this, &MainForm::check_invisible_changed);
            // 
            // label_suicide
            // 
            this->label_suicide->AutoSize = true;
            this->label_suicide->Location = System::Drawing::Point(134, 19);
            this->label_suicide->Name = L"label_suicide";
            this->label_suicide->Size = System::Drawing::Size(82, 13);
            this->label_suicide->TabIndex = 22;
            this->label_suicide->Text = L"Suicide penalty:";
            // 
            // label_respawngrowth
            // 
            this->label_respawngrowth->AutoSize = true;
            this->label_respawngrowth->Location = System::Drawing::Point(134, 64);
            this->label_respawngrowth->Name = L"label_respawngrowth";
            this->label_respawngrowth->Size = System::Drawing::Size(112, 13);
            this->label_respawngrowth->TabIndex = 19;
            this->label_respawngrowth->Text = L"Respawn time growth:";
            // 
            // check_shields
            // 
            this->check_shields->AutoSize = true;
            this->check_shields->Location = System::Drawing::Point(137, 115);
            this->check_shields->Name = L"check_shields";
            this->check_shields->Size = System::Drawing::Size(60, 17);
            this->check_shields->TabIndex = 14;
            this->check_shields->Text = L"Shields";
            this->check_shields->UseVisualStyleBackColor = true;
            this->check_shields->CheckedChanged += gcnew System::EventHandler(this, &MainForm::check_shields_changed);
            // 
            // label_respawn
            // 
            this->label_respawn->AutoSize = true;
            this->label_respawn->Location = System::Drawing::Point(9, 64);
            this->label_respawn->Name = L"label_respawn";
            this->label_respawn->Size = System::Drawing::Size(77, 13);
            this->label_respawn->TabIndex = 17;
            this->label_respawn->Text = L"Respawn time:";
            // 
            // combo_health
            // 
            this->combo_health->DropDownHeight = 100;
            this->combo_health->FormattingEnabled = true;
            this->combo_health->IntegralHeight = false;
            this->combo_health->Items->AddRange(gcnew cli::array< System::Object^  >(6) {
                L"50%", L"100%", L"150%", L"200%", L"300%",
                    L"400%"
            });
            this->combo_health->Location = System::Drawing::Point(12, 125);
            this->combo_health->Name = L"combo_health";
            this->combo_health->Size = System::Drawing::Size(110, 21);
            this->combo_health->TabIndex = 14;
            this->combo_health->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::combo_health_changed);
            // 
            // label_health
            // 
            this->label_health->AutoSize = true;
            this->label_health->Location = System::Drawing::Point(9, 109);
            this->label_health->Name = L"label_health";
            this->label_health->Size = System::Drawing::Size(41, 13);
            this->label_health->TabIndex = 16;
            this->label_health->Text = L"Health:";
            // 
            // label_lives
            // 
            this->label_lives->AutoSize = true;
            this->label_lives->Location = System::Drawing::Point(9, 19);
            this->label_lives->Name = L"label_lives";
            this->label_lives->Size = System::Drawing::Size(83, 13);
            this->label_lives->TabIndex = 14;
            this->label_lives->Text = L"Number of lives:";
            // 
            // group_item
            // 
            this->group_item->Controls->Add(this->combo_startingequipment);
            this->group_item->Controls->Add(this->label_startingequipment);
            this->group_item->Controls->Add(this->combo_weaponset);
            this->group_item->Controls->Add(this->label_weaponset);
            this->group_item->Controls->Add(this->check_infgrenades);
            this->group_item->Location = System::Drawing::Point(564, 79);
            this->group_item->Name = L"group_item";
            this->group_item->Size = System::Drawing::Size(134, 183);
            this->group_item->TabIndex = 14;
            this->group_item->TabStop = false;
            this->group_item->Text = L"Item options";
            // 
            // combo_startingequipment
            // 
            this->combo_startingequipment->DropDownHeight = 100;
            this->combo_startingequipment->FormattingEnabled = true;
            this->combo_startingequipment->IntegralHeight = false;
            this->combo_startingequipment->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"Generic", L"Custom" });
            this->combo_startingequipment->Location = System::Drawing::Point(12, 80);
            this->combo_startingequipment->Name = L"combo_startingequipment";
            this->combo_startingequipment->Size = System::Drawing::Size(110, 21);
            this->combo_startingequipment->TabIndex = 19;
            this->combo_startingequipment->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::combo_startingequipment_changed);
            // 
            // label_startingequipment
            // 
            this->label_startingequipment->AutoSize = true;
            this->label_startingequipment->Location = System::Drawing::Point(9, 64);
            this->label_startingequipment->Name = L"label_startingequipment";
            this->label_startingequipment->Size = System::Drawing::Size(98, 13);
            this->label_startingequipment->TabIndex = 20;
            this->label_startingequipment->Text = L"Starting equipment:";
            // 
            // combo_weaponset
            // 
            this->combo_weaponset->DropDownHeight = 200;
            this->combo_weaponset->FormattingEnabled = true;
            this->combo_weaponset->IntegralHeight = false;
            this->combo_weaponset->Items->AddRange(gcnew cli::array< System::Object^  >(13) {
                L"Normal", L"Pistols", L"Rifles", L"Plasma",
                    L"Snipers", L"No Sniping", L"Rockets", L"Shotguns", L"Short Range", L"Human", L"Covenant", L"Classic", L"Heavy"
            });
            this->combo_weaponset->Location = System::Drawing::Point(12, 35);
            this->combo_weaponset->Name = L"combo_weaponset";
            this->combo_weaponset->Size = System::Drawing::Size(110, 21);
            this->combo_weaponset->TabIndex = 17;
            this->combo_weaponset->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::combo_weaponset_changed);
            // 
            // label_weaponset
            // 
            this->label_weaponset->AutoSize = true;
            this->label_weaponset->Location = System::Drawing::Point(9, 19);
            this->label_weaponset->Name = L"label_weaponset";
            this->label_weaponset->Size = System::Drawing::Size(68, 13);
            this->label_weaponset->TabIndex = 18;
            this->label_weaponset->Text = L"Weapon set:";
            // 
            // check_infgrenades
            // 
            this->check_infgrenades->AutoSize = true;
            this->check_infgrenades->Location = System::Drawing::Point(12, 115);
            this->check_infgrenades->Name = L"check_infgrenades";
            this->check_infgrenades->Size = System::Drawing::Size(104, 17);
            this->check_infgrenades->TabIndex = 15;
            this->check_infgrenades->Text = L"Infinite grenades";
            this->check_infgrenades->UseVisualStyleBackColor = true;
            this->check_infgrenades->CheckedChanged += gcnew System::EventHandler(this, &MainForm::check_infgrenades_change);
            // 
            // group_game
            // 
            this->group_game->Controls->Add(this->combo_scorelimit);
            this->group_game->Controls->Add(this->combo_timelimit);
            this->group_game->Controls->Add(this->combo_gtmod5);
            this->group_game->Controls->Add(this->label_gtmod5);
            this->group_game->Controls->Add(this->check_gtmod4);
            this->group_game->Controls->Add(this->check_gtmod3);
            this->group_game->Controls->Add(this->check_gtmod2);
            this->group_game->Controls->Add(this->check_gtmod1);
            this->group_game->Controls->Add(this->combo_gamemode);
            this->group_game->Controls->Add(this->label_mode);
            this->group_game->Controls->Add(this->label_timelimit);
            this->group_game->Controls->Add(this->label_scorelimit);
            this->group_game->Controls->Add(this->check_teamplay);
            this->group_game->Location = System::Drawing::Point(12, 79);
            this->group_game->Name = L"group_game";
            this->group_game->Size = System::Drawing::Size(263, 183);
            this->group_game->TabIndex = 15;
            this->group_game->TabStop = false;
            this->group_game->Text = L"Game options";
            // 
            // combo_scorelimit
            // 
            this->combo_scorelimit->DropDownHeight = 100;
            this->combo_scorelimit->FormattingEnabled = true;
            this->combo_scorelimit->IntegralHeight = false;
            this->combo_scorelimit->Location = System::Drawing::Point(12, 80);
            this->combo_scorelimit->Name = L"combo_scorelimit";
            this->combo_scorelimit->Size = System::Drawing::Size(110, 21);
            this->combo_scorelimit->TabIndex = 20;
            this->combo_scorelimit->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::combo_scorelimit_changed);
            // 
            // combo_timelimit
            // 
            this->combo_timelimit->DropDownHeight = 100;
            this->combo_timelimit->FormattingEnabled = true;
            this->combo_timelimit->IntegralHeight = false;
            this->combo_timelimit->Items->AddRange(gcnew cli::array< System::Object^  >(7) {
                L"None", L"10 Minutes", L"15 Minutes", L"20 Minutes",
                    L"25 Minutes", L"30 Minutes", L"45 Minutes"
            });
            this->combo_timelimit->Location = System::Drawing::Point(12, 125);
            this->combo_timelimit->Name = L"combo_timelimit";
            this->combo_timelimit->Size = System::Drawing::Size(110, 21);
            this->combo_timelimit->TabIndex = 19;
            this->combo_timelimit->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::combo_timelimit_changed);
            // 
            // combo_gtmod5
            // 
            this->combo_gtmod5->DropDownHeight = 100;
            this->combo_gtmod5->Enabled = false;
            this->combo_gtmod5->FormattingEnabled = true;
            this->combo_gtmod5->IntegralHeight = false;
            this->combo_gtmod5->Location = System::Drawing::Point(137, 134);
            this->combo_gtmod5->Name = L"combo_gtmod5";
            this->combo_gtmod5->Size = System::Drawing::Size(110, 21);
            this->combo_gtmod5->TabIndex = 18;
            this->combo_gtmod5->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::combo_gtmod5_changed);
            // 
            // label_gtmod5
            // 
            this->label_gtmod5->AutoSize = true;
            this->label_gtmod5->Enabled = false;
            this->label_gtmod5->Location = System::Drawing::Point(134, 118);
            this->label_gtmod5->Name = L"label_gtmod5";
            this->label_gtmod5->Size = System::Drawing::Size(56, 13);
            this->label_gtmod5->TabIndex = 17;
            this->label_gtmod5->Text = L"Modifier 5:";
            // 
            // check_gtmod4
            // 
            this->check_gtmod4->AutoSize = true;
            this->check_gtmod4->BackColor = System::Drawing::Color::Transparent;
            this->check_gtmod4->Enabled = false;
            this->check_gtmod4->Location = System::Drawing::Point(137, 93);
            this->check_gtmod4->Name = L"check_gtmod4";
            this->check_gtmod4->Size = System::Drawing::Size(72, 17);
            this->check_gtmod4->TabIndex = 16;
            this->check_gtmod4->Text = L"Modifier 4";
            this->check_gtmod4->UseVisualStyleBackColor = false;
            this->check_gtmod4->CheckedChanged += gcnew System::EventHandler(this, &MainForm::check_gtmod4_changed);
            // 
            // check_gtmod3
            // 
            this->check_gtmod3->AutoSize = true;
            this->check_gtmod3->BackColor = System::Drawing::Color::Transparent;
            this->check_gtmod3->Enabled = false;
            this->check_gtmod3->Location = System::Drawing::Point(137, 70);
            this->check_gtmod3->Name = L"check_gtmod3";
            this->check_gtmod3->Size = System::Drawing::Size(72, 17);
            this->check_gtmod3->TabIndex = 15;
            this->check_gtmod3->Text = L"Modifier 3";
            this->check_gtmod3->UseVisualStyleBackColor = false;
            this->check_gtmod3->CheckedChanged += gcnew System::EventHandler(this, &MainForm::check_gtmod3_changed);
            // 
            // check_gtmod2
            // 
            this->check_gtmod2->AutoSize = true;
            this->check_gtmod2->BackColor = System::Drawing::Color::Transparent;
            this->check_gtmod2->Enabled = false;
            this->check_gtmod2->Location = System::Drawing::Point(137, 47);
            this->check_gtmod2->Name = L"check_gtmod2";
            this->check_gtmod2->Size = System::Drawing::Size(72, 17);
            this->check_gtmod2->TabIndex = 14;
            this->check_gtmod2->Text = L"Modifier 2";
            this->check_gtmod2->UseVisualStyleBackColor = false;
            this->check_gtmod2->CheckedChanged += gcnew System::EventHandler(this, &MainForm::check_gtmod2_changed);
            // 
            // check_gtmod1
            // 
            this->check_gtmod1->AutoSize = true;
            this->check_gtmod1->BackColor = System::Drawing::Color::Transparent;
            this->check_gtmod1->Enabled = false;
            this->check_gtmod1->Location = System::Drawing::Point(137, 24);
            this->check_gtmod1->Name = L"check_gtmod1";
            this->check_gtmod1->Size = System::Drawing::Size(72, 17);
            this->check_gtmod1->TabIndex = 13;
            this->check_gtmod1->Text = L"Modifier 1";
            this->check_gtmod1->UseVisualStyleBackColor = false;
            this->check_gtmod1->CheckedChanged += gcnew System::EventHandler(this, &MainForm::check_gtmod1_changed);
            // 
            // group_indicator
            // 
            this->group_indicator->Controls->Add(this->check_friendindicators);
            this->group_indicator->Controls->Add(this->combo_radar);
            this->group_indicator->Controls->Add(this->label_radar);
            this->group_indicator->Controls->Add(this->combo_objectives);
            this->group_indicator->Controls->Add(this->label_objectives);
            this->group_indicator->Location = System::Drawing::Point(379, 470);
            this->group_indicator->Name = L"group_indicator";
            this->group_indicator->Size = System::Drawing::Size(170, 137);
            this->group_indicator->TabIndex = 16;
            this->group_indicator->TabStop = false;
            this->group_indicator->Text = L"Indicator options";
            // 
            // check_friendindicators
            // 
            this->check_friendindicators->AutoSize = true;
            this->check_friendindicators->Location = System::Drawing::Point(12, 111);
            this->check_friendindicators->Name = L"check_friendindicators";
            this->check_friendindicators->Size = System::Drawing::Size(153, 17);
            this->check_friendindicators->TabIndex = 16;
            this->check_friendindicators->Text = L"Friend indicators on screen";
            this->check_friendindicators->UseVisualStyleBackColor = true;
            this->check_friendindicators->CheckedChanged += gcnew System::EventHandler(this, &MainForm::check_friendindicators_changed);
            // 
            // combo_radar
            // 
            this->combo_radar->DropDownHeight = 100;
            this->combo_radar->FormattingEnabled = true;
            this->combo_radar->IntegralHeight = false;
            this->combo_radar->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"All", L"Friends", L"No" });
            this->combo_radar->Location = System::Drawing::Point(12, 80);
            this->combo_radar->Name = L"combo_radar";
            this->combo_radar->Size = System::Drawing::Size(110, 21);
            this->combo_radar->TabIndex = 8;
            this->combo_radar->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::combo_radar_changed);
            // 
            // label_radar
            // 
            this->label_radar->AutoSize = true;
            this->label_radar->Location = System::Drawing::Point(9, 64);
            this->label_radar->Name = L"label_radar";
            this->label_radar->Size = System::Drawing::Size(157, 13);
            this->label_radar->TabIndex = 7;
            this->label_radar->Text = L"Other players on motion tracker:";
            // 
            // combo_objectives
            // 
            this->combo_objectives->DropDownHeight = 100;
            this->combo_objectives->FormattingEnabled = true;
            this->combo_objectives->IntegralHeight = false;
            this->combo_objectives->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"Motion Tracker", L"Nav Points", L"None" });
            this->combo_objectives->Location = System::Drawing::Point(12, 35);
            this->combo_objectives->Name = L"combo_objectives";
            this->combo_objectives->Size = System::Drawing::Size(110, 21);
            this->combo_objectives->TabIndex = 6;
            this->combo_objectives->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::combo_objectives_changed);
            // 
            // label_objectives
            // 
            this->label_objectives->AutoSize = true;
            this->label_objectives->Location = System::Drawing::Point(9, 19);
            this->label_objectives->Name = L"label_objectives";
            this->label_objectives->Size = System::Drawing::Size(103, 13);
            this->label_objectives->TabIndex = 5;
            this->label_objectives->Text = L"Objectives indicator:";
            // 
            // group_teamplay
            // 
            this->group_teamplay->Controls->Add(this->combo_friendlyfire);
            this->group_teamplay->Controls->Add(this->label2);
            this->group_teamplay->Controls->Add(this->combo_friendlyfirepenalty);
            this->group_teamplay->Controls->Add(this->label_friendlyfirepenalty);
            this->group_teamplay->Controls->Add(this->check_teambalance);
            this->group_teamplay->Location = System::Drawing::Point(564, 470);
            this->group_teamplay->Name = L"group_teamplay";
            this->group_teamplay->Size = System::Drawing::Size(134, 137);
            this->group_teamplay->TabIndex = 17;
            this->group_teamplay->TabStop = false;
            this->group_teamplay->Text = L"Teamplay options";
            // 
            // combo_friendlyfire
            // 
            this->combo_friendlyfire->DropDownHeight = 100;
            this->combo_friendlyfire->FormattingEnabled = true;
            this->combo_friendlyfire->IntegralHeight = false;
            this->combo_friendlyfire->Items->AddRange(gcnew cli::array< System::Object^  >(4) { L"Off", L"On", L"Shields Only", L"Explosives Only" });
            this->combo_friendlyfire->Location = System::Drawing::Point(12, 58);
            this->combo_friendlyfire->Name = L"combo_friendlyfire";
            this->combo_friendlyfire->Size = System::Drawing::Size(110, 21);
            this->combo_friendlyfire->TabIndex = 22;
            this->combo_friendlyfire->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::combo_friendlyfire_changed);
            // 
            // label2
            // 
            this->label2->AutoSize = true;
            this->label2->Location = System::Drawing::Point(9, 42);
            this->label2->Name = L"label2";
            this->label2->Size = System::Drawing::Size(63, 13);
            this->label2->TabIndex = 21;
            this->label2->Text = L"Friendly fire:";
            // 
            // combo_friendlyfirepenalty
            // 
            this->combo_friendlyfirepenalty->DropDownHeight = 100;
            this->combo_friendlyfirepenalty->FormattingEnabled = true;
            this->combo_friendlyfirepenalty->IntegralHeight = false;
            this->combo_friendlyfirepenalty->Items->AddRange(gcnew cli::array< System::Object^  >(4) {
                L"None", L"5 Seconds", L"10 Seconds",
                    L"15 Seconds"
            });
            this->combo_friendlyfirepenalty->Location = System::Drawing::Point(12, 103);
            this->combo_friendlyfirepenalty->Name = L"combo_friendlyfirepenalty";
            this->combo_friendlyfirepenalty->Size = System::Drawing::Size(110, 21);
            this->combo_friendlyfirepenalty->TabIndex = 20;
            this->combo_friendlyfirepenalty->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::combo_friendlyfirepenalty_changed);
            // 
            // label_friendlyfirepenalty
            // 
            this->label_friendlyfirepenalty->AutoSize = true;
            this->label_friendlyfirepenalty->Location = System::Drawing::Point(9, 87);
            this->label_friendlyfirepenalty->Name = L"label_friendlyfirepenalty";
            this->label_friendlyfirepenalty->Size = System::Drawing::Size(100, 13);
            this->label_friendlyfirepenalty->TabIndex = 19;
            this->label_friendlyfirepenalty->Text = L"Friendly fire penalty:";
            // 
            // check_teambalance
            // 
            this->check_teambalance->AutoSize = true;
            this->check_teambalance->Location = System::Drawing::Point(12, 20);
            this->check_teambalance->Name = L"check_teambalance";
            this->check_teambalance->Size = System::Drawing::Size(115, 17);
            this->check_teambalance->TabIndex = 17;
            this->check_teambalance->Text = L"Auto team balance";
            this->check_teambalance->UseVisualStyleBackColor = true;
            this->check_teambalance->CheckedChanged += gcnew System::EventHandler(this, &MainForm::check_autoteambalance_changed);
            // 
            // tab_main
            // 
            this->tab_main->Controls->Add(this->Normal);
            this->tab_main->Controls->Add(this->Advanced);
            this->tab_main->Controls->Add(this->Settings);
            this->tab_main->ImeMode = System::Windows::Forms::ImeMode::NoControl;
            this->tab_main->ItemSize = System::Drawing::Size(100, 24);
            this->tab_main->Location = System::Drawing::Point(12, 12);
            this->tab_main->Margin = System::Windows::Forms::Padding(4);
            this->tab_main->Name = L"tab_main";
            this->tab_main->Padding = System::Drawing::Point(0, 0);
            this->tab_main->SelectedIndex = 0;
            this->tab_main->ShowToolTips = true;
            this->tab_main->Size = System::Drawing::Size(720, 651);
            this->tab_main->SizeMode = System::Windows::Forms::TabSizeMode::Fixed;
            this->tab_main->TabIndex = 18;
            this->tab_main->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::tab_main_indexchanged);
            // 
            // Normal
            // 
            this->Normal->BackColor = System::Drawing::Color::White;
            this->Normal->Controls->Add(this->banner);
            this->Normal->Controls->Add(this->groupBox1);
            this->Normal->Controls->Add(this->group_item);
            this->Normal->Controls->Add(this->group_player);
            this->Normal->Controls->Add(this->group_teamplay);
            this->Normal->Controls->Add(this->group_game);
            this->Normal->Controls->Add(this->group_indicator);
            this->Normal->Location = System::Drawing::Point(4, 28);
            this->Normal->Name = L"Normal";
            this->Normal->Padding = System::Windows::Forms::Padding(8);
            this->Normal->Size = System::Drawing::Size(712, 619);
            this->Normal->TabIndex = 0;
            this->Normal->Text = L"Normal";
            this->Normal->ToolTipText = L"Edit gametypes with an interface that mimics the original game";
            // 
            // banner
            // 
            this->banner->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"banner.BackgroundImage")));
            this->banner->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"banner.Image")));
            this->banner->Location = System::Drawing::Point(12, 476);
            this->banner->Name = L"banner";
            this->banner->Size = System::Drawing::Size(352, 130);
            this->banner->SizeMode = System::Windows::Forms::PictureBoxSizeMode::CenterImage;
            this->banner->TabIndex = 19;
            this->banner->TabStop = false;
            // 
            // groupBox1
            // 
            this->groupBox1->Controls->Add(this->group_blueteam);
            this->groupBox1->Controls->Add(this->group_redteam);
            this->groupBox1->Controls->Add(this->combo_vehiclerespawntime);
            this->groupBox1->Controls->Add(this->label_vehiclerespawntime);
            this->groupBox1->Location = System::Drawing::Point(12, 270);
            this->groupBox1->Name = L"groupBox1";
            this->groupBox1->Size = System::Drawing::Size(686, 192);
            this->groupBox1->TabIndex = 18;
            this->groupBox1->TabStop = false;
            this->groupBox1->Text = L"Vehicle options";
            // 
            // group_blueteam
            // 
            this->group_blueteam->Controls->Add(this->num_blueteam_gunturret);
            this->group_blueteam->Controls->Add(this->label_blueteam_gunturret);
            this->group_blueteam->Controls->Add(this->num_blueteam_banshee);
            this->group_blueteam->Controls->Add(this->label_blueteam_banshee);
            this->group_blueteam->Controls->Add(this->num_blueteam_rockethog);
            this->group_blueteam->Controls->Add(this->label_blueteam_rockethog);
            this->group_blueteam->Controls->Add(this->num_blueteam_scorpion);
            this->group_blueteam->Controls->Add(this->label_blueteam_scorpion);
            this->group_blueteam->Controls->Add(this->num_blueteam_ghost);
            this->group_blueteam->Controls->Add(this->label_blueteam_ghost);
            this->group_blueteam->Controls->Add(this->label_blueteam_vehicleset);
            this->group_blueteam->Controls->Add(this->num_blueteam_warthog);
            this->group_blueteam->Controls->Add(this->label_blueteam_warthog);
            this->group_blueteam->Controls->Add(this->combo_blueteam_vehicleset);
            this->group_blueteam->Location = System::Drawing::Point(409, 16);
            this->group_blueteam->Name = L"group_blueteam";
            this->group_blueteam->Size = System::Drawing::Size(261, 160);
            this->group_blueteam->TabIndex = 21;
            this->group_blueteam->TabStop = false;
            this->group_blueteam->Text = L"Blue team";
            // 
            // num_blueteam_gunturret
            // 
            this->num_blueteam_gunturret->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->num_blueteam_gunturret->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular,
                System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
            this->num_blueteam_gunturret->Location = System::Drawing::Point(198, 128);
            this->num_blueteam_gunturret->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 4, 0, 0, 0 });
            this->num_blueteam_gunturret->Name = L"num_blueteam_gunturret";
            this->num_blueteam_gunturret->Size = System::Drawing::Size(42, 20);
            this->num_blueteam_gunturret->TabIndex = 19;
            this->num_blueteam_gunturret->ValueChanged += gcnew System::EventHandler(this, &MainForm::num_blueteam_gunturret_changed);
            // 
            // label_blueteam_gunturret
            // 
            this->label_blueteam_gunturret->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
            this->label_blueteam_gunturret->Location = System::Drawing::Point(128, 130);
            this->label_blueteam_gunturret->Name = L"label_blueteam_gunturret";
            this->label_blueteam_gunturret->Size = System::Drawing::Size(70, 13);
            this->label_blueteam_gunturret->TabIndex = 20;
            this->label_blueteam_gunturret->Text = L"Gun Turret:";
            this->label_blueteam_gunturret->TextAlign = System::Drawing::ContentAlignment::TopRight;
            // 
            // num_blueteam_banshee
            // 
            this->num_blueteam_banshee->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->num_blueteam_banshee->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular,
                System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
            this->num_blueteam_banshee->Location = System::Drawing::Point(198, 98);
            this->num_blueteam_banshee->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 4, 0, 0, 0 });
            this->num_blueteam_banshee->Name = L"num_blueteam_banshee";
            this->num_blueteam_banshee->Size = System::Drawing::Size(42, 20);
            this->num_blueteam_banshee->TabIndex = 17;
            this->num_blueteam_banshee->ValueChanged += gcnew System::EventHandler(this, &MainForm::num_blueteam_banshee_changed);
            // 
            // label_blueteam_banshee
            // 
            this->label_blueteam_banshee->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
            this->label_blueteam_banshee->Location = System::Drawing::Point(128, 100);
            this->label_blueteam_banshee->Name = L"label_blueteam_banshee";
            this->label_blueteam_banshee->Size = System::Drawing::Size(70, 13);
            this->label_blueteam_banshee->TabIndex = 18;
            this->label_blueteam_banshee->Text = L"Banshee:";
            this->label_blueteam_banshee->TextAlign = System::Drawing::ContentAlignment::TopRight;
            // 
            // num_blueteam_rockethog
            // 
            this->num_blueteam_rockethog->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->num_blueteam_rockethog->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular,
                System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
            this->num_blueteam_rockethog->Location = System::Drawing::Point(198, 68);
            this->num_blueteam_rockethog->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 4, 0, 0, 0 });
            this->num_blueteam_rockethog->Name = L"num_blueteam_rockethog";
            this->num_blueteam_rockethog->Size = System::Drawing::Size(42, 20);
            this->num_blueteam_rockethog->TabIndex = 15;
            this->num_blueteam_rockethog->ValueChanged += gcnew System::EventHandler(this, &MainForm::num_blueteam_rockethog_changed);
            // 
            // label_blueteam_rockethog
            // 
            this->label_blueteam_rockethog->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
            this->label_blueteam_rockethog->Location = System::Drawing::Point(128, 70);
            this->label_blueteam_rockethog->Name = L"label_blueteam_rockethog";
            this->label_blueteam_rockethog->Size = System::Drawing::Size(70, 13);
            this->label_blueteam_rockethog->TabIndex = 16;
            this->label_blueteam_rockethog->Text = L"Rocket Hog:";
            this->label_blueteam_rockethog->TextAlign = System::Drawing::ContentAlignment::TopRight;
            // 
            // num_blueteam_scorpion
            // 
            this->num_blueteam_scorpion->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->num_blueteam_scorpion->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular,
                System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
            this->num_blueteam_scorpion->Location = System::Drawing::Point(77, 126);
            this->num_blueteam_scorpion->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 4, 0, 0, 0 });
            this->num_blueteam_scorpion->Name = L"num_blueteam_scorpion";
            this->num_blueteam_scorpion->Size = System::Drawing::Size(42, 20);
            this->num_blueteam_scorpion->TabIndex = 13;
            this->num_blueteam_scorpion->ValueChanged += gcnew System::EventHandler(this, &MainForm::num_blueteam_scorpion_changed);
            // 
            // label_blueteam_scorpion
            // 
            this->label_blueteam_scorpion->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
            this->label_blueteam_scorpion->Location = System::Drawing::Point(7, 128);
            this->label_blueteam_scorpion->Name = L"label_blueteam_scorpion";
            this->label_blueteam_scorpion->Size = System::Drawing::Size(70, 13);
            this->label_blueteam_scorpion->TabIndex = 14;
            this->label_blueteam_scorpion->Text = L"Scorpion:";
            this->label_blueteam_scorpion->TextAlign = System::Drawing::ContentAlignment::TopRight;
            // 
            // num_blueteam_ghost
            // 
            this->num_blueteam_ghost->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->num_blueteam_ghost->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular,
                System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
            this->num_blueteam_ghost->Location = System::Drawing::Point(77, 96);
            this->num_blueteam_ghost->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 4, 0, 0, 0 });
            this->num_blueteam_ghost->Name = L"num_blueteam_ghost";
            this->num_blueteam_ghost->Size = System::Drawing::Size(42, 20);
            this->num_blueteam_ghost->TabIndex = 11;
            this->num_blueteam_ghost->ValueChanged += gcnew System::EventHandler(this, &MainForm::num_blueteam_ghost_changed);
            // 
            // label_blueteam_ghost
            // 
            this->label_blueteam_ghost->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
            this->label_blueteam_ghost->Location = System::Drawing::Point(7, 98);
            this->label_blueteam_ghost->Name = L"label_blueteam_ghost";
            this->label_blueteam_ghost->Size = System::Drawing::Size(70, 13);
            this->label_blueteam_ghost->TabIndex = 12;
            this->label_blueteam_ghost->Text = L"Ghost:";
            this->label_blueteam_ghost->TextAlign = System::Drawing::ContentAlignment::TopRight;
            // 
            // label_blueteam_vehicleset
            // 
            this->label_blueteam_vehicleset->AutoSize = true;
            this->label_blueteam_vehicleset->Location = System::Drawing::Point(9, 19);
            this->label_blueteam_vehicleset->Name = L"label_blueteam_vehicleset";
            this->label_blueteam_vehicleset->Size = System::Drawing::Size(62, 13);
            this->label_blueteam_vehicleset->TabIndex = 8;
            this->label_blueteam_vehicleset->Text = L"Vehicle set:";
            // 
            // num_blueteam_warthog
            // 
            this->num_blueteam_warthog->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->num_blueteam_warthog->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular,
                System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
            this->num_blueteam_warthog->Location = System::Drawing::Point(77, 66);
            this->num_blueteam_warthog->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 4, 0, 0, 0 });
            this->num_blueteam_warthog->Name = L"num_blueteam_warthog";
            this->num_blueteam_warthog->Size = System::Drawing::Size(42, 20);
            this->num_blueteam_warthog->TabIndex = 7;
            this->num_blueteam_warthog->ValueChanged += gcnew System::EventHandler(this, &MainForm::num_blueteam_warthog_changed);
            // 
            // label_blueteam_warthog
            // 
            this->label_blueteam_warthog->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
            this->label_blueteam_warthog->Location = System::Drawing::Point(7, 68);
            this->label_blueteam_warthog->Name = L"label_blueteam_warthog";
            this->label_blueteam_warthog->Size = System::Drawing::Size(70, 13);
            this->label_blueteam_warthog->TabIndex = 10;
            this->label_blueteam_warthog->Text = L"Warthog:";
            this->label_blueteam_warthog->TextAlign = System::Drawing::ContentAlignment::TopRight;
            // 
            // combo_blueteam_vehicleset
            // 
            this->combo_blueteam_vehicleset->AutoCompleteCustomSource->AddRange(gcnew cli::array< System::String^  >(7) {
                L"Never", L"30 Seconds",
                    L"60 Seconds", L"90 Seconds", L"2 Minutes", L"3 Minutes", L"5 Minutes"
            });
            this->combo_blueteam_vehicleset->DropDownHeight = 150;
            this->combo_blueteam_vehicleset->FormattingEnabled = true;
            this->combo_blueteam_vehicleset->IntegralHeight = false;
            this->combo_blueteam_vehicleset->Items->AddRange(gcnew cli::array< System::Object^  >(9) {
                L"Default", L"None", L"Warthogs",
                    L"Ghosts", L"Scorpions", L"Rocket Warthogs", L"Banshees", L"Gun Turrets", L"Custom"
            });
            this->combo_blueteam_vehicleset->Location = System::Drawing::Point(12, 35);
            this->combo_blueteam_vehicleset->Name = L"combo_blueteam_vehicleset";
            this->combo_blueteam_vehicleset->Size = System::Drawing::Size(110, 21);
            this->combo_blueteam_vehicleset->TabIndex = 9;
            this->combo_blueteam_vehicleset->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::combo_blueteam_vehicleset_changed);
            // 
            // group_redteam
            // 
            this->group_redteam->Controls->Add(this->num_redteam_gunturret);
            this->group_redteam->Controls->Add(this->label_redteam_gunturret);
            this->group_redteam->Controls->Add(this->num_redteam_banshee);
            this->group_redteam->Controls->Add(this->label_redteam_banshee);
            this->group_redteam->Controls->Add(this->num_redteam_rockethog);
            this->group_redteam->Controls->Add(this->label_redteam_rockethog);
            this->group_redteam->Controls->Add(this->num_redteam_scorpion);
            this->group_redteam->Controls->Add(this->label_redteam_scorpion);
            this->group_redteam->Controls->Add(this->num_redteam_ghost);
            this->group_redteam->Controls->Add(this->label_redteam_ghost);
            this->group_redteam->Controls->Add(this->label_redteam_vehicleset);
            this->group_redteam->Controls->Add(this->num_redteam_warthog);
            this->group_redteam->Controls->Add(this->label_redteam_warthog);
            this->group_redteam->Controls->Add(this->combo_redteam_vehicleset);
            this->group_redteam->Location = System::Drawing::Point(133, 16);
            this->group_redteam->Name = L"group_redteam";
            this->group_redteam->Size = System::Drawing::Size(261, 160);
            this->group_redteam->TabIndex = 11;
            this->group_redteam->TabStop = false;
            this->group_redteam->Text = L"Red team";
            // 
            // num_redteam_gunturret
            // 
            this->num_redteam_gunturret->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->num_redteam_gunturret->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular,
                System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
            this->num_redteam_gunturret->Location = System::Drawing::Point(198, 129);
            this->num_redteam_gunturret->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 4, 0, 0, 0 });
            this->num_redteam_gunturret->Name = L"num_redteam_gunturret";
            this->num_redteam_gunturret->Size = System::Drawing::Size(42, 20);
            this->num_redteam_gunturret->TabIndex = 19;
            this->num_redteam_gunturret->ValueChanged += gcnew System::EventHandler(this, &MainForm::num_redteam_gunturret_changed);
            // 
            // label_redteam_gunturret
            // 
            this->label_redteam_gunturret->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
            this->label_redteam_gunturret->Location = System::Drawing::Point(128, 131);
            this->label_redteam_gunturret->Name = L"label_redteam_gunturret";
            this->label_redteam_gunturret->Size = System::Drawing::Size(70, 13);
            this->label_redteam_gunturret->TabIndex = 20;
            this->label_redteam_gunturret->Text = L"Gun Turret:";
            this->label_redteam_gunturret->TextAlign = System::Drawing::ContentAlignment::TopRight;
            // 
            // num_redteam_banshee
            // 
            this->num_redteam_banshee->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->num_redteam_banshee->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular,
                System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
            this->num_redteam_banshee->Location = System::Drawing::Point(198, 99);
            this->num_redteam_banshee->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 4, 0, 0, 0 });
            this->num_redteam_banshee->Name = L"num_redteam_banshee";
            this->num_redteam_banshee->Size = System::Drawing::Size(42, 20);
            this->num_redteam_banshee->TabIndex = 17;
            this->num_redteam_banshee->ValueChanged += gcnew System::EventHandler(this, &MainForm::num_redteam_banshee_changed);
            // 
            // label_redteam_banshee
            // 
            this->label_redteam_banshee->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
            this->label_redteam_banshee->Location = System::Drawing::Point(128, 101);
            this->label_redteam_banshee->Name = L"label_redteam_banshee";
            this->label_redteam_banshee->Size = System::Drawing::Size(70, 13);
            this->label_redteam_banshee->TabIndex = 18;
            this->label_redteam_banshee->Text = L"Banshee:";
            this->label_redteam_banshee->TextAlign = System::Drawing::ContentAlignment::TopRight;
            // 
            // num_redteam_rockethog
            // 
            this->num_redteam_rockethog->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->num_redteam_rockethog->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular,
                System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
            this->num_redteam_rockethog->Location = System::Drawing::Point(198, 69);
            this->num_redteam_rockethog->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 4, 0, 0, 0 });
            this->num_redteam_rockethog->Name = L"num_redteam_rockethog";
            this->num_redteam_rockethog->Size = System::Drawing::Size(42, 20);
            this->num_redteam_rockethog->TabIndex = 15;
            this->num_redteam_rockethog->ValueChanged += gcnew System::EventHandler(this, &MainForm::num_redteam_rockethog_changed);
            // 
            // label_redteam_rockethog
            // 
            this->label_redteam_rockethog->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
            this->label_redteam_rockethog->Location = System::Drawing::Point(128, 71);
            this->label_redteam_rockethog->Name = L"label_redteam_rockethog";
            this->label_redteam_rockethog->Size = System::Drawing::Size(70, 13);
            this->label_redteam_rockethog->TabIndex = 16;
            this->label_redteam_rockethog->Text = L"Rocket Hog:";
            this->label_redteam_rockethog->TextAlign = System::Drawing::ContentAlignment::TopRight;
            // 
            // num_redteam_scorpion
            // 
            this->num_redteam_scorpion->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->num_redteam_scorpion->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular,
                System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
            this->num_redteam_scorpion->Location = System::Drawing::Point(77, 127);
            this->num_redteam_scorpion->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 4, 0, 0, 0 });
            this->num_redteam_scorpion->Name = L"num_redteam_scorpion";
            this->num_redteam_scorpion->Size = System::Drawing::Size(42, 20);
            this->num_redteam_scorpion->TabIndex = 13;
            this->num_redteam_scorpion->ValueChanged += gcnew System::EventHandler(this, &MainForm::num_redteam_scorpion_changed);
            // 
            // label_redteam_scorpion
            // 
            this->label_redteam_scorpion->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
            this->label_redteam_scorpion->Location = System::Drawing::Point(7, 129);
            this->label_redteam_scorpion->Name = L"label_redteam_scorpion";
            this->label_redteam_scorpion->Size = System::Drawing::Size(70, 13);
            this->label_redteam_scorpion->TabIndex = 14;
            this->label_redteam_scorpion->Text = L"Scorpion:";
            this->label_redteam_scorpion->TextAlign = System::Drawing::ContentAlignment::TopRight;
            // 
            // num_redteam_ghost
            // 
            this->num_redteam_ghost->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->num_redteam_ghost->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular,
                System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
            this->num_redteam_ghost->Location = System::Drawing::Point(77, 97);
            this->num_redteam_ghost->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 4, 0, 0, 0 });
            this->num_redteam_ghost->Name = L"num_redteam_ghost";
            this->num_redteam_ghost->Size = System::Drawing::Size(42, 20);
            this->num_redteam_ghost->TabIndex = 11;
            this->num_redteam_ghost->ValueChanged += gcnew System::EventHandler(this, &MainForm::num_redteam_ghost_changed);
            // 
            // label_redteam_ghost
            // 
            this->label_redteam_ghost->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
            this->label_redteam_ghost->Location = System::Drawing::Point(7, 99);
            this->label_redteam_ghost->Name = L"label_redteam_ghost";
            this->label_redteam_ghost->Size = System::Drawing::Size(70, 13);
            this->label_redteam_ghost->TabIndex = 12;
            this->label_redteam_ghost->Text = L"Ghost:";
            this->label_redteam_ghost->TextAlign = System::Drawing::ContentAlignment::TopRight;
            // 
            // label_redteam_vehicleset
            // 
            this->label_redteam_vehicleset->AutoSize = true;
            this->label_redteam_vehicleset->Location = System::Drawing::Point(9, 19);
            this->label_redteam_vehicleset->Name = L"label_redteam_vehicleset";
            this->label_redteam_vehicleset->Size = System::Drawing::Size(62, 13);
            this->label_redteam_vehicleset->TabIndex = 8;
            this->label_redteam_vehicleset->Text = L"Vehicle set:";
            // 
            // num_redteam_warthog
            // 
            this->num_redteam_warthog->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->num_redteam_warthog->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular,
                System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
            this->num_redteam_warthog->Location = System::Drawing::Point(77, 67);
            this->num_redteam_warthog->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 4, 0, 0, 0 });
            this->num_redteam_warthog->Name = L"num_redteam_warthog";
            this->num_redteam_warthog->Size = System::Drawing::Size(42, 20);
            this->num_redteam_warthog->TabIndex = 7;
            this->num_redteam_warthog->ValueChanged += gcnew System::EventHandler(this, &MainForm::num_redteam_warthog_changed);
            // 
            // label_redteam_warthog
            // 
            this->label_redteam_warthog->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
            this->label_redteam_warthog->Location = System::Drawing::Point(7, 69);
            this->label_redteam_warthog->Name = L"label_redteam_warthog";
            this->label_redteam_warthog->Size = System::Drawing::Size(70, 13);
            this->label_redteam_warthog->TabIndex = 10;
            this->label_redteam_warthog->Text = L"Warthog:";
            this->label_redteam_warthog->TextAlign = System::Drawing::ContentAlignment::TopRight;
            // 
            // combo_redteam_vehicleset
            // 
            this->combo_redteam_vehicleset->AutoCompleteCustomSource->AddRange(gcnew cli::array< System::String^  >(7) {
                L"Never", L"30 Seconds",
                    L"60 Seconds", L"90 Seconds", L"2 Minutes", L"3 Minutes", L"5 Minutes"
            });
            this->combo_redteam_vehicleset->DropDownHeight = 150;
            this->combo_redteam_vehicleset->FormattingEnabled = true;
            this->combo_redteam_vehicleset->IntegralHeight = false;
            this->combo_redteam_vehicleset->Items->AddRange(gcnew cli::array< System::Object^  >(9) {
                L"Default", L"None", L"Warthogs",
                    L"Ghosts", L"Scorpions", L"Rocket Warthogs", L"Banshees", L"Gun Turrets", L"Custom"
            });
            this->combo_redteam_vehicleset->Location = System::Drawing::Point(12, 35);
            this->combo_redteam_vehicleset->Name = L"combo_redteam_vehicleset";
            this->combo_redteam_vehicleset->Size = System::Drawing::Size(110, 21);
            this->combo_redteam_vehicleset->TabIndex = 9;
            this->combo_redteam_vehicleset->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::combo_redteam_vehicleset_changed);
            // 
            // combo_vehiclerespawntime
            // 
            this->combo_vehiclerespawntime->AutoCompleteCustomSource->AddRange(gcnew cli::array< System::String^  >(7) {
                L"Never", L"30 Seconds",
                    L"60 Seconds", L"90 Seconds", L"2 Minutes", L"3 Minutes", L"5 Minutes"
            });
            this->combo_vehiclerespawntime->DropDownHeight = 100;
            this->combo_vehiclerespawntime->FormattingEnabled = true;
            this->combo_vehiclerespawntime->IntegralHeight = false;
            this->combo_vehiclerespawntime->Items->AddRange(gcnew cli::array< System::Object^  >(7) {
                L"Never", L"30 Seconds", L"60 Seconds",
                    L"90 Seconds", L"2 Minutes", L"3 Minutes", L"5 Minutes"
            });
            this->combo_vehiclerespawntime->Location = System::Drawing::Point(12, 35);
            this->combo_vehiclerespawntime->Name = L"combo_vehiclerespawntime";
            this->combo_vehiclerespawntime->Size = System::Drawing::Size(110, 21);
            this->combo_vehiclerespawntime->TabIndex = 6;
            this->combo_vehiclerespawntime->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::combo_vehiclerespawntime_changed);
            // 
            // label_vehiclerespawntime
            // 
            this->label_vehiclerespawntime->AutoSize = true;
            this->label_vehiclerespawntime->Location = System::Drawing::Point(9, 19);
            this->label_vehiclerespawntime->Name = L"label_vehiclerespawntime";
            this->label_vehiclerespawntime->Size = System::Drawing::Size(110, 13);
            this->label_vehiclerespawntime->TabIndex = 5;
            this->label_vehiclerespawntime->Text = L"Vehicle respawn time:";
            // 
            // Advanced
            // 
            this->Advanced->BackColor = System::Drawing::Color::White;
            this->Advanced->BackgroundImageLayout = System::Windows::Forms::ImageLayout::None;
            this->Advanced->Controls->Add(this->group_details);
            this->Advanced->Controls->Add(this->list_properties);
            this->Advanced->Location = System::Drawing::Point(4, 28);
            this->Advanced->Name = L"Advanced";
            this->Advanced->Padding = System::Windows::Forms::Padding(3);
            this->Advanced->Size = System::Drawing::Size(712, 619);
            this->Advanced->TabIndex = 1;
            this->Advanced->Text = L"Advanced";
            this->Advanced->ToolTipText = L"Edit gametypes with a custom advanced interface";
            // 
            // group_details
            // 
            this->group_details->BackColor = System::Drawing::Color::Transparent;
            this->group_details->Controls->Add(this->browser_details);
            this->group_details->Location = System::Drawing::Point(12, 423);
            this->group_details->Name = L"group_details";
            this->group_details->Size = System::Drawing::Size(686, 183);
            this->group_details->TabIndex = 2;
            this->group_details->TabStop = false;
            this->group_details->Text = L"Details";
            // 
            // browser_details
            // 
            this->browser_details->AllowWebBrowserDrop = false;
            this->browser_details->Dock = System::Windows::Forms::DockStyle::Fill;
            this->browser_details->IsWebBrowserContextMenuEnabled = false;
            this->browser_details->Location = System::Drawing::Point(3, 16);
            this->browser_details->MinimumSize = System::Drawing::Size(20, 20);
            this->browser_details->Name = L"browser_details";
            this->browser_details->ScriptErrorsSuppressed = true;
            this->browser_details->Size = System::Drawing::Size(680, 164);
            this->browser_details->TabIndex = 0;
            this->browser_details->Url = (gcnew System::Uri(L"", System::UriKind::Relative));
            this->browser_details->WebBrowserShortcutsEnabled = false;
            // 
            // list_properties
            // 
            this->list_properties->AutoArrange = false;
            this->list_properties->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            this->list_properties->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(4) {
                this->prop, this->propertyvalue,
                    this->type, this->description
            });
            this->list_properties->FullRowSelect = true;
            listViewGroup1->Header = L"Game options";
            listViewGroup1->Name = L"gameoptions";
            listViewGroup2->Header = L"Teamplay options";
            listViewGroup2->Name = L"teamplayoptions";
            listViewGroup3->Header = L"Player options";
            listViewGroup3->Name = L"playeroptions";
            listViewGroup4->Header = L"Item options";
            listViewGroup4->Name = L"itemoptions";
            listViewGroup5->Header = L"Vehicle options";
            listViewGroup5->Name = L"vehicleoptions";
            listViewGroup6->Header = L"Indicator options";
            listViewGroup6->Name = L"indicatoroptions";
            this->list_properties->Groups->AddRange(gcnew cli::array< System::Windows::Forms::ListViewGroup^  >(6) {
                listViewGroup1, listViewGroup2,
                    listViewGroup3, listViewGroup4, listViewGroup5, listViewGroup6
            });
            this->list_properties->HeaderStyle = System::Windows::Forms::ColumnHeaderStyle::Nonclickable;
            this->list_properties->HideSelection = false;
            listViewItem1->Group = listViewGroup1;
            listViewItem1->UseItemStyleForSubItems = false;
            listViewItem2->Group = listViewGroup1;
            listViewItem2->StateImageIndex = 0;
            listViewItem3->Group = listViewGroup1;
            listViewItem3->StateImageIndex = 0;
            listViewItem4->Group = listViewGroup1;
            listViewItem4->StateImageIndex = 0;
            listViewItem5->Group = listViewGroup1;
            listViewItem5->StateImageIndex = 0;
            listViewItem6->Group = listViewGroup1;
            listViewItem6->StateImageIndex = 0;
            listViewItem7->Group = listViewGroup1;
            listViewItem7->StateImageIndex = 0;
            listViewItem8->Group = listViewGroup1;
            listViewItem8->StateImageIndex = 0;
            listViewItem9->Group = listViewGroup1;
            listViewItem9->StateImageIndex = 0;
            listViewItem10->Group = listViewGroup2;
            listViewItem10->StateImageIndex = 0;
            listViewItem11->Group = listViewGroup2;
            listViewItem11->StateImageIndex = 0;
            listViewItem12->Group = listViewGroup2;
            listViewItem12->StateImageIndex = 0;
            listViewItem13->Group = listViewGroup2;
            listViewItem13->StateImageIndex = 0;
            listViewItem14->Group = listViewGroup3;
            listViewItem15->Group = listViewGroup3;
            listViewItem16->Group = listViewGroup3;
            listViewItem17->Group = listViewGroup3;
            listViewItem18->Group = listViewGroup3;
            listViewItem19->Group = listViewGroup3;
            listViewItem20->Group = listViewGroup3;
            listViewItem21->Group = listViewGroup3;
            listViewItem22->Group = listViewGroup4;
            listViewItem23->Group = listViewGroup4;
            listViewItem24->Group = listViewGroup4;
            listViewItem25->Group = listViewGroup5;
            listViewItem26->Group = listViewGroup5;
            listViewItem27->Group = listViewGroup5;
            listViewItem28->Group = listViewGroup5;
            listViewItem29->Group = listViewGroup5;
            listViewItem30->Group = listViewGroup5;
            listViewItem31->Group = listViewGroup5;
            listViewItem32->Group = listViewGroup5;
            listViewItem33->Group = listViewGroup5;
            listViewItem34->Group = listViewGroup5;
            listViewItem35->Group = listViewGroup5;
            listViewItem36->Group = listViewGroup5;
            listViewItem37->Group = listViewGroup5;
            listViewItem38->Group = listViewGroup5;
            listViewItem39->Group = listViewGroup5;
            listViewItem40->Group = listViewGroup6;
            listViewItem41->Group = listViewGroup6;
            listViewItem42->Group = listViewGroup6;
            listViewItem43->Group = listViewGroup6;
            this->list_properties->Items->AddRange(gcnew cli::array< System::Windows::Forms::ListViewItem^  >(43) {
                listViewItem1, listViewItem2,
                    listViewItem3, listViewItem4, listViewItem5, listViewItem6, listViewItem7, listViewItem8, listViewItem9, listViewItem10, listViewItem11,
                    listViewItem12, listViewItem13, listViewItem14, listViewItem15, listViewItem16, listViewItem17, listViewItem18, listViewItem19,
                    listViewItem20, listViewItem21, listViewItem22, listViewItem23, listViewItem24, listViewItem25, listViewItem26, listViewItem27,
                    listViewItem28, listViewItem29, listViewItem30, listViewItem31, listViewItem32, listViewItem33, listViewItem34, listViewItem35,
                    listViewItem36, listViewItem37, listViewItem38, listViewItem39, listViewItem40, listViewItem41, listViewItem42, listViewItem43
            });
            this->list_properties->Location = System::Drawing::Point(12, 84);
            this->list_properties->MultiSelect = false;
            this->list_properties->Name = L"list_properties";
            this->list_properties->Size = System::Drawing::Size(686, 330);
            this->list_properties->TabIndex = 0;
            this->list_properties->UseCompatibleStateImageBehavior = false;
            this->list_properties->View = System::Windows::Forms::View::Details;
            this->list_properties->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::list_properties_indexchanged);
            this->list_properties->DoubleClick += gcnew System::EventHandler(this, &MainForm::list_properties_doubleclick);
            this->list_properties->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &MainForm::list_properties_keydown);
            // 
            // prop
            // 
            this->prop->Text = L"Property";
            this->prop->Width = 170;
            // 
            // propertyvalue
            // 
            this->propertyvalue->Text = L"Value";
            this->propertyvalue->Width = 130;
            // 
            // type
            // 
            this->type->Text = L"Data type";
            this->type->Width = 130;
            // 
            // description
            // 
            this->description->Text = L"Description";
            this->description->Width = 238;
            // 
            // Settings
            // 
            this->Settings->BackColor = System::Drawing::Color::White;
            this->Settings->Controls->Add(this->group_github);
            this->Settings->Controls->Add(this->group_update);
            this->Settings->Controls->Add(this->group_settings);
            this->Settings->Controls->Add(this->label_info);
            this->Settings->Location = System::Drawing::Point(4, 28);
            this->Settings->Name = L"Settings";
            this->Settings->Size = System::Drawing::Size(712, 619);
            this->Settings->TabIndex = 2;
            this->Settings->Text = L"Settings";
            this->Settings->ToolTipText = L"View information and settings";
            // 
            // group_github
            // 
            this->group_github->Controls->Add(this->label_github);
            this->group_github->Location = System::Drawing::Point(506, 513);
            this->group_github->Name = L"group_github";
            this->group_github->Size = System::Drawing::Size(192, 94);
            this->group_github->TabIndex = 30;
            this->group_github->TabStop = false;
            // 
            // label_github
            // 
            this->label_github->Anchor = System::Windows::Forms::AnchorStyles::None;
            this->label_github->BackColor = System::Drawing::Color::White;
            this->label_github->Cursor = System::Windows::Forms::Cursors::Hand;
            this->label_github->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->label_github->ForeColor = System::Drawing::Color::Black;
            this->label_github->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"label_github.Image")));
            this->label_github->Location = System::Drawing::Point(1, 7);
            this->label_github->Name = L"label_github";
            this->label_github->Size = System::Drawing::Size(190, 85);
            this->label_github->TabIndex = 29;
            this->label_github->Text = L"      View on GitHub";
            this->label_github->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->label_github->Click += gcnew System::EventHandler(this, &MainForm::label_github_click);
            this->label_github->MouseEnter += gcnew System::EventHandler(this, &MainForm::label_github_mousehover);
            this->label_github->MouseLeave += gcnew System::EventHandler(this, &MainForm::label_github_mouseleave);
            // 
            // group_update
            // 
            this->group_update->Controls->Add(this->check_updatenotify);
            this->group_update->Controls->Add(this->label_version);
            this->group_update->Location = System::Drawing::Point(286, 513);
            this->group_update->Name = L"group_update";
            this->group_update->Size = System::Drawing::Size(205, 94);
            this->group_update->TabIndex = 28;
            this->group_update->TabStop = false;
            this->group_update->Text = L"Updates";
            // 
            // check_updatenotify
            // 
            this->check_updatenotify->AutoSize = true;
            this->check_updatenotify->Location = System::Drawing::Point(12, 66);
            this->check_updatenotify->Name = L"check_updatenotify";
            this->check_updatenotify->Size = System::Drawing::Size(188, 17);
            this->check_updatenotify->TabIndex = 29;
            this->check_updatenotify->Text = L"Notify when an update is available";
            this->check_updatenotify->UseVisualStyleBackColor = true;
            this->check_updatenotify->CheckedChanged += gcnew System::EventHandler(this, &MainForm::check_updatenotify_changed);
            // 
            // label_version
            // 
            this->label_version->Location = System::Drawing::Point(9, 19);
            this->label_version->Name = L"label_version";
            this->label_version->Size = System::Drawing::Size(186, 44);
            this->label_version->TabIndex = 28;
            this->label_version->Text = L"Status: Checking for updates...\r\nCurrent version: 1.0\r\nLatest version:\r\n";
            // 
            // group_settings
            // 
            this->group_settings->Controls->Add(this->label_theme);
            this->group_settings->Controls->Add(this->combo_backup);
            this->group_settings->Controls->Add(this->check_debug);
            this->group_settings->Controls->Add(this->label_backup);
            this->group_settings->Controls->Add(this->combo_theme);
            this->group_settings->Location = System::Drawing::Point(12, 513);
            this->group_settings->Name = L"group_settings";
            this->group_settings->Size = System::Drawing::Size(259, 94);
            this->group_settings->TabIndex = 26;
            this->group_settings->TabStop = false;
            this->group_settings->Text = L"Settings";
            // 
            // label_theme
            // 
            this->label_theme->AutoSize = true;
            this->label_theme->Location = System::Drawing::Point(134, 19);
            this->label_theme->Name = L"label_theme";
            this->label_theme->Size = System::Drawing::Size(43, 13);
            this->label_theme->TabIndex = 27;
            this->label_theme->Text = L"Theme:";
            // 
            // combo_backup
            // 
            this->combo_backup->FormattingEnabled = true;
            this->combo_backup->Items->AddRange(gcnew cli::array< System::Object^  >(3) {
                L"Once (original)", L"Every time (latest)",
                    L"Off"
            });
            this->combo_backup->Location = System::Drawing::Point(12, 35);
            this->combo_backup->Name = L"combo_backup";
            this->combo_backup->Size = System::Drawing::Size(110, 21);
            this->combo_backup->TabIndex = 28;
            this->combo_backup->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::combo_backup_SelectedIndexChanged);
            // 
            // check_debug
            // 
            this->check_debug->AutoSize = true;
            this->check_debug->Location = System::Drawing::Point(12, 66);
            this->check_debug->Name = L"check_debug";
            this->check_debug->Size = System::Drawing::Size(126, 17);
            this->check_debug->TabIndex = 26;
            this->check_debug->Text = L"Show debug console";
            this->check_debug->UseVisualStyleBackColor = true;
            this->check_debug->CheckedChanged += gcnew System::EventHandler(this, &MainForm::check_debug_changed);
            // 
            // label_backup
            // 
            this->label_backup->AutoSize = true;
            this->label_backup->Location = System::Drawing::Point(9, 19);
            this->label_backup->Name = L"label_backup";
            this->label_backup->Size = System::Drawing::Size(88, 13);
            this->label_backup->TabIndex = 24;
            this->label_backup->Text = L"Backup on save:";
            // 
            // combo_theme
            // 
            this->combo_theme->FormattingEnabled = true;
            this->combo_theme->Items->AddRange(gcnew cli::array< System::Object^  >(7) {
                L"Death Island", L"Blood Gulch", L"Damnation",
                    L"Battle Creek", L"Derilect", L"Ice", L"Invidious"
            });
            this->combo_theme->Location = System::Drawing::Point(137, 35);
            this->combo_theme->Name = L"combo_theme";
            this->combo_theme->Size = System::Drawing::Size(110, 21);
            this->combo_theme->TabIndex = 25;
            this->combo_theme->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::combo_theme_indexchanged);
            // 
            // label_info
            // 
            this->label_info->Anchor = System::Windows::Forms::AnchorStyles::Top;
            this->label_info->BackColor = System::Drawing::Color::Transparent;
            this->label_info->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
            this->label_info->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->label_info->ForeColor = System::Drawing::SystemColors::Window;
            this->label_info->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"label_info.Image")));
            this->label_info->Location = System::Drawing::Point(12, 14);
            this->label_info->Name = L"label_info";
            this->label_info->Size = System::Drawing::Size(686, 490);
            this->label_info->TabIndex = 23;
            this->label_info->Text = L"HaloCE Gametype Editor\r\nv1.0\r\n\r\nmade by noctilucent";
            this->label_info->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            // 
            // label_name
            // 
            this->label_name->Anchor = System::Windows::Forms::AnchorStyles::Top;
            this->label_name->BackColor = System::Drawing::Color::White;
            this->label_name->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->label_name->ForeColor = System::Drawing::SystemColors::Window;
            this->label_name->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"label_name.Image")));
            this->label_name->ImageAlign = System::Drawing::ContentAlignment::BottomCenter;
            this->label_name->Location = System::Drawing::Point(28, 48);
            this->label_name->Name = L"label_name";
            this->label_name->Size = System::Drawing::Size(686, 67);
            this->label_name->TabIndex = 22;
            this->label_name->Text = L"New001";
            this->label_name->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            this->label_name->Click += gcnew System::EventHandler(this, &MainForm::label_name_click);
            // 
            // tab_file
            // 
            this->tab_file->Controls->Add(this->file_open);
            this->tab_file->Controls->Add(this->file_save);
            this->tab_file->Controls->Add(this->file_saveas);
            this->tab_file->ItemSize = System::Drawing::Size(100, 24);
            this->tab_file->Location = System::Drawing::Point(428, 12);
            this->tab_file->Name = L"tab_file";
            this->tab_file->SelectedIndex = 0;
            this->tab_file->ShowToolTips = true;
            this->tab_file->Size = System::Drawing::Size(304, 27);
            this->tab_file->SizeMode = System::Windows::Forms::TabSizeMode::Fixed;
            this->tab_file->TabIndex = 19;
            this->tab_file->Click += gcnew System::EventHandler(this, &MainForm::tab_file_click);
            this->tab_file->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::tab_file_mouseup);
            // 
            // file_open
            // 
            this->file_open->Location = System::Drawing::Point(4, 28);
            this->file_open->Name = L"file_open";
            this->file_open->Padding = System::Windows::Forms::Padding(3);
            this->file_open->Size = System::Drawing::Size(296, 0);
            this->file_open->TabIndex = 0;
            this->file_open->Text = L"Open";
            this->file_open->ToolTipText = L"Open a HaloCE gametype for editing";
            this->file_open->UseVisualStyleBackColor = true;
            // 
            // file_save
            // 
            this->file_save->BackColor = System::Drawing::Color::BlueViolet;
            this->file_save->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"file_save.BackgroundImage")));
            this->file_save->ForeColor = System::Drawing::SystemColors::ActiveCaption;
            this->file_save->Location = System::Drawing::Point(4, 28);
            this->file_save->Name = L"file_save";
            this->file_save->Padding = System::Windows::Forms::Padding(3);
            this->file_save->Size = System::Drawing::Size(296, 0);
            this->file_save->TabIndex = 1;
            this->file_save->Text = L"Save";
            this->file_save->ToolTipText = L"Save all changes to the current gametype";
            // 
            // file_saveas
            // 
            this->file_saveas->Location = System::Drawing::Point(4, 28);
            this->file_saveas->Name = L"file_saveas";
            this->file_saveas->Size = System::Drawing::Size(296, 0);
            this->file_saveas->TabIndex = 2;
            this->file_saveas->Text = L"Save As";
            this->file_saveas->ToolTipText = L"Save the current gametype as another file";
            this->file_saveas->UseVisualStyleBackColor = true;
            // 
            // label_warning
            // 
            this->label_warning->BackColor = System::Drawing::Color::Transparent;
            this->label_warning->Location = System::Drawing::Point(344, 15);
            this->label_warning->Name = L"label_warning";
            this->label_warning->Size = System::Drawing::Size(80, 23);
            this->label_warning->TabIndex = 23;
            this->label_warning->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
            // 
            // picbox_notify
            // 
            this->picbox_notify->BackColor = System::Drawing::Color::Transparent;
            this->picbox_notify->Location = System::Drawing::Point(325, 16);
            this->picbox_notify->Name = L"picbox_notify";
            this->picbox_notify->Size = System::Drawing::Size(21, 21);
            this->picbox_notify->TabIndex = 24;
            this->picbox_notify->TabStop = false;
            // 
            // label_save_no
            // 
            this->label_save_no->BackColor = System::Drawing::SystemColors::ControlLight;
            this->label_save_no->Cursor = System::Windows::Forms::Cursors::No;
            this->label_save_no->ForeColor = System::Drawing::SystemColors::GrayText;
            this->label_save_no->Location = System::Drawing::Point(530, 15);
            this->label_save_no->Name = L"label_save_no";
            this->label_save_no->Size = System::Drawing::Size(99, 23);
            this->label_save_no->TabIndex = 25;
            this->label_save_no->Text = L" Save";
            this->label_save_no->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            // 
            // label_saveas_no
            // 
            this->label_saveas_no->BackColor = System::Drawing::SystemColors::ControlLight;
            this->label_saveas_no->Cursor = System::Windows::Forms::Cursors::No;
            this->label_saveas_no->ForeColor = System::Drawing::SystemColors::GrayText;
            this->label_saveas_no->Location = System::Drawing::Point(630, 15);
            this->label_saveas_no->Name = L"label_saveas_no";
            this->label_saveas_no->Size = System::Drawing::Size(97, 23);
            this->label_saveas_no->TabIndex = 26;
            this->label_saveas_no->Text = L" Save As";
            this->label_saveas_no->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            // 
            // MainForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Center;
            this->ClientSize = System::Drawing::Size(742, 707);
            this->Controls->Add(this->label_saveas_no);
            this->Controls->Add(this->label_save_no);
            this->Controls->Add(this->picbox_notify);
            this->Controls->Add(this->label_warning);
            this->Controls->Add(this->label_name);
            this->Controls->Add(this->tab_file);
            this->Controls->Add(this->tab_main);
            this->Controls->Add(this->button_close);
            this->Controls->Add(this->text_name);
            this->DoubleBuffered = true;
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
            this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
            this->MaximizeBox = false;
            this->Name = L"MainForm";
            this->Text = L"HaloCE Gametype Editor";
            this->Load += gcnew System::EventHandler(this, &MainForm::MainForm_Load);
            this->group_player->ResumeLayout(false);
            this->group_player->PerformLayout();
            this->group_item->ResumeLayout(false);
            this->group_item->PerformLayout();
            this->group_game->ResumeLayout(false);
            this->group_game->PerformLayout();
            this->group_indicator->ResumeLayout(false);
            this->group_indicator->PerformLayout();
            this->group_teamplay->ResumeLayout(false);
            this->group_teamplay->PerformLayout();
            this->tab_main->ResumeLayout(false);
            this->Normal->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->banner))->EndInit();
            this->groupBox1->ResumeLayout(false);
            this->groupBox1->PerformLayout();
            this->group_blueteam->ResumeLayout(false);
            this->group_blueteam->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_blueteam_gunturret))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_blueteam_banshee))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_blueteam_rockethog))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_blueteam_scorpion))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_blueteam_ghost))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_blueteam_warthog))->EndInit();
            this->group_redteam->ResumeLayout(false);
            this->group_redteam->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_redteam_gunturret))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_redteam_banshee))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_redteam_rockethog))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_redteam_scorpion))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_redteam_ghost))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->num_redteam_warthog))->EndInit();
            this->Advanced->ResumeLayout(false);
            this->group_details->ResumeLayout(false);
            this->Settings->ResumeLayout(false);
            this->group_github->ResumeLayout(false);
            this->group_update->ResumeLayout(false);
            this->group_update->PerformLayout();
            this->group_settings->ResumeLayout(false);
            this->group_settings->PerformLayout();
            this->tab_file->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->picbox_notify))->EndInit();
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion
	// Events
	// ----------------------------------------------------
	private: System::Void button_close_Click(System::Object^ sender, System::EventArgs^ e) {
		System::Windows::Forms::Application::Exit();
	}
    private: System::Void tab_file_click(System::Object^ sender, System::EventArgs^ e) {
        Gametype gametype = Gametype(); // Re-initialise to default values
        std::vector<Gametype> gametypes;
        if (tab_file->SelectedIndex == 0) {
            std::cout << "[Open]" << std::endl;
            OpenFileDialog^ openFileDialog1 = gcnew OpenFileDialog();

            //openFileDialog1->InitialDirectory = "C:\\"; // Set the initial directory if needed.
            openFileDialog1->Filter = "HaloCE Gametype|blam.lst|All Files|*.*"; // Filter for gametypes, or other files (if you want it to fail later)
            openFileDialog1->Title = "Select a gametype's blam.lst file";

            if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                // User selected a file.
                String^ selectedFileName = openFileDialog1->FileName;
                SFilePath = selectedFileName;

                // Check if the selected file name is "blam.lst".
                if (Path::GetFileName(selectedFileName) == "blam.lst") {
			        //MessageBox::Show(selectedFileName, "Valid gametype selected", MessageBoxButtons::OK, MessageBoxIcon::Information);
                    // Convert System::String^ to std::string
                    std::string filePath;
                    for (int i = 0; i < selectedFileName->Length; i++) {
                        filePath += (char)selectedFileName[i];
                    }

                    Gametype gametype = {}; // Create an instance of the Gametype struct and initialize
            
                    if (readGametypesFromBinaryFile(filePath, gametypes)) {
                        for (Gametype& gametype : gametypes) {
                            try {
                                // Open the registry key for reading
                                RegistryKey^ key = Registry::CurrentUser->OpenSubKey(sRegistryKey, true);

                                if (key != nullptr) {
                                    // Read the current DWORD value
                                    Object^ loadCountValue = key->GetValue("LoadCount");

                                    // Check if the value exists and is an integer
                                    if (loadCountValue != nullptr && loadCountValue->GetType() == int::typeid) {
                                        // Increment the value
                                        int currentCount = safe_cast<int>(loadCountValue);
                                        int newCount = currentCount + 1;

                                        // Save the updated value back to the registry
                                        key->SetValue("LoadCount", newCount, RegistryValueKind::DWord);
                                    }
                                    else {
                                        // If the value doesn't exist or is not an integer, set it to 1
                                        key->SetValue("LoadCount", 1, RegistryValueKind::DWord);
                                    }
                                }
                            }
                            catch (Exception^ ex) {
                                std::cerr << "Error updating LoadCount in registry." << std::endl;
                            }

                            int iGtMod1 = static_cast<int>(gametype.GtMod1);
                            int iGtMod2 = static_cast<int>(gametype.GtMod2);
                            int iGtMod3 = static_cast<int>(gametype.GtMod3);
                            int iGtMod4 = static_cast<int>(gametype.GtMod4);

                            // Gametype name
                            System::String^ name = gcnew System::String(gametype.Name);
                            SetName(name);

                            // Game Mode
                            SGtMode = System::Convert::ToString(static_cast<UInt32>(gametype.GameMode));
                            SetInt(0, gametype.GameMode);

                            // Team play
                            SetInt(8, gametype.TeamPlay);

                            // Parameters
                            unsigned char byte = (gametype.Parameters >> 0) & 0xFF;
                            unsigned char reversedByte = 0;
                            for (int i = 0; i < 8; i++) {
                                reversedByte |= ((byte >> i) & 1) << (7 - i);
                            }
                            std::bitset<8> reversedBinaryByte(byte);
                            std::cout << "Parameters: " << gametype.Parameters << " (" << reversedBinaryByte << ")" << std::endl;
                            // 0 = Other players on radar, 1 = Friend indicators on screen, 2 = Infinite grenades, 3 = No shields, 4 = Invisible players
                            // 5 = Starting equipment (1 = generic, 0 = custom), 6 = Only friends on radar, 7 = Unknown
                            bool bShowRadar = reversedBinaryByte.test(0);
                            bool bFriendIndicatorsOnScreen = reversedBinaryByte.test(1);
                            bool bInfiniteGrenades = reversedBinaryByte.test(2);
                            bool bNoShields = reversedBinaryByte.test(3);
                            bool bInvisiblePlayers = reversedBinaryByte.test(4);
                            bool bCustomStartingEquipment = reversedBinaryByte.test(5);
                            bool bAllPlayersOnRadar = reversedBinaryByte.test(6);
                            bool bUnknown = reversedBinaryByte.test(7);

                            std::cout << "bShowRadar: " << bShowRadar << ", bFriendIndicatorsOnScreen: " << bFriendIndicatorsOnScreen << ", bInfiniteGrenades: " << bInfiniteGrenades << ", bNoShields: " << bNoShields << std::endl;
                            std::cout << "bInvisiblePlayers: " << bInvisiblePlayers << ", bCustomStartingEquipment: " << bCustomStartingEquipment << ", bAllPlayersOnRadar: " << bAllPlayersOnRadar << ", bUnknown: " << bUnknown << std::endl;

                            if (bAllPlayersOnRadar == 1) {
                                SetInt(39, 0);
                            } else {
                                SetInt(39, 1);
                            }
                            SetInt(38, bShowRadar);
                            
                            SetInt(40, bFriendIndicatorsOnScreen);
                            SetInt(21, bInfiniteGrenades);

                            if (bNoShields == 1) {
                                SetInt(16, 0);
                            }
                            else {
                                SetInt(16, 1);
                            }

                            SetInt(18, bInvisiblePlayers);
                            
                            if (bCustomStartingEquipment == 1) {
                                SetInt(20, 0);
                            }
                            else {
                                SetInt(20, 1);
                            }

                            // Navigation objectives
                            SetInt(37, gametype.NavIndicator);
                            
                            // Odd man out
                            SetInt(17, gametype.OddManOut);

                            // Respawn time growth
                            SetInt(14, gametype.RespawnTimeGrowth);

                            // Respawn time
                            SetInt(13, gametype.RespawnTime);

                            // Suicide penalty
                            SetInt(15, gametype.SuicidePenalty);

                            // Number of lives
                            SetInt(12, gametype.NumLives);

                            // Health
                            SetFloat(0, std::to_string(gametype.Health));

                            // Score limit
                            SetInt(1, gametype.ScoreLimit);

                            // Weapon set
                            SetInt(19, gametype.WeaponSet);

                            // Vehicle respawn time
                            SetInt(22, gametype.VehicleRespawnTime);

                            // RedVehicles
                            std::stringstream ssrv;
                            ssrv << std::hex << gametype.RedVehicles;
                            std::string sRedVehiclesHex = ssrv.str();
                            int iRedVehicleSet = VehicleValue(gametype.RedVehicles, 1);
                            int iRedHogs = VehicleValue(gametype.RedVehicles, 2);
                            int iRedGhosts = VehicleValue(gametype.RedVehicles, 3);
                            int iRedScorpions = VehicleValue(gametype.RedVehicles, 4);
                            int iRedRocketHogs = VehicleValue(gametype.RedVehicles, 5);
                            int iRedBanshees = VehicleValue(gametype.RedVehicles, 6);
                            int iRedTurrets = VehicleValue(gametype.RedVehicles, 7);
                            
                            char cRedVehicleSet = sRedVehiclesHex[sRedVehiclesHex.length() - 1];
                            int iRedActualVehicleSet = cRedVehicleSet - '0';
                            SetInt(23, iRedActualVehicleSet);
                            if (iRedActualVehicleSet == 8) {
                                SetInt(24, iRedHogs);
                                SetInt(25, iRedGhosts);
                                SetInt(26, iRedScorpions);
                                SetInt(27, iRedRocketHogs);
                                SetInt(28, iRedBanshees);
                                SetInt(29, iRedTurrets);
                            }

                            if (gametype.TeamPlay == 1) {
                                group_redteam->Text = "Red team";
                                group_blueteam->Text = "Blue team";

                                // BlueVehicles
                                combo_blueteam_vehicleset->Enabled = true;
                                std::stringstream ssbv;
                                ssbv << std::hex << gametype.BlueVehicles;
                                std::string sBlueVehiclesHex = ssbv.str();
                                int iBlueVehicleSet = VehicleValue(gametype.BlueVehicles, 1);
                                int iBlueHogs = VehicleValue(gametype.BlueVehicles, 2);
                                int iBlueGhosts = VehicleValue(gametype.BlueVehicles, 3);
                                int iBlueScorpions = VehicleValue(gametype.BlueVehicles, 4);
                                int iBlueRocketHogs = VehicleValue(gametype.BlueVehicles, 5);
                                int iBlueBanshees = VehicleValue(gametype.BlueVehicles, 6);
                                int iBlueTurrets = VehicleValue(gametype.BlueVehicles, 7);
                                char cBlueVehicleSet = sBlueVehiclesHex[sBlueVehiclesHex.length() - 1];
                                int iBlueActualVehicleSet = cBlueVehicleSet - '0';
                                SetInt(30, iBlueActualVehicleSet);
                                if (iBlueActualVehicleSet == 8) {
                                    SetInt(31, iBlueHogs);
                                    SetInt(32, iBlueGhosts);
                                    SetInt(33, iBlueScorpions);
                                    SetInt(34, iBlueRocketHogs);
                                    SetInt(35, iBlueBanshees);
                                    SetInt(36, iBlueTurrets);
                                }
                            } else {
                                group_redteam->Text = "Free for all";
                                group_blueteam->Text = "Unused";
                                //combo_blueteam_vehicleset->Text = "";
                                combo_blueteam_vehicleset->Enabled = false;
                                SetInt(31, 0);
                                SetInt(32, 0);
                                SetInt(33, 0);
                                SetInt(34, 0);
                                SetInt(35, 0);
                                SetInt(36, 0);
                                num_blueteam_warthog->Enabled = false;
                                num_blueteam_ghost->Enabled = false;
                                num_blueteam_scorpion->Enabled = false;
                                num_blueteam_rockethog->Enabled = false;
                                num_blueteam_banshee->Enabled = false;
                                num_blueteam_gunturret->Enabled = false;
                            }

                            // Friendly fire
                            SetInt(10, gametype.FriendlyFire);

                            // Friendly fire penalty
                            SetInt(11, gametype.FriendlyFirePenalty);
                          
                            // Team balance
                            SetInt(9, gametype.TeamBalance);

                            // Time limit
                            SetInt(2, gametype.TimeLimit);

                            // Print actual gametype modifiers
                            //std::cout << "GtMod1: " << iGtMod1 << std::endl;
                            //::cout << "GtMod2: " << iGtMod2 << std::endl;
                            //std::cout << "GtMod3: " << iGtMod3 << std::endl;
                            //std::cout << "GtMod4: " << iGtMod4 << std::endl;
                            //std::cout << "GtMod5: " << gametype.GtMod5 << std::endl;

                            if (gametype.GameMode == 2) {
                                check_gtmod1->Checked = (iGtMod1 == 1) ? false : true;
                                check_gtmod2->Checked = (iGtMod2 == 1) ? false : true;
                                check_gtmod3->Checked = (iGtMod3 == 1) ? true : false;
                                check_gtmod4->Checked = (iGtMod4 == 1) ? true : false;
                                list_properties->Items[4]->SubItems[1]->Text = (iGtMod1 == 1) ? System::Convert::ToString(0) : System::Convert::ToString(1);
                                list_properties->Items[5]->SubItems[1]->Text = (iGtMod1 == 1) ? System::Convert::ToString(0) : System::Convert::ToString(1);
                                list_properties->Items[6]->SubItems[1]->Text = System::Convert::ToString(iGtMod3);
                                list_properties->Items[7]->SubItems[1]->Text = System::Convert::ToString(iGtMod4);
                                list_properties->Items[8]->SubItems[1]->Text = System::Convert::ToString(static_cast<UInt32>(gametype.GtMod5));
                            } else {
                                check_gtmod1->Checked = (iGtMod1 == 1) ? true : false;
                                check_gtmod2->Checked = (iGtMod2 == 1) ? true : false;
                                check_gtmod3->Checked = (iGtMod3 == 1) ? true : false;
                                check_gtmod4->Checked = (iGtMod4 == 1) ? true : false;
                                list_properties->Items[4]->SubItems[1]->Text = System::Convert::ToString(iGtMod1);
                                list_properties->Items[5]->SubItems[1]->Text = System::Convert::ToString(iGtMod2);
                                list_properties->Items[6]->SubItems[1]->Text = System::Convert::ToString(iGtMod3);
                                list_properties->Items[7]->SubItems[1]->Text = System::Convert::ToString(iGtMod4);
                                list_properties->Items[8]->SubItems[1]->Text = System::Convert::ToString(static_cast<UInt32>(gametype.GtMod5));
                            }

                            SetInt(7, gametype.GtMod5);
                            
                            DWORD OldCRC = gametype.NewCRC;

                            // Calculate the size of the struct
                            size_t structSize = sizeof(Gametype);

                            // Exclude the NewCRC field
                            size_t byteBufferLength = structSize - sizeof(gametype.NewCRC);

                            // Create a byte buffer using std::vector
                            std::vector<BYTE> byteBuffer(byteBufferLength);

                            // Copy the data from gametype to the byte buffer
                            std::memcpy(byteBuffer.data(), &gametype, byteBufferLength);

                            std::string_view str = byteBufferToStringView(byteBuffer);
                            uint32_t NewCRC = GetCRC(str);
                            gametype.NewCRC = NewCRC;

                            std::cout << "Old CRC: " << std::hex << OldCRC << std::dec << " (" << OldCRC << ")" << std::endl;
                            std::cout << "New CRC: " << std::hex << NewCRC << std::dec << " (" << NewCRC << ")" << std::endl;

                            label_save_no->Visible = false;
                            label_saveas_no->Visible = false;

                            if (OldCRC != NewCRC) {
                                std::cerr << "CRC mismatch!" << std::endl;
                                label_warning->Text = "CRC mismatch!";
                                // Get a system icon (e.g., information icon)
                                System::Drawing::Icon^ infoIcon = System::Drawing::SystemIcons::Warning;

                                // Convert the System::Drawing::Icon to a Bitmap
                                System::Drawing::Bitmap^ originalBitmap = gcnew System::Drawing::Bitmap(infoIcon->ToBitmap());

                                // Create a new Bitmap with half the size
                                System::Drawing::Bitmap^ scaledBitmap = gcnew System::Drawing::Bitmap(originalBitmap->Width / 1.75, originalBitmap->Height / 1.75);

                                // Create a Graphics object to perform the scaling
                                System::Drawing::Graphics^ graphics = System::Drawing::Graphics::FromImage(scaledBitmap);

                                // Draw the original image onto the scaled Bitmap with half the size
                                graphics->DrawImage(originalBitmap, 0, 0, scaledBitmap->Width, scaledBitmap->Height);

                                // Set the PictureBox's Image property
                                picbox_notify->Image = scaledBitmap;
                            } else {
                                label_warning->Text = "";
                                picbox_notify->Image = nullptr;
                            }

                            UpdateBanner();

                            if (gametype.NewCRC != 0) {
                                break;
                            }
                        }
                    }
            

                }
                else {
			        MessageBox::Show("Please select a valid HaloCE gametype file. (blam.lst)\n\nBy default they are located in C:\\Users\\<username>\\Documents\\My Games\\Halo CE\\savegames\\<gametype>\\blam.lst", "Invalid gametype selected", MessageBoxButtons::OK, MessageBoxIcon::Error);
                }
            }
        } else if (tab_file->SelectedIndex > 0) {
            System::String^ SNewFilePath;
            std::string sNewFilePath;
            if (tab_file->SelectedIndex == 1) {
                std::cout << "[Save]" << std::endl;
            } else if (tab_file->SelectedIndex == 2) {
                std::cout << "[Save As]" << std::endl;
                SaveFileDialog^ saveFileDialog1 = gcnew SaveFileDialog();
                saveFileDialog1->Filter = "HaloCE Gametype|blam.lst|All Files|*.*"; // Filter for gametypes, or other files (if you want it to fail later)
                if (saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                    SNewFilePath = saveFileDialog1->FileName;
                    for (int i = 0; i < SNewFilePath->Length; i++) {
                        sNewFilePath += (char)SNewFilePath[i];
                    }
                }
            }

            std::string filePath;
            for (int i = 0; i < SFilePath->Length; i++) {
                filePath += (char)SFilePath[i];
            }

            if (readGametypesFromBinaryFile(filePath, gametypes)) {
                for (Gametype& gametype : gametypes) {
                    // Do the thing
                    // Copy data from interface to gametype struct
                    
                    // Name
                    System::String^ textValue = text_name->Text;
                    pin_ptr<const wchar_t> wch = PtrToStringChars(textValue);
                    wcscpy_s(gametype.Name, wch);

                    // GameMode
                    String^ SGameMode = list_properties->Items[1]->SubItems[1]->Text;
                    gametype.GameMode = Convert::ToUInt32(SGameMode);

                    // TeamPlay
                    String^ STeamPlay = list_properties->Items[9]->SubItems[1]->Text;
                    gametype.TeamPlay = Convert::ToUInt32(STeamPlay);

                    // Parameters
                    bool bShowRadar = (list_properties->Items[40]->SubItems[1]->Text == "1");
                    bool bFriendIndicatorsOnScreen = (list_properties->Items[42]->SubItems[1]->Text == "1");
                    bool bInfiniteGrenades = (list_properties->Items[23]->SubItems[1]->Text == "1");
                    bool bNoShields = (list_properties->Items[18]->SubItems[1]->Text == "0");
                    bool bInvisiblePlayers = (list_properties->Items[20]->SubItems[1]->Text == "1");
                    bool bCustomStartingEquipment = (list_properties->Items[22]->SubItems[1]->Text == "0");
                    bool bAllPlayersOnRadar = (list_properties->Items[41]->SubItems[1]->Text == "0");
                    bool bUnknown = false;

                    // Create a reversed binary byte
                    unsigned char reversedBinaryByte = 0;

                    // Set the bits based on boolean values
                    reversedBinaryByte |= (bShowRadar ? 1 : 0) << 0;
                    reversedBinaryByte |= (bFriendIndicatorsOnScreen ? 1 : 0) << 1;
                    reversedBinaryByte |= (bInfiniteGrenades ? 1 : 0) << 2;
                    reversedBinaryByte |= (bNoShields ? 1 : 0) << 3;
                    reversedBinaryByte |= (bInvisiblePlayers ? 1 : 0) << 4;
                    reversedBinaryByte |= (bCustomStartingEquipment ? 1 : 0) << 5;
                    reversedBinaryByte |= (bAllPlayersOnRadar ? 1 : 0) << 6;
                    reversedBinaryByte |= (bUnknown ? 1 : 0) << 7;

                    // Output the reversed binary byte
                    std::bitset<8> reversedBinaryByteBitset(reversedBinaryByte);
                    std::cout << "Parameters: " << static_cast<int>(reversedBinaryByte) << " (" << reversedBinaryByteBitset << ")" << std::endl;
                    unsigned int reversedParameters = reversedBinaryByte;
                    gametype.Parameters = reversedParameters;

                    // NavIndicator
                    String^ SNavIndicator = list_properties->Items[39]->SubItems[1]->Text;
                    gametype.NavIndicator = Convert::ToUInt32(SNavIndicator);

                    // OddManOut
                    String^ SOddManOut = list_properties->Items[19]->SubItems[1]->Text;
                    gametype.OddManOut = Convert::ToUInt32(SOddManOut);

                    // RespawnTimeGrowth
                    String^ SRespawnTimeGrowth = list_properties->Items[15]->SubItems[1]->Text;
                    gametype.RespawnTimeGrowth = Convert::ToUInt32(SRespawnTimeGrowth);

                    // RespawnTime
                    String^ SRespawnTime = list_properties->Items[14]->SubItems[1]->Text;
                    gametype.RespawnTime = Convert::ToUInt32(SRespawnTime);

                    // SuicidePenalty
                    String^ SSuicidePenalty = list_properties->Items[16]->SubItems[1]->Text;
                    gametype.SuicidePenalty = Convert::ToUInt32(SSuicidePenalty);

                    // NumLives
                    String^ SNumLives = list_properties->Items[13]->SubItems[1]->Text;
                    gametype.NumLives = Convert::ToUInt32(SNumLives);

                    // Health
                    String^ SHealth = list_properties->Items[17]->SubItems[1]->Text;
                    gametype.Health = Convert::ToSingle(SHealth);

                    // ScoreLimit
                    String^ SScoreLimit = list_properties->Items[2]->SubItems[1]->Text;
                    gametype.ScoreLimit = Convert::ToUInt32(SScoreLimit);

                    // WeaponSet
                    String^ SWeaponSet = list_properties->Items[21]->SubItems[1]->Text;
                    gametype.WeaponSet = Convert::ToUInt32(SWeaponSet);

                    // RedVehicles
                    int iRedVehicleSet = Convert::ToUInt32(list_properties->Items[25]->SubItems[1]->Text);
                    int iRedWarthogs = Convert::ToUInt32(list_properties->Items[26]->SubItems[1]->Text);
                    int iRedGhosts = Convert::ToUInt32(list_properties->Items[27]->SubItems[1]->Text);
                    int iRedScorpions = Convert::ToUInt32(list_properties->Items[28]->SubItems[1]->Text);
                    int iRedRocketHogs = Convert::ToUInt32(list_properties->Items[29]->SubItems[1]->Text);
                    int iRedBanshees = Convert::ToUInt32(list_properties->Items[30]->SubItems[1]->Text);
                    int iRedTurrets = Convert::ToUInt32(list_properties->Items[31]->SubItems[1]->Text);
                    if (iRedVehicleSet != 8) {
                        iRedWarthogs = 4; iRedGhosts = 4; iRedScorpions = 4; iRedRocketHogs = 4; iRedBanshees = 4; iRedTurrets = 4;
                    }
                    gametype.RedVehicles = SetVehicleValue(0, iRedVehicleSet, iRedWarthogs, iRedGhosts, iRedScorpions, iRedRocketHogs, iRedBanshees, iRedTurrets);

                    // BlueVehicles
                    int iBlueVehicleSet = Convert::ToUInt32(list_properties->Items[32]->SubItems[1]->Text);
                    int iBlueWarthogs = Convert::ToUInt32(list_properties->Items[33]->SubItems[1]->Text);
                    int iBlueGhosts = Convert::ToUInt32(list_properties->Items[34]->SubItems[1]->Text);
                    int iBlueScorpions = Convert::ToUInt32(list_properties->Items[35]->SubItems[1]->Text);
                    int iBlueRocketHogs = Convert::ToUInt32(list_properties->Items[36]->SubItems[1]->Text);
                    int iBlueBanshees = Convert::ToUInt32(list_properties->Items[37]->SubItems[1]->Text);
                    int iBlueTurrets = Convert::ToUInt32(list_properties->Items[38]->SubItems[1]->Text);
                    if (iBlueVehicleSet != 8) {
                        iBlueWarthogs = 4; iBlueGhosts = 4; iBlueScorpions = 4; iBlueRocketHogs = 4; iBlueBanshees = 4; iBlueTurrets = 4;
                    }
                    gametype.BlueVehicles = SetVehicleValue(0, iBlueVehicleSet, iBlueWarthogs, iBlueGhosts, iBlueScorpions, iBlueRocketHogs, iBlueBanshees, iBlueTurrets);

                    // VehicleRespawnTime
                    String^ SVehicleRespawnTime = list_properties->Items[24]->SubItems[1]->Text;
                    gametype.VehicleRespawnTime = Convert::ToUInt32(SVehicleRespawnTime);

                    // FriendlyFire
                    String^ SFriendlyFire = list_properties->Items[11]->SubItems[1]->Text;
                    gametype.FriendlyFire = Convert::ToUInt32(SFriendlyFire);

                    // FriendlyFirePenalty
                    String^ SFriendlyFirePenalty = list_properties->Items[12]->SubItems[1]->Text;
                    gametype.FriendlyFirePenalty = Convert::ToUInt32(SFriendlyFirePenalty);

                    // TeamBalance
                    String^ STeamBalance = list_properties->Items[10]->SubItems[1]->Text;
                    gametype.TeamBalance = Convert::ToUInt32(STeamBalance);

                    // TimeLimit
                    String^ STimeLimit = list_properties->Items[3]->SubItems[1]->Text;
                    gametype.TimeLimit = Convert::ToUInt32(STimeLimit);


                    if (gametype.GameMode == 2) {
                        // GtMod1
                        String^ SGtMod1 = list_properties->Items[4]->SubItems[1]->Text;
                        if (SGtMod1 == "1") {
                            gametype.GtMod1 = 0;
                        }
                        else {
                            gametype.GtMod1 = 1;
                        }
                        
                        // GtMod2
                        String^ SGtMod2 = list_properties->Items[5]->SubItems[1]->Text;
                        if (SGtMod2 == "1") {
                            gametype.GtMod2 = 0;
                        }
                        else {
                            gametype.GtMod2 = 1;
                        }
                    }
                    else {
                        // GtMod1
                        String^ SGtMod1 = list_properties->Items[4]->SubItems[1]->Text;
                        gametype.GtMod1 = Convert::ToUInt32(SGtMod1);

                        // GtMod2
                        String^ SGtMod2 = list_properties->Items[5]->SubItems[1]->Text;
                        gametype.GtMod2 = Convert::ToUInt32(SGtMod2);
                    }
                    // GtMod3
                    String^ SGtMod3 = list_properties->Items[6]->SubItems[1]->Text;
                    gametype.GtMod3 = Convert::ToUInt32(SGtMod3);

                    // GtMod4
                    String^ SGtMod4 = list_properties->Items[7]->SubItems[1]->Text;
                    gametype.GtMod4 = Convert::ToUInt32(SGtMod4);

                    // GtMod5
                    String^ SGtMod5 = list_properties->Items[8]->SubItems[1]->Text;
                    gametype.GtMod5 = Convert::ToUInt32(SGtMod5);

                    // Modify gametype data here
                    DWORD OldCRC = gametype.NewCRC;

                    // Calculate the size of the struct
                    size_t structSize = sizeof(Gametype);

                    // Exclude the NewCRC field
                    size_t byteBufferLength = structSize - sizeof(gametype.NewCRC);

                    // Create a byte buffer using std::vector
                    std::vector<BYTE> byteBuffer(byteBufferLength);

                    // Copy the data from gametype to the byte buffer
                    std::memcpy(byteBuffer.data(), &gametype, byteBufferLength);

                    std::string_view str = byteBufferToStringView(byteBuffer);
                    uint32_t NewCRC = GetCRC(str);
                    gametype.NewCRC = NewCRC;

                    std::cout << "Old CRC: " << std::hex << OldCRC << std::dec << " (" << OldCRC << ")" << std::endl;
                    std::cout << "New CRC: " << std::hex << NewCRC << std::dec << " (" << NewCRC << ")" << std::endl;

                    if (OldCRC != NewCRC || tab_file->SelectedIndex == 2) {
                        // Backup routine
                        if (tab_file->SelectedIndex == 1) { // save
                            std::string sBackupFilePath = filePath + ".bak";
                            if (iBackupIndex == 1) {
                                // Check if the source file exists
                                namespace fs = std::filesystem;
                                if (!fs::exists(sBackupFilePath.c_str())) {
                                    if (std::rename(filePath.c_str(), sBackupFilePath.c_str()) != 0) {
                                        std::cout << "Error backing up gametype." << std::endl;
                                    }
                                    else {
                                        std::cout << "Gametype backed up." << std::endl;
                                    }
                                }
                                else {
                                    std::cout << "Not backing up gametype first because a backup already exists." << std::endl;
                                }
                            }
                            else if (iBackupIndex == 2) { // back up every time
                                if (std::remove(sBackupFilePath.c_str()) != 0) {
                                    std::perror("Error deleting old backup file!");
                                }
                                else {
                                    std::cout << "Old backup file deleted successfully." << std::endl;
                                }
                                if (std::rename(filePath.c_str(), sBackupFilePath.c_str()) != 0) {
                                    std::cout << "Error backing up gametype." << std::endl;
                                }
                                else {
                                    std::cout << "Gametype backed up." << std::endl;
                                }
                            }
                        } else if (tab_file->SelectedIndex == 2) { 
                            // Create a new file for writing the updated data.
                            filePath = sNewFilePath;
                        }
                        std::ofstream outputFile(filePath, std::ios::binary);
                        if (!outputFile.is_open()) {
                            if (filePath != "") {
                                std::cerr << "Failed to open blam.lst for writing." << std::endl;
                                using namespace InfoBox;
                                CustomInfoBox^ infoBox = gcnew CustomInfoBox();
                                infoBox->StartPosition = FormStartPosition::CenterParent; // Center the dialog
                                infoBox->Owner = this; // Set the owner to your main form
                                using namespace System;
                                using namespace System::Collections::Generic;
                                using namespace System::Windows::Forms;
                                infoBox->SetTitle("Save gametype");
                                infoBox->SetMessage("Error saving gametype! Unable to access the file. It may be in use by another application, or you may not have permission to access it.");
                                infoBox->SetWidth(470);
                                infoBox->SetIcon("Error");
                                if (infoBox->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                                }
                            }
                            else {
                                std::cout << "No file selected." << std::endl;
                            }
                        }
                        else {
                            try {
                                // Open the registry key for reading
                                RegistryKey^ key = Registry::CurrentUser->OpenSubKey(sRegistryKey, true);

                                if (key != nullptr) {
                                    // Read the current DWORD value
                                    Object^ saveCountValue = key->GetValue("SaveCount");

                                    // Check if the value exists and is an integer
                                    if (saveCountValue != nullptr && saveCountValue->GetType() == int::typeid) {
                                        // Increment the value
                                        int currentCount = safe_cast<int>(saveCountValue);
                                        int newCount = currentCount + 1;

                                        // Save the updated value back to the registry
                                        key->SetValue("SaveCount", newCount, RegistryValueKind::DWord);
                                    }
                                    else {
                                        // If the value doesn't exist or is not an integer, set it to 1
                                        key->SetValue("SaveCount", 1, RegistryValueKind::DWord);
                                    }
                                }
                            }
                            catch (Exception^ ex) {
                                std::cerr << "Error updating SaveCount in registry." << std::endl;
                            }

                            // Write the updated Gametype objects to the new file.
                            for (const Gametype& updatedGametype : gametypes) {
                                outputFile.write(reinterpret_cast<const char*>(&updatedGametype), sizeof(Gametype));
                            }

                            outputFile.close();
                            picbox_notify->Image = nullptr;
                            label_warning->Text = "";
                            std::cout << "blam.lst has been saved with updated data." << std::endl;
                            using namespace InfoBox;
                            CustomInfoBox^ infoBox = gcnew CustomInfoBox();
                            infoBox->StartPosition = FormStartPosition::CenterParent; // Center the dialog
                            infoBox->Owner = this; // Set the owner to your main form
                            using namespace System;
                            using namespace System::Collections::Generic;
                            using namespace System::Windows::Forms;
                            infoBox->SetTitle("Save gametype");
                            infoBox->SetMessage("Gametype has been saved successfully!");
                            infoBox->SetWidth(297);
                            infoBox->SetIcon("Success");
                            if (infoBox->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                            }
                        }
                    } else {
                        std::cout << "Nothing saved because no changes have been made." << std::endl;
                        using namespace InfoBox;
                        CustomInfoBox^ infoBox = gcnew CustomInfoBox();
                        infoBox->StartPosition = FormStartPosition::CenterParent; // Center the dialog
                        infoBox->Owner = this; // Set the owner to your main form
                        using namespace System;
                        using namespace System::Collections::Generic;
                        using namespace System::Windows::Forms;
                        infoBox->SetTitle("Save gametype");
                        infoBox->SetMessage("Nothing saved because no changes have been made.");
                        infoBox->SetWidth(365);
                        infoBox->SetIcon("Information");
                        if (infoBox->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                        }
                    }
                    break;
                }
            }
        }

        tab_file->SelectedIndex = -1;
    }
    private: System::Void tab_file_mouseup(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
        tab_file->SelectedIndex = -1;
    }

	private: System::Void text_name_changed(System::Object^ sender, System::EventArgs^ e) {
        SetName(text_name->Text);
	}
    private: System::Void label_name_click(System::Object ^ sender, System::EventArgs ^ e) {
        text_name->Focus();
        text_name->DeselectAll();
        text_name->SelectionStart = text_name->Text->Length;
    }
    private: System::Void label_name_enterfocus(System::Object^ sender, System::EventArgs^ e) {
        timer_caret->Start();
    }
    private: System::Void label_name_leavefocus(System::Object^ sender, System::EventArgs^ e) {
        timer_caret->Stop();
        if (label_name->Text->EndsWith("|")) {
            label_name->Text = label_name->Text->Substring(0, label_name->Text->Length - 1);
        }
    }
	private: System::Void combo_gamemode_changed(System::Object^ sender, System::EventArgs^ e) {
        if (combo_gamemode->Text == "Slayer") {
            //combo_scorelimit->Text = "";
            combo_scorelimit->Items->Clear();
            combo_scorelimit->Items->Add("5 Kills");
            combo_scorelimit->Items->Add("10 Kills");
            combo_scorelimit->Items->Add("15 Kills");
            combo_scorelimit->Items->Add("25 Kills");
            combo_scorelimit->Items->Add("50 Kills");
            check_gtmod1->Text = "Death bonus";
            check_gtmod2->Text = "Kill penalty";
            check_gtmod3->Text = "Kill in order";
            check_gtmod4->Text = "Modifier 4";
            label_gtmod5->Text = "Modifier 5:";
            combo_gtmod5->Text = "";
            combo_gtmod5->Items->Clear();
            check_gtmod1->Enabled = true;
            check_gtmod2->Enabled = true;
            check_gtmod3->Enabled = true;
            check_gtmod4->Enabled = false;            
            label_gtmod5->Enabled = false;
            combo_gtmod5->Enabled = false;
            banner->Image = LoadPngResource(IDB_GT2);
        } else if (combo_gamemode->Text == "Capture The Flag") {
            //combo_scorelimit->Text = "";
            combo_scorelimit->Items->Clear();
            combo_scorelimit->Items->Add("1 Capture");
            combo_scorelimit->Items->Add("3 Captures");
            combo_scorelimit->Items->Add("5 Captures");
            combo_scorelimit->Items->Add("10 Captures");
            combo_scorelimit->Items->Add("15 Captures");
            check_gtmod1->Text = "Assault";
            check_gtmod2->Text = "Modifier 2";
            check_gtmod3->Text = "Flag must reset";
            check_gtmod4->Text = "Flag at home to score";
            label_gtmod5->Text = "Single flag:";
            combo_gtmod5->Items->Clear();
            combo_gtmod5->Items->Add("Off");
            combo_gtmod5->Items->Add("1 Minute");
            combo_gtmod5->Items->Add("2 Minutes");
            combo_gtmod5->Items->Add("3 Minutes");
            combo_gtmod5->Items->Add("5 Minutes");
            combo_gtmod5->Items->Add("10 Minutes");
            check_gtmod1->Enabled = true;
            check_gtmod2->Enabled = false;
            check_gtmod3->Enabled = true;
            check_gtmod4->Enabled = true;
            label_gtmod5->Enabled = true;
            combo_gtmod5->Enabled = true;
            banner->Image = LoadPngResource(IDB_GT1);
        } else if (combo_gamemode->Text == "King Of The Hill") {
            //combo_scorelimit->Text = "";
            combo_scorelimit->Items->Clear();
            combo_scorelimit->Items->Add("1 Minute");
            combo_scorelimit->Items->Add("2 Minutes");
            combo_scorelimit->Items->Add("3 Minutes");
            combo_scorelimit->Items->Add("5 Minutes");
            combo_scorelimit->Items->Add("10 Minutes");
            combo_scorelimit->Items->Add("15 Minutes");
            check_gtmod1->Text = "Moving hill";
            check_gtmod2->Text = "Modifier 2";
            check_gtmod3->Text = "Modifier 3";
            check_gtmod4->Text = "Modifier 4";
            label_gtmod5->Text = "Modifier 5:";
            combo_gtmod5->Text = "";
            combo_gtmod5->Items->Clear();
            check_gtmod1->Enabled = true;
            check_gtmod2->Enabled = false;
            check_gtmod3->Enabled = false;
            check_gtmod4->Enabled = false;
            label_gtmod5->Enabled = false;
            combo_gtmod5->Enabled = false;
            banner->Image = LoadPngResource(IDB_GT4);
        } else if (combo_gamemode->Text == "Oddball") {
            //combo_scorelimit->Text = "";
            combo_scorelimit->Items->Clear();
            combo_scorelimit->Items->Add("1 Minute");
            combo_scorelimit->Items->Add("2 Minutes");
            combo_scorelimit->Items->Add("3 Minutes");
            combo_scorelimit->Items->Add("5 Minutes");
            combo_scorelimit->Items->Add("10 Minutes");
            combo_scorelimit->Items->Add("15 Minutes");
            check_gtmod1->Text = "Modifier 1";
            check_gtmod2->Text = "Modifier 2";
            check_gtmod3->Text = "Modifier 3";
            check_gtmod4->Text = "Modifier 4";
            label_gtmod5->Text = "Speed with ball:";
            combo_gtmod5->Items->Clear();
            combo_gtmod5->Items->Add("Slow");
            combo_gtmod5->Items->Add("Normal");
            combo_gtmod5->Items->Add("Fast");
            check_gtmod1->Enabled = false;
            check_gtmod2->Enabled = false;
            check_gtmod3->Enabled = false;
            check_gtmod4->Enabled = false;
            label_gtmod5->Enabled = true;
            combo_gtmod5->Enabled = true;
            banner->Image = LoadPngResource(IDB_GT3);
        } else if (combo_gamemode->Text == "Race") {
            //combo_scorelimit->Text = "";
            combo_scorelimit->Items->Clear();
            combo_scorelimit->Items->Add("1 Lap");
            combo_scorelimit->Items->Add("3 Laps");
            combo_scorelimit->Items->Add("5 Laps");
            combo_scorelimit->Items->Add("10 Laps");
            combo_scorelimit->Items->Add("15 Laps");
            combo_scorelimit->Items->Add("25 Laps");
            check_gtmod1->Text = "Modifier 1";
            check_gtmod2->Text = "Modifier 2";
            check_gtmod3->Text = "Modifier 3";
            check_gtmod4->Text = "Modifier 4";
            label_gtmod5->Text = "Team scoring:";
            combo_gtmod5->Items->Clear();
            combo_gtmod5->Items->Add("Minimum");
            combo_gtmod5->Items->Add("Maximum");
            combo_gtmod5->Items->Add("Sum");
            check_gtmod1->Enabled = false;
            check_gtmod2->Enabled = false;
            check_gtmod3->Enabled = false;
            check_gtmod4->Enabled = false;
            label_gtmod5->Enabled = true;
            combo_gtmod5->Enabled = true;
            banner->Image = LoadPngResource(IDB_GT5);
        } else {
            //combo_scorelimit->Text = "";
            check_gtmod1->Text = "Modifier 1";
            check_gtmod2->Text = "Modifier 2";
            check_gtmod3->Text = "Modifier 3";
            check_gtmod4->Text = "Modifier 4";
            label_gtmod5->Text = "Modifier 5:";
            combo_gtmod5->Text = "";
            check_gtmod1->Enabled = false;
            check_gtmod2->Enabled = false;
            check_gtmod3->Enabled = false;
            check_gtmod4->Enabled = false;
            label_gtmod5->Enabled = false;
            combo_gtmod5->Enabled = false;
            banner->Image = LoadPngResource(IDB_GT0);
        }
        combo_scorelimit_changed(sender, e);
        SGtMode = combo_gamemode->Text;
        SetInt(0, combo_gamemode->SelectedIndex + 1);
	}


	// ----------------------------------------------------
	private: System::Void check_team_changed(System::Object^ sender, System::EventArgs^ e) {
        SetInt(8, check_teamplay->Checked);
        if (check_teamplay->Checked == 1) {
            group_redteam->Text = "Red team";
            group_blueteam->Text = "Blue team";
            //combo_blueteam_vehicleset->Text = "";
            combo_blueteam_vehicleset->Enabled = true;
            num_blueteam_warthog->Enabled = true;
            num_blueteam_ghost->Enabled = true;
            num_blueteam_scorpion->Enabled = true;
            num_blueteam_rockethog->Enabled = true;
            num_blueteam_banshee->Enabled = true;
            num_blueteam_gunturret->Enabled = true;
        }
        else {
            group_redteam->Text = "Free for all";
            group_blueteam->Text = "Unused";
            //combo_blueteam_vehicleset->Text = "";
            combo_blueteam_vehicleset->Enabled = false;
            num_blueteam_warthog->Enabled = false;
            num_blueteam_ghost->Enabled = false;
            num_blueteam_scorpion->Enabled = false;
            num_blueteam_rockethog->Enabled = false;
            num_blueteam_banshee->Enabled = false;
            num_blueteam_gunturret->Enabled = false;
        }
	}
	private: System::Void check_oddmanout_changed(System::Object^ sender, System::EventArgs^ e) {
        SetInt(17, check_oddmanout->Checked);
	}

	// ----------------------------------------------------
    private: System::Void button_open_click(System::Object^ sender, System::EventArgs^ e) {
        tab_file->SelectedIndex = -1;
    }

private: System::Void groupBox1_Enter(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void MainForm_Load(System::Object^ sender, System::EventArgs^ e) {
    if (bDebug) {
        ShowConsole();
    } else {
        HideConsole();
    }
    tab_file->SelectedIndex = -1;
    
    UpdateBanner();
    label_info->Text = "HaloCE Gametype Editor\nv" + String::Format("{0:F1}", fCurrentVersion) + "\n\nmade by noctilucent";

    if (!bUpdatesThread) {
        bUpdatesThread = true;
        timer_updatecheck->Start();
    }

    System::String^ SFontPath = SaveResource("Interstate-Bold.ttf", sFontInterstateBold);
    System::String^ SBarLeftPath = SaveResource("bar_left.gif", sBarLeft);
    System::String^ SBarRightPath = SaveResource("bar_right.gif", sBarRight);

    if (SFontPath != "") {
        pfc->AddFontFile(SFontPath);
    }

    if (pfc->Families->Length > 0) {
        if (label_name->Font->FontFamily->Name != "Interstate-Bold") {
            label_name->Font = gcnew Drawing::Font(pfc->Families[0], 16, FontStyle::Regular);
        }
        if (label_info->Font->FontFamily->Name != "Interstate-Bold") {
            label_info->Font = gcnew Drawing::Font(pfc->Families[0], 20, FontStyle::Regular);
        }
        if (label_github->Font->FontFamily->Name != "Interstate-Bold") {
            label_github->Font = gcnew Drawing::Font(pfc->Families[0], 12, FontStyle::Regular);
        }
    }
}
    private: void CheckForUpdateResponse() {
        if (!bUpdatesChecked)
        {
            std::string UpdateString = UpdateCheck();
            if (UpdateString.size() > 1 && UpdateString.find("Error! ") != std::string::npos) {
                label_version->Text = "Status: " + ConvertStdStringToSystemString(UpdateString.substr(7)) + "\nCurrent version: " + String::Format("{0:F1}", fCurrentVersion) + "\nLatest version: ";
                label_github->Image = LoadPngResource(IDB_IG);
            }
            else {
                if (UpdateString.size() > 1) {
                    fLatestVersion = std::stof(UpdateString);
                }
                if (fLatestVersion > fCurrentVersion) {
                    label_version->Text = "Status: Update available!\nCurrent version: " + String::Format("{0:F1}", fCurrentVersion) + "\nLatest version: " + String::Format("{0:F1}", fLatestVersion);
                    label_github->Text = "      Update Available!";
                    label_github->Image = LoadPngResource(IDB_IGU);
                    bUpdateAvailable = true;
                }
                else {
                    label_version->Text = "Status: Up to date\nCurrent version: " + String::Format("{0:F1}", fCurrentVersion) + "\nLatest version: " + String::Format("{0:F1}", fLatestVersion);
                    label_github->Image = LoadPngResource(IDB_IG);
                }
                bGitHubReachable = true;
                // if (GetLatestCommitHash() == sStyleSheetCommit) { }
                std::string sCSS = GetStyleSheet();
                std::ofstream outFile(GetTempFilePath() + "\\style.css");
                if (outFile.is_open()) {
                    outFile << sCSS;
                    outFile.close();
                    std::cout << "Saved updated style sheet." << std::endl;
                }
                else {
                    std::cerr << "Error saving updated style sheet!" << std::endl;
                }
            }

            //String^ LatestVersion = ConvertStdStringToSystemString(UpdateCheck());
            //label_version->Text = ConvertStdStringToSystemString(UpdateCheck());
            bUpdatesChecked = true;
        }
    }
private: System::Void tab_main_indexchanged(System::Object^ sender, System::EventArgs^ e) {
    UpdateBanner();
    switch (tab_main->SelectedIndex) {
    case 0:
        label_name->Visible = true;
        timer_browser->Stop();
        break;
    case 1: {
        label_name->Visible = true;
        timer_browser->Start();
        if (!bBrowserLoaded) {
            if (bGitHubReachable) {
                iDetailsPage = 1337;
                bBrowserReady = true;
            }
            else {
                // Access the embedded resource
                System::IO::Stream^ htmlStream = System::Reflection::Assembly::GetExecutingAssembly()->GetManifestResourceStream("details__.html");

                // Check if the stream is not null
                if (htmlStream != nullptr) {
                    // Read the content of the stream
                    StreamReader^ reader = gcnew StreamReader(htmlStream);
                    String^ ShtmlContent = reader->ReadToEnd();

                    std::string shtmlContent = ConvertSystemStringToStdString(ShtmlContent);
                    size_t pos = 0;
                    const std::string oldString = "style.css";
                    const std::string newString = GetTempFilePath() + "\\style.css";
                    while ((pos = shtmlContent.find(oldString, pos)) != std::string::npos) {
                        shtmlContent.replace(pos, oldString.length(), newString);
                        pos += newString.length();
                    }
                    // Load the HTML content into the WebBrowser control
                    browser_details->DocumentText = ConvertStdStringToSystemString(shtmlContent);
                }
                else {
                    // Handle the case where the resource stream is null
                    std::cout << "Failed to load embedded HTML resource." << std::endl;
                }
            }
            bBrowserLoaded = true;
        }
        break;
    }
    case 2:
        label_name->Visible = false;
        CheckForUpdateResponse();
        timer_browser->Stop();
        break;
    }
}
private: System::Void check_gtmod1_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(3, check_gtmod1->Checked);
}
private: System::Void check_gtmod2_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(4, check_gtmod2->Checked);
}
private: System::Void check_gtmod3_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(5, check_gtmod3->Checked);
}
private: System::Void check_gtmod4_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(6, check_gtmod4->Checked);
}
private: System::Void list_properties_indexchanged(System::Object^ sender, System::EventArgs^ e) {
    using namespace System;
    using namespace System::Collections::Generic;
    using namespace System::Windows::Forms;
    // Create a list to store the selected indices
    List<int>^ selectedIndices = gcnew List<int>();

    // Iterate through the selected indices and add them to the list
    for each (int index in list_properties->SelectedIndices) {
        // Make sure the index is within the valid range
        if (index >= 0 && index < list_properties->Items->Count) {
            // Set focus to the second column of the selected row
            iDetailsPage = index;
            if (bGitHubReachable) {
                bBrowserReady = true;
            }
            else {
                // Access the embedded resource
                System::IO::Stream^ htmlStream = System::Reflection::Assembly::GetExecutingAssembly()->GetManifestResourceStream("details_" + index + ".html");

                // Check if the stream is not null
                if (htmlStream != nullptr) {
                    // Read the content of the stream
                    StreamReader^ reader = gcnew StreamReader(htmlStream);
                    String^ ShtmlContent = reader->ReadToEnd();

                    std::string shtmlContent = ConvertSystemStringToStdString(ShtmlContent);
                    size_t pos = 0;
                    const std::string oldString = "style.css";
                    const std::string newString = GetTempFilePath() + "\\style.css";
                    while ((pos = shtmlContent.find(oldString, pos)) != std::string::npos) {
                        shtmlContent.replace(pos, oldString.length(), newString);
                        pos += newString.length();
                    }
                    // Load the HTML content into the WebBrowser control
                    browser_details->DocumentText = ConvertStdStringToSystemString(shtmlContent);
                }
                else {
                    // Handle the case where the resource stream is null
                    MessageBox::Show("Failed to load embedded HTML resource.", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
                }
            }
        }
        break;
    }
}
private: System::Void list_properties_doubleclick(System::Object^ sender, System::EventArgs^ e) {
    using namespace InputForm;
    CustomInputDialogForm^ inputDialog = gcnew CustomInputDialogForm();

    inputDialog->StartPosition = FormStartPosition::CenterParent; // Center the dialog
    inputDialog->Owner = this; // Set the owner to your main form
    // Create a list to store the selected indices
    using namespace System;
    using namespace System::Collections::Generic;
    using namespace System::Windows::Forms;
    List<int>^ selectedIndices = gcnew List<int>();
    for each (int index in list_properties->SelectedIndices) {
        inputDialog->SetTitle("Update property: " + list_properties->Items[index]->SubItems[0]->Text);
        inputDialog->SetPrompt(list_properties->Items[index]->SubItems[3]->Text);
        inputDialog->SetInputValue(list_properties->Items[index]->SubItems[1]->Text);
        break;
    }   

    if (inputDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
        String^ newValue = inputDialog->InputValue;

        // Do something with the entered value
        if (!String::IsNullOrEmpty(newValue)) {
            // Create a list to store the selected indices
            List<int>^ selectedIndices = gcnew List<int>();
            
            // Iterate through the selected indices and add them to the list
            for each (int index in list_properties->SelectedIndices) {
                // Make sure the index is within the valid range
                if (index >= 0 && index < list_properties->Items->Count) {
                    if (list_properties->Items->Count > index) {
                        Gametype g = {};
                        switch (index) {
                        case 0:
                            SetName(newValue);
                            break;
                        case 1: {
                            std::string sGameMode;
                            for (int i = 0; i < newValue->Length; i++) {
                                sGameMode += (char)newValue[i];
                            }

                            SetInt(0, std::stoi(sGameMode));
                            break;
                        }
                        case 2: {
                            std::string sScoreLimit;
                            for (int i = 0; i < newValue->Length; i++) {
                                sScoreLimit += (char)newValue[i];
                            }

                            SetInt(1, std::stoi(sScoreLimit));
                            break;
                        }
                        case 3: {
                            std::string sTimeLimit;
                            for (int i = 0; i < newValue->Length; i++) {
                                sTimeLimit += (char)newValue[i];
                            }

                            SetInt(2, std::stoi(sTimeLimit));
                            break;
                        }
                        case 4: {
                            std::string sModifier1;
                            for (int i = 0; i < newValue->Length; i++) {
                                sModifier1 += (char)newValue[i];
                            }

                            SetInt(3, std::stoi(sModifier1));
                            break;
                        }
                        case 5: {
                            std::string sModifier2;
                            for (int i = 0; i < newValue->Length; i++) {
                                sModifier2 += (char)newValue[i];
                            }

                            SetInt(4, std::stoi(sModifier2));
                            break;
                        }
                        case 6: {
                            std::string sModifier3;
                            for (int i = 0; i < newValue->Length; i++) {
                                sModifier3 += (char)newValue[i];
                            }

                            SetInt(5, std::stoi(sModifier3));
                            break;
                        }
                        case 7: {
                            std::string sModifier4;
                            for (int i = 0; i < newValue->Length; i++) {
                                sModifier4 += (char)newValue[i];
                            }

                            SetInt(6, std::stoi(sModifier4));
                            break;
                        }
                        case 8: {
                            std::string sModifier5;
                            for (int i = 0; i < newValue->Length; i++) {
                                sModifier5 += (char)newValue[i];
                            }

                            SetInt(7, std::stoi(sModifier5));
                            break;
                        }
                        case 9: {
                            std::string sTeamPlay;
                            for (int i = 0; i < newValue->Length; i++) {
                                sTeamPlay += (char)newValue[i];
                            }

                            SetInt(8, std::stoi(sTeamPlay));
                            break;
                        }
                        case 10: {
                            std::string sAutoTeamBalance;
                            for (int i = 0; i < newValue->Length; i++) {
                                sAutoTeamBalance += (char)newValue[i];
                            }

                            SetInt(9, std::stoi(sAutoTeamBalance));
                            break;
                        }
                        case 11: {
                            std::string sFriendlyFire;
                            for (int i = 0; i < newValue->Length; i++) {
                                sFriendlyFire += (char)newValue[i];
                            }

                            SetInt(10, std::stoi(sFriendlyFire));
                            break;
                        }
                        case 12: {
                            std::string sFriendlyFirePenalty;
                            for (int i = 0; i < newValue->Length; i++) {
                                sFriendlyFirePenalty += (char)newValue[i];
                            }

                            SetInt(11, std::stoi(sFriendlyFirePenalty));
                            break;
                        }
                        case 13: {
                            std::string sNumberOfLives;
                            for (int i = 0; i < newValue->Length; i++) {
                                sNumberOfLives += (char)newValue[i];
                            }

                            SetInt(12, std::stoi(sNumberOfLives));
                            break;
                        }
                        case 14: {
                            std::string sRespawnTime;
                            for (int i = 0; i < newValue->Length; i++) {
                                sRespawnTime += (char)newValue[i];
                            }

                            SetInt(13, std::stoi(sRespawnTime));
                            break;
                        }
                        case 15: {
                            std::string sRespawnTimeGrowth;
                            for (int i = 0; i < newValue->Length; i++) {
                                sRespawnTimeGrowth += (char)newValue[i];
                            }

                            SetInt(14, std::stoi(sRespawnTimeGrowth));
                            break;
                        }
                        case 16: {
                            std::string sSuicidePenalty;
                            for (int i = 0; i < newValue->Length; i++) {
                                sSuicidePenalty += (char)newValue[i];
                            }

                            SetInt(15, std::stoi(sSuicidePenalty));
                            break;
                        }
                        case 17: {
                            std::string sHealth;
                            for (int i = 0; i < newValue->Length; i++) {
                                sHealth += (char)newValue[i];
                            }

                            SetFloat(0, sHealth);
                            break;
                        }
                        case 18: {
                            std::string sShields;
                            for (int i = 0; i < newValue->Length; i++) {
                                sShields += (char)newValue[i];
                            }

                            SetInt(16, std::stoi(sShields));
                            break;
                        }
                        case 19: {
                            std::string sOddManOut;
                            for (int i = 0; i < newValue->Length; i++) {
                                sOddManOut += (char)newValue[i];
                            }

                            SetInt(17, std::stoi(sOddManOut));
                            break;
                        }
                        case 20: {
                            std::string sInvisible;
                            for (int i = 0; i < newValue->Length; i++) {
                                sInvisible += (char)newValue[i];
                            }

                            SetInt(18, std::stoi(sInvisible));
                            break;
                        }
                        case 21: {
                            std::string sWeaponSet;
                            for (int i = 0; i < newValue->Length; i++) {
                                sWeaponSet += (char)newValue[i];
                            }

                            SetInt(19, std::stoi(sWeaponSet));
                            break;
                        }
                        case 22: {
                            std::string sStartingEquipment;
                            for (int i = 0; i < newValue->Length; i++) {
                                sStartingEquipment += (char)newValue[i];
                            }

                            SetInt(20, std::stoi(sStartingEquipment));
                            break;
                        }
                        case 23: {
                            std::string sInfiniteGrenades;
                            for (int i = 0; i < newValue->Length; i++) {
                                sInfiniteGrenades += (char)newValue[i];
                            }

                            SetInt(21, std::stoi(sInfiniteGrenades));
                            break;
                        }
                        case 24: {
                            std::string sVehicleRespawnTime;
                            for (int i = 0; i < newValue->Length; i++) {
                                sVehicleRespawnTime += (char)newValue[i];
                            }

                            SetInt(22, std::stoi(sVehicleRespawnTime));
                            break;
                        }
                        case 25: {
                            std::string sTeam0VehicleSet;
                            for (int i = 0; i < newValue->Length; i++) {
                                sTeam0VehicleSet += (char)newValue[i];
                            }

                            SetInt(23, std::stoi(sTeam0VehicleSet));
                            break;
                        }
                        case 26: {
                            std::string sTeam0Warthogs;
                            for (int i = 0; i < newValue->Length; i++) {
                                sTeam0Warthogs += (char)newValue[i];
                            }

                            SetInt(24, std::stoi(sTeam0Warthogs));
                            break;
                        }
                        case 27: {
                            std::string sTeam0Ghosts;
                            for (int i = 0; i < newValue->Length; i++) {
                                sTeam0Ghosts += (char)newValue[i];
                            }

                            SetInt(25, std::stoi(sTeam0Ghosts));
                            break;
                        }
                        case 28: {
                            std::string sTeam0Scorpions;
                            for (int i = 0; i < newValue->Length; i++) {
                                sTeam0Scorpions += (char)newValue[i];
                            }

                            SetInt(26, std::stoi(sTeam0Scorpions));
                            break;
                        }
                        case 29: {
                            std::string sTeam0RocketHogs;
                            for (int i = 0; i < newValue->Length; i++) {
                                sTeam0RocketHogs += (char)newValue[i];
                            }

                            SetInt(27, std::stoi(sTeam0RocketHogs));
                            break;
                        }
                        case 30: {
                            std::string sTeam0Banshees;
                            for (int i = 0; i < newValue->Length; i++) {
                                sTeam0Banshees += (char)newValue[i];
                            }

                            SetInt(28, std::stoi(sTeam0Banshees));
                            break;
                        }
                        case 31: {
                            std::string sTeam0GunTurrets;
                            for (int i = 0; i < newValue->Length; i++) {
                                sTeam0GunTurrets += (char)newValue[i];
                            }

                            SetInt(29, std::stoi(sTeam0GunTurrets));
                            break;
                        }
                        case 32: {
                            std::string sTeam1VehicleSet;
                            for (int i = 0; i < newValue->Length; i++) {
                                sTeam1VehicleSet += (char)newValue[i];
                            }

                            SetInt(30, std::stoi(sTeam1VehicleSet));
                            break;
                        }
                        case 33: {
                            std::string sTeam1Warthogs;
                            for (int i = 0; i < newValue->Length; i++) {
                                sTeam1Warthogs += (char)newValue[i];
                            }

                            SetInt(31, std::stoi(sTeam1Warthogs));
                            break;
                        }
                        case 34: {
                            std::string sTeam1Ghosts;
                            for (int i = 0; i < newValue->Length; i++) {
                                sTeam1Ghosts += (char)newValue[i];
                            }

                            SetInt(32, std::stoi(sTeam1Ghosts));
                            break;
                        }
                        case 35: {
                            std::string sTeam1Scorpions;
                            for (int i = 0; i < newValue->Length; i++) {
                                sTeam1Scorpions += (char)newValue[i];
                            }

                            SetInt(33, std::stoi(sTeam1Scorpions));
                            break;
                        }
                        case 36: {
                            std::string sTeam1RocketHogs;
                            for (int i = 0; i < newValue->Length; i++) {
                                sTeam1RocketHogs += (char)newValue[i];
                            }

                            SetInt(34, std::stoi(sTeam1RocketHogs));
                            break;
                        }
                        case 37: {
                            std::string sTeam1Banshees;
                            for (int i = 0; i < newValue->Length; i++) {
                                sTeam1Banshees += (char)newValue[i];
                            }

                            SetInt(35, std::stoi(sTeam1Banshees));
                            break;
                        }
                        case 38: {
                            std::string sTeam1GunTurrets;
                            for (int i = 0; i < newValue->Length; i++) {
                                sTeam1GunTurrets += (char)newValue[i];
                            }

                            SetInt(36, std::stoi(sTeam1GunTurrets));
                            break;
                        }
                        case 39: {
                            std::string sObjectivesIndicator;
                            for (int i = 0; i < newValue->Length; i++) {
                                sObjectivesIndicator += (char)newValue[i];
                            }

                            SetInt(37, std::stoi(sObjectivesIndicator));
                            break;
                        }
                        case 40: {
                            std::string sMotionTracker;
                            for (int i = 0; i < newValue->Length; i++) {
                                sMotionTracker += (char)newValue[i];
                            }

                            SetInt(38, std::stoi(sMotionTracker));
                            break;
                        }
                        case 41: {
                            std::string sPlayersOnTracker;
                            for (int i = 0; i < newValue->Length; i++) {
                                sPlayersOnTracker += (char)newValue[i];
                            }

                            SetInt(39, std::stoi(sPlayersOnTracker));
                            break;
                        }
                        case 42: {
                            std::string sFriendIndicators;
                            for (int i = 0; i < newValue->Length; i++) {
                                sFriendIndicators += (char)newValue[i];
                            }

                            SetInt(40, std::stoi(sFriendIndicators));
                            break;
                        }
                        }
                    }
                }
                break;
            }
        }
    }
}
       
private: System::Void combo_theme_indexchanged(System::Object^ sender, System::EventArgs^ e) {
    iUserThemeIndex = combo_theme->SelectedIndex;
    UpdateBanner();
    try {
        // Open the registry key for writing
        RegistryKey^ key = Registry::CurrentUser->OpenSubKey(sRegistryKey, true);

        if (key != nullptr) {
            // Set the new value of iUserThemeIndex as a DWORD
            key->SetValue("UserThemeIndex", iUserThemeIndex, RegistryValueKind::DWord);
            std::cout << "Succesfully updated iUserThemeIndex in registry key: " << iUserThemeIndex << std::endl;

            // Close the key when you're done with it
            key->Close();
        }
    }
    catch (Exception^ ex) {
        // Handle any exceptions that may occur during registry access
        std::cerr << "Exception updating iUserThemeIndex in registry key." << std::endl;
    }
}
private: System::Void list_properties_keydown(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e) {
    if (e->KeyCode == Keys::Enter) {
        list_properties_doubleclick(sender, e);
    }
}
private: System::Void combo_scorelimit_changed(System::Object^ sender, System::EventArgs^ e) {
    if (combo_gamemode->Text == "Capture The Flag") {
        switch (combo_scorelimit->SelectedIndex) {
        case 0:
            SetInt(1, 1);
            break;
        case 1:
            SetInt(1, 3);
            break;
        case 2:
            SetInt(1, 5);
            break;
        case 3:
            SetInt(1, 10);
            break;
        case 4:
            SetInt(1, 15);
            break;
        }
    } else if(combo_gamemode->Text == "Slayer") {
        switch (combo_scorelimit->SelectedIndex) {
        case 0:
            SetInt(1, 5);
            break;
        case 1:
            SetInt(1, 10);
            break;
        case 2:
            SetInt(1, 15);
            break;
        case 3:
            SetInt(1, 25);
            break;
        case 4:
            SetInt(1, 50);
            break;
        }
    }
    else if (combo_gamemode->Text == "Oddball" || combo_gamemode->Text == "King Of The Hill") {
        switch (combo_scorelimit->SelectedIndex) {
        case 0:
            SetInt(1, 1);
            break;
        case 1:
            SetInt(1, 2);
            break;
        case 2:
            SetInt(1, 3);
            break;
        case 3:
            SetInt(1, 5);
            break;
        case 4:
            SetInt(1, 10);
            break;
        case 5:
            SetInt(1, 15);
            break;
        }
    }
    else if (combo_gamemode->Text == "Race"){
        switch (combo_scorelimit->SelectedIndex) {
        case 0:
            SetInt(1, 1);
            break;
        case 1:
            SetInt(1, 3);
            break;
        case 2:
            SetInt(1, 5);
            break;
        case 3:
            SetInt(1, 10);
            break;
        case 4:
            SetInt(1, 15);
            break;
        case 5:
            SetInt(1, 25);
            break;
        }
    }
}
private: System::Void combo_timelimit_changed(System::Object^ sender, System::EventArgs^ e) {
    switch (combo_timelimit->SelectedIndex) {
    case 0:
        SetInt(2, 0);
        break;
    case 1:
        SetInt(2, 18000);
        break;
    case 2:
        SetInt(2, 27000);
        break;
    case 3:
        SetInt(2, 36000);
        break;
    case 4:
        SetInt(2, 45000);
        break;
    case 5:
        SetInt(2, 54000);
        break;
    case 6:
        SetInt(2, 81000);
        break;
    }
}
private: System::Void combo_gtmod5_changed(System::Object^ sender, System::EventArgs^ e) {
    if (combo_gamemode->Text == "Capture The Flag") {
        switch (combo_gtmod5->SelectedIndex) {
        case 0:
            SetInt(7, 0);
            break;
        case 1:
            SetInt(7, 1800);
            break;
        case 2:
            SetInt(7, 3600);
            break;
        case 3:
            SetInt(7, 5400);
            break;
        case 4:
            SetInt(7, 9000);
            break;
        case 5:
            SetInt(7, 18000);
            break;;
        }
    }
    else if (combo_gamemode->Text == "Oddball" || combo_gamemode->Text == "Race") {
        switch (combo_gtmod5->SelectedIndex) {
        case 0:
            SetInt(7, 0);
            break;
        case 1:
            SetInt(7, 1);
            break;
        case 2:
            SetInt(7, 2);
            break;
        }
    }
    else {
        SetInt(7, 0);
    }
}
private: System::Void check_autoteambalance_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(9, check_teambalance->Checked);
}
private: System::Void combo_friendlyfire_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(10, combo_friendlyfire->SelectedIndex);
}
private: System::Void combo_friendlyfirepenalty_changed(System::Object^ sender, System::EventArgs^ e) {
    int ifriendlyfirepenalty_ticks;
    switch (combo_friendlyfirepenalty->SelectedIndex) {
    case 0:
        ifriendlyfirepenalty_ticks = 0;
        break;
    case 1:
        ifriendlyfirepenalty_ticks = 150;
        break;
    case 2:
        ifriendlyfirepenalty_ticks = 300;
        break;
    case 3:
        ifriendlyfirepenalty_ticks = 450;
        break;
    }

    SetInt(11, ifriendlyfirepenalty_ticks);
}
private: System::Void combo_lives_changed(System::Object^ sender, System::EventArgs^ e) {
    int iLives;
    switch (combo_lives->SelectedIndex) {
    case 0:
        iLives = 0;
        break;
    case 1:
        iLives = 1;
        break;
    case 2:
        iLives = 3;
        break;
    case 3:
        iLives = 5;
        break;
    }
    SetInt(12, iLives);
}
private: System::Void combo_respawntime_changed(System::Object^ sender, System::EventArgs^ e) {
    int iRespawnTimeTicks;
    switch (combo_respawntime->SelectedIndex) {
    case 0:
        iRespawnTimeTicks = 0;
        break;
    case 1:
        iRespawnTimeTicks = 150;
        break;
    case 2:
        iRespawnTimeTicks = 300;
        break;
    case 3:
        iRespawnTimeTicks = 450;
        break;
    }
    SetInt(13, iRespawnTimeTicks);
}
private: System::Void combo_respawntimegrowth_changed(System::Object^ sender, System::EventArgs^ e) {
    int iRespawnTimeGrowthTicks;
    switch (combo_respawntimegrowth->SelectedIndex) {
    case 0:
        iRespawnTimeGrowthTicks = 0;
        break;
    case 1:
        iRespawnTimeGrowthTicks = 150;
        break;
    case 2:
        iRespawnTimeGrowthTicks = 300;
        break;
    case 3:
        iRespawnTimeGrowthTicks = 450;
        break;
    }
    SetInt(14, iRespawnTimeGrowthTicks);
}
private: System::Void combo_suicidepenalty_changed(System::Object^ sender, System::EventArgs^ e) {
    int iSuicidePenalty;
    switch (combo_suicidepenalty->SelectedIndex) {
    case 0:
        iSuicidePenalty = 0;
        break;
    case 1:
        iSuicidePenalty = 150;
        break;
    case 2:
        iSuicidePenalty = 300;
        break;
    case 3:
        iSuicidePenalty = 450;
        break;
    }
    SetInt(15, iSuicidePenalty);
}
private: System::Void combo_health_changed(System::Object^ sender, System::EventArgs^ e) {
    float fHealth;
    switch (combo_health->SelectedIndex) {
    case 0:
        fHealth = 0.5;
        break;
    case 1:
        fHealth = 1.0;
        break;
    case 2:
        fHealth = 1.5;
        break;
    case 3:
        fHealth = 2.0;
        break;
    case 4:
        fHealth = 3.0;
        break;
    case 5:
        fHealth = 4.0;
        break;
    }
    SetFloat(0, std::to_string(fHealth));
}
private: System::Void check_shields_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(16, check_shields->Checked);
}
private: System::Void check_invisible_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(18, check_invisible->Checked);
}
private: System::Void combo_weaponset_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(19, combo_weaponset->SelectedIndex);
}
private: System::Void combo_startingequipment_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(20, combo_startingequipment->SelectedIndex);
}
private: System::Void check_infgrenades_change(System::Object^ sender, System::EventArgs^ e) {
    SetInt(21, check_infgrenades->Checked);
}
private: System::Void combo_vehiclerespawntime_changed(System::Object^ sender, System::EventArgs^ e) {
    int iVehicleRespawnTime;
    switch (combo_vehiclerespawntime->SelectedIndex) {
    case 0:
        iVehicleRespawnTime = 0;
        break;
    case 1:
        iVehicleRespawnTime = 900;
        break;
    case 2:
        iVehicleRespawnTime = 1800;
        break;
    case 3:
        iVehicleRespawnTime = 2700;
        break;
    case 4:
        iVehicleRespawnTime = 3600;
        break;
    case 5:
        iVehicleRespawnTime = 5400;
        break;
    case 6:
        iVehicleRespawnTime = 9000;
        break;
    }
    SetInt(22, iVehicleRespawnTime);
}
private: System::Void combo_redteam_vehicleset_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(23, combo_redteam_vehicleset->SelectedIndex);
}
private: System::Void num_redteam_warthog_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(24, System::Decimal::ToInt32(num_redteam_warthog->Value));
}
private: System::Void num_redteam_ghost_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(25, System::Decimal::ToInt32(num_redteam_ghost->Value));
}
private: System::Void num_redteam_scorpion_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(26, System::Decimal::ToInt32(num_redteam_scorpion->Value));
}
private: System::Void num_redteam_rockethog_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(27, System::Decimal::ToInt32(num_redteam_rockethog->Value));
}
private: System::Void num_redteam_banshee_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(28, System::Decimal::ToInt32(num_redteam_banshee->Value));
}
private: System::Void num_redteam_gunturret_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(29, System::Decimal::ToInt32(num_redteam_gunturret->Value));
}
private: System::Void combo_blueteam_vehicleset_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(30, combo_blueteam_vehicleset->SelectedIndex);
}
private: System::Void num_blueteam_warthog_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(31, System::Decimal::ToInt32(num_blueteam_warthog->Value));
}
private: System::Void num_blueteam_ghost_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(32, System::Decimal::ToInt32(num_blueteam_ghost->Value));
}
private: System::Void num_blueteam_scorpion_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(33, System::Decimal::ToInt32(num_blueteam_scorpion->Value));
}
private: System::Void num_blueteam_rockethog_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(34, System::Decimal::ToInt32(num_blueteam_rockethog->Value));
}
private: System::Void num_blueteam_banshee_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(35, System::Decimal::ToInt32(num_blueteam_banshee->Value));
}
private: System::Void num_blueteam_gunturret_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(36, System::Decimal::ToInt32(num_blueteam_gunturret->Value));
}
private: System::Void combo_objectives_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(37, combo_objectives->SelectedIndex);
}
private: System::Void check_friendindicators_changed(System::Object^ sender, System::EventArgs^ e) {
    SetInt(40, check_friendindicators->Checked);
}
private: System::Void combo_radar_changed(System::Object^ sender, System::EventArgs^ e) {
    switch (combo_radar->SelectedIndex) {
    case 0:
        SetInt(39, 1);
        break;
    case 1:
        SetInt(39, 0);
        break;
    case 2:
        SetInt(38, 0);
        break;
    }
}
private: System::Void check_debug_changed(System::Object^ sender, System::EventArgs^ e) {
    bDebug = check_debug->Checked;

    if (bDebug) {
        if (!bDebugPrompted) {
            std::cout << "You must restart the application in order for the debug console to appear." << std::endl;
            using namespace InfoBox;
            CustomInfoBox^ infoBox = gcnew CustomInfoBox();
            infoBox->StartPosition = FormStartPosition::CenterParent; // Center the dialog
            infoBox->Owner = this; // Set the owner to your main form
            using namespace System;
            using namespace System::Collections::Generic;
            using namespace System::Windows::Forms;
            infoBox->SetTitle("Debug console");
            infoBox->SetMessage("The debug console has been enabled, it will appear next time the application starts.");
            infoBox->SetWidth(352);
            infoBox->SetIcon("Information");
            if (infoBox->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            }
            bDebugPrompted = true;
        }
        else {
            ShowConsole();
        }
    }
    else {
        if (bDebugPrompted) {
            std::cout << "Debug console has been disabled, the console will not open next time the application starts. :)" << std::endl;
            bDebugPrompted = false;
        }
    }

    try {
        // Open the registry key for writing
        RegistryKey^ key = Registry::CurrentUser->OpenSubKey(sRegistryKey, true);

        if (key != nullptr) {
            // Set the new value of iUserThemeIndex as a DWORD
            key->SetValue("ShowDebugConsole", bDebug, RegistryValueKind::DWord);
            std::cout << "Succesfully updated ShowDebugConsole in registry key: " << bDebug << std::endl;

            // Close the key when you're done with it
            key->Close();
        }
    }
    catch (Exception^ ex) {
        // Handle any exceptions that may occur during registry access
        std::cerr << "Exception updating ShowDebugConsole in registry key." << std::endl;
    }
}
private: System::Void combo_backup_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
    switch (combo_backup->SelectedIndex) {
    case 0:
        iBackupIndex = 1;
        break;
    case 1:
        iBackupIndex = 2;
        break;
    case 2:
        iBackupIndex = 0;
        break;
    }

    try {
        // Open the registry key for writing
        RegistryKey^ key = Registry::CurrentUser->OpenSubKey(sRegistryKey, true);

        if (key != nullptr) {
            // Set the new value of iUserThemeIndex as a DWORD
            key->SetValue("BackupSetting", iBackupIndex, RegistryValueKind::DWord);
            std::cout << "Succesfully updated BackupSetting in registry key: " << iBackupIndex << std::endl;

            // Close the key when you're done with it
            key->Close();
        }
    }
    catch (Exception^ ex) {
        // Handle any exceptions that may occur during registry access
        std::cerr << "Exception updating BackupSetting in registry key." << std::endl;
    }
}
private: System::Void check_updatenotify_changed(System::Object^ sender, System::EventArgs^ e) {
    bUpdateNotify = check_updatenotify->Checked;

    try {
        // Open the registry key for writing
        RegistryKey^ key = Registry::CurrentUser->OpenSubKey(sRegistryKey, true);

        if (key != nullptr) {
            // Set the new value of iUserThemeIndex as a DWORD
            key->SetValue("UpdateNotify", bUpdateNotify, RegistryValueKind::DWord);
            std::cout << "Succesfully updated UpdateNotify in registry key: " << bUpdateNotify << std::endl;

            // Close the key when you're done with it
            key->Close();
        }
    }
    catch (Exception^ ex) {
        // Handle any exceptions that may occur during registry access
        std::cerr << "Exception updating UpdateNotify in registry key." << std::endl;
    }
}
private: System::Void label_github_click(System::Object^ sender, System::EventArgs^ e) {
    if (bUpdateAvailable) {
        System::Diagnostics::Process::Start("https://github.com/nocti1/HaloCE-Gametype-Editor/releases/latest");
    }
    else {
        System::Diagnostics::Process::Start("https://github.com/nocti1/HaloCE-Gametype-Editor");
    }
}
private: System::Void label_github_mousehover(System::Object^ sender, System::EventArgs^ e) {
    label_github->BackColor = System::Drawing::Color::FromArgb(0xF7, 0xF7, 0xF7);
}
private: System::Void label_github_mouseleave(System::Object^ sender, System::EventArgs^ e) {
    label_github->BackColor = System::Drawing::Color::FromArgb(0xFF, 0xFF, 0xFF);
}
};
}
