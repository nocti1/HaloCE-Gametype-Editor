#pragma once

namespace InputForm {
    using namespace System;
    using namespace System::Windows::Forms;

    public ref class CustomInputDialogForm : public System::Windows::Forms::Form {
    private: System::Windows::Forms::Button^ inputform_button_ok;
    private: System::Windows::Forms::Button^ inputform_button_close;
    private: System::Windows::Forms::Label^ inputform_label_prompt;
    private: System::Windows::Forms::TextBox^ inputform_text_value;

    public:
        CustomInputDialogForm() {
            InitializeComponent();
        }

        property String^ InputValue {
            String^ get() {
                return inputform_text_value->Text;
            }
        }

        void SetTitle(String^ value) {
            this->Text = value;
        }

        void SetPrompt(String^ value) {
            inputform_label_prompt->Text = value + ":";
        }

        void SetInputValue(String^ value) {
            inputform_text_value->Text = value;
        }

    private:
    private: System::Void inputform_text_value_keydown(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e) {
        if (e->KeyCode == Keys::Enter) {
            this->DialogResult = System::Windows::Forms::DialogResult::OK;
            this->Close();
        }
        else if (e->KeyCode == Keys::Escape) {
            this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            this->Close();
        }
    }

           System::Void inputform_button_ok_clicked(System::Object^ sender, System::EventArgs^ e) {
               this->DialogResult = System::Windows::Forms::DialogResult::OK;
               this->Close();
           }

           System::Void inputform_button_close_clicked(System::Object^ sender, System::EventArgs^ e) {
               this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
               this->Close();
           }

           System::Void InitializeComponent() {
               this->inputform_button_ok = (gcnew System::Windows::Forms::Button());
               this->inputform_button_close = (gcnew System::Windows::Forms::Button());
               this->inputform_label_prompt = (gcnew System::Windows::Forms::Label());
               this->inputform_text_value = (gcnew System::Windows::Forms::TextBox());
               this->SuspendLayout();
               // 
               // inputform_button_ok
               // 
               this->inputform_button_ok->Location = System::Drawing::Point(62, 69);
               this->inputform_button_ok->Name = L"inputform_button_ok";
               this->inputform_button_ok->Size = System::Drawing::Size(75, 23);
               this->inputform_button_ok->TabIndex = 0;
               this->inputform_button_ok->Text = L"OK";
               this->inputform_button_ok->UseVisualStyleBackColor = true;
               this->inputform_button_ok->Click += gcnew System::EventHandler(this, &CustomInputDialogForm::inputform_button_ok_clicked);
               // 
               // inputform_button_close
               // 
               this->inputform_button_close->Location = System::Drawing::Point(147, 69);
               this->inputform_button_close->Name = L"inputform_button_close";
               this->inputform_button_close->Size = System::Drawing::Size(75, 23);
               this->inputform_button_close->TabIndex = 1;
               this->inputform_button_close->Text = L"Cancel";
               this->inputform_button_close->UseVisualStyleBackColor = true;
               this->inputform_button_close->Click += gcnew System::EventHandler(this, &CustomInputDialogForm::inputform_button_close_clicked);
               // 
               // inputform_label_prompt
               // 
               this->inputform_label_prompt->AutoSize = true;
               this->inputform_label_prompt->Location = System::Drawing::Point(15, 15);
               this->inputform_label_prompt->Name = L"inputform_label_prompt";
               this->inputform_label_prompt->Size = System::Drawing::Size(43, 13);
               this->inputform_label_prompt->TabIndex = 2;
               this->inputform_label_prompt->Text = L"Prompt:";
               // 
               // inputform_text_value
               // 
               this->inputform_text_value->AcceptsReturn = true;
               this->inputform_text_value->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
               this->inputform_text_value->Location = System::Drawing::Point(63, 38);
               this->inputform_text_value->MaxLength = 23;
               this->inputform_text_value->Name = L"inputform_text_value";
               this->inputform_text_value->Size = System::Drawing::Size(158, 20);
               this->inputform_text_value->TabIndex = 0;
               this->inputform_text_value->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &CustomInputDialogForm::inputform_text_value_keydown);
               // 
               // CustomInputDialogForm
               // 
               this->ClientSize = System::Drawing::Size(284, 106);
               this->Controls->Add(this->inputform_text_value);
               this->Controls->Add(this->inputform_label_prompt);
               this->Controls->Add(this->inputform_button_close);
               this->Controls->Add(this->inputform_button_ok);
               this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
               this->MaximizeBox = false;
               this->MinimizeBox = false;
               this->Name = L"CustomInputDialogForm";
               this->ShowIcon = false;
               this->ShowInTaskbar = false;
               this->Text = L"Update property:";
               this->ResumeLayout(false);
               this->PerformLayout();

           }
    };
}
