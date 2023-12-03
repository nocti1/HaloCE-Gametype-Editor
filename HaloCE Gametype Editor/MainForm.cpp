#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#include <vector>
#include "Structs.h"
#include "Functions.h"
#include "MainForm.h"

using namespace System;
using namespace System::Windows::Forms;
[STAThread]

void main(array<String^>^ args)
{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    HaloCEGametypeEditor::MainForm form;
    Application::Run(% form);
}