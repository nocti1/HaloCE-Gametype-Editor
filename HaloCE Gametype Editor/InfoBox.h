#pragma once

namespace InfoBox {
    using namespace System;
    using namespace System::Windows::Forms;

    public ref class CustomInfoBox : public System::Windows::Forms::Form {
    private: System::Windows::Forms::Button^ infobox_button_ok;

    private: System::Windows::Forms::PictureBox^ infobox_picbox;

    private: System::Windows::Forms::Label^ infobox_label;

    public:
        CustomInfoBox() {
            InitializeComponent();
        }

        void SetTitle(String^ value) {
            this->Text = value;
        }

        void SetMessage(String^ value) {
            infobox_label->Text = value;
        }

        void SetWidth(int value) {
            this->Width = value;
            int iButtonPos = ((value / 2) - 38); 
            if (value > 370) {
                int iLabelWidth = (this->infobox_label->Width + (value - 370));
                this->infobox_label->Width = iLabelWidth;
            }
            this->infobox_button_ok->Location = System::Drawing::Point(iButtonPos, 64);
            //infobox_picbox
            // Get a system icon (e.g., information icon)
            System::Drawing::Icon^ infoIcon = System::Drawing::SystemIcons::Shield;
            System::Drawing::Bitmap^ originalBitmap = gcnew System::Drawing::Bitmap(infoIcon->ToBitmap());
            this->infobox_picbox->Image = originalBitmap;
        }

        void SetIcon(std::string sIcon) {
            using namespace System;
            using namespace System::Drawing;           
            if (sIcon == "Information") {
                System::Drawing::Icon^ infoIcon = System::Drawing::SystemIcons::Information;
                System::Drawing::Bitmap^ bitmap = gcnew System::Drawing::Bitmap(infoIcon->ToBitmap());
                this->infobox_picbox->Image = bitmap;
            } else if (sIcon == "Error") {
                System::Drawing::Icon^ infoIcon = System::Drawing::SystemIcons::Error;
                System::Drawing::Bitmap^ bitmap = gcnew System::Drawing::Bitmap(infoIcon->ToBitmap());
                this->infobox_picbox->Image = bitmap;
            } else if (sIcon == "Success") {
                this->infobox_picbox->Image = LoadPngResource(IDB_IS);
            } else if (sIcon == "Update") {
                this->infobox_picbox->Image = LoadPngResource(IDB_IU);
                System::Drawing::Point newLocation = this->infobox_picbox->Location;
                newLocation.X -= 1;
                newLocation.Y -= 1;
                this->infobox_picbox->Location = newLocation;
            }
        }

    private:
           System::Void inputform_button_ok_clicked(System::Object^ sender, System::EventArgs^ e) {
               this->DialogResult = System::Windows::Forms::DialogResult::OK;
               this->Close();
           }

           System::Void InitializeComponent() {
               this->infobox_button_ok = (gcnew System::Windows::Forms::Button());
               this->infobox_label = (gcnew System::Windows::Forms::Label());
               this->infobox_picbox = (gcnew System::Windows::Forms::PictureBox());
               (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->infobox_picbox))->BeginInit();
               this->SuspendLayout();
               // 
               // infobox_button_ok
               // 
               this->infobox_button_ok->Location = System::Drawing::Point(139, 64);
               this->infobox_button_ok->Name = L"infobox_button_ok";
               this->infobox_button_ok->Size = System::Drawing::Size(60, 23);
               this->infobox_button_ok->TabIndex = 0;
               this->infobox_button_ok->Text = L"OK";
               this->infobox_button_ok->UseVisualStyleBackColor = true;
               this->infobox_button_ok->Click += gcnew System::EventHandler(this, &CustomInfoBox::inputform_button_ok_clicked);
               this->infobox_button_ok->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &CustomInfoBox::infobox_button_keydown);
               // 
               // infobox_label
               // 
               this->infobox_label->Location = System::Drawing::Point(69, 25);
               this->infobox_label->Name = L"infobox_label";
               this->infobox_label->Size = System::Drawing::Size(270, 30);
               this->infobox_label->TabIndex = 2;
               this->infobox_label->Text = L"© noctilucent 2023";
               // 
               // infobox_picbox
               // 
               this->infobox_picbox->Location = System::Drawing::Point(20, 23);
               this->infobox_picbox->Name = L"infobox_picbox";
               this->infobox_picbox->Size = System::Drawing::Size(42, 42);
               this->infobox_picbox->TabIndex = 25;
               this->infobox_picbox->TabStop = false;
               // 
               // CustomInfoBox
               // 
               this->ClientSize = System::Drawing::Size(353, 101);
               this->Controls->Add(this->infobox_picbox);
               this->Controls->Add(this->infobox_label);
               this->Controls->Add(this->infobox_button_ok);
               this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
               this->MaximizeBox = false;
               this->MinimizeBox = false;
               this->Name = L"CustomInfoBox";
               this->ShowIcon = false;
               this->ShowInTaskbar = false;
               this->Text = L"Information";
               (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->infobox_picbox))->EndInit();
               this->ResumeLayout(false);

           }

    private: System::Void infobox_button_keydown(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e) {
        if (e->KeyCode == Keys::Enter || e->KeyCode == Keys::Escape) {
            this->DialogResult = System::Windows::Forms::DialogResult::OK;
            this->Close();
        }
    }
};
}