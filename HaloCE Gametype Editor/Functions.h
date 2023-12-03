#pragma once
#include "SysIncludes.h"

using json = nlohmann::json;
typedef void (*CallbackFunction)(const std::string& jsonData);
std::string jsonData;
std::string sLastError;
bool bUpdated = false;

System::String^ ConvertStdStringToSystemString(const std::string& stdString) {
    array<wchar_t>^ chars = gcnew array<wchar_t>(stdString.length());
    for (int i = 0; i < stdString.length(); i++) {
        chars[i] = stdString[i];
    }
    return gcnew System::String(chars);
}

std::string ConvertSystemStringToStdString(System::String^ systemString) {
    // Check for null String
    if (systemString == nullptr)
        return "";

    // Use pin_ptr to get a direct pointer to the characters
    pin_ptr<const wchar_t> wch = PtrToStringChars(systemString);

    // Convert wchar_t to char
    size_t convertedChars = 0;
    size_t sizeInBytes = ((systemString->Length + 1) * 2);
    char* ch = new char[sizeInBytes];
    wcstombs_s(&convertedChars, ch, sizeInBytes, wch, sizeInBytes);

    // Create std::string
    std::string result(ch);

    // Clean up
    delete[] ch;

    return result;
}

// Trim from start (in place)
static inline void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
        }));
}

// Trim from end (in place)
static inline void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
        }).base(), s.end());
}

// Trim from both ends (in place)
static inline void trim(std::string& s) {
    ltrim(s);
    rtrim(s);
}

bool FileExists(const std::string& filePath) {
    return std::filesystem::exists(filePath);
}

std::string GetTempFilePath() {
    // Use the temp directory environment variable (on Windows)
    const char* tempDir = std::getenv("TEMP");
    std::filesystem::path tempPath(tempDir);
    tempPath /= "noctilucent";
    tempPath /= "HaloCE Gametype Editor";

    if (tempDir) {
        if (!FileExists(tempPath.string())) {
            try {
                // Create the directory if it doesn't exist
                std::filesystem::create_directories(tempPath);

                std::cout << "Temp directory created successfully." << std::endl;

                return tempPath.string() + "\\";
            }
            catch (const std::filesystem::filesystem_error& e) {
                std::cerr << "Error creating directory in temp: " << e.what() << std::endl;
            }
        } else {
            return tempPath.string();
        }
    }
    else {
        // Handle the case where TEMP environment variable is not set
        std::cerr << "Error: TEMP environment variable not set." << std::endl;
    }

    return "";  // Return an empty string on failure
}

System::String^ SaveResource(std::string sName, std::string sResourceBase64) {
    using namespace System;
    std::string sPath = GetTempFilePath() + "\\" + sName;
    System::String^ SPath = ConvertStdStringToSystemString(sPath);
    System::String^ SResourceBase64 = ConvertStdStringToSystemString(sResourceBase64);
    if (!FileExists(sPath)) {
        std::ofstream outFile(sPath);
        // Check if the file is open
        if (outFile.is_open()) {
            // Decode base64 string to bytes
            array<Byte>^ bytes = Convert::FromBase64String(SResourceBase64);

            // Close the file
            outFile.close();

            // Save the font content to a file
            System::IO::File::WriteAllBytes(SPath, bytes);
            
            std::cout << "Resource saved to file: " << sPath << std::endl;
        }
        else {
            std::cerr << "Error saving resource file: " << sPath << std::endl;
        }
        return SPath;
    }
    else {
        return SPath;
    }
}

// Callback function to write received data into a string
size_t WriteCallbackDetails(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

size_t WriteCallbackHash(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}


// Function to get details page based on index
std::string GetDetailsPage(int iIndex) {
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Create a libcurl handle
    CURL* curl = curl_easy_init();
    std::string responseData;

    if (curl) {
        // URL to fetch
        std::string sPage = std::to_string(iIndex);
        if (iIndex == 1337) { sPage = "_"; }
        std::string url = "https://raw.githubusercontent.com/nocti1/HaloCE-Gametype-Editor/master/details/details_" + sPage + ".html";

        // Set the URL for the request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the callback function to write data to responseData
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackDetails);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        // Clean up
        curl_easy_cleanup(curl);
    }

    // Clean up libcurl
    curl_global_cleanup();

    return responseData;
}

std::string GetStyleSheet() {
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Create a libcurl handle
    CURL* curl = curl_easy_init();
    std::string responseData;

    if (curl) {
        // URL to fetch
        std::string url = "https://raw.githubusercontent.com/nocti1/HaloCE-Gametype-Editor/master/details/style.css";

        // Set the URL for the request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the callback function to write data to responseData
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackDetails);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        // Clean up
        curl_easy_cleanup(curl);
    }

    // Clean up libcurl
    curl_global_cleanup();

    return responseData;
}

std::string GetLatestCommitHash() {
    CURL* curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        std::string apiUrl = "https://api.github.com/repos/nocti1/HaloCE-Gametype-Editor/commits?path=details/style.css";
        std::string responseData;

        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());

        // Set the callback function to write response data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackHash);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else {
            // Parse the JSON response
            json commitData = json::parse(responseData);

            // Check if there are commits
            if (!commitData.empty()) {
                // Get the latest commit hash
                std::string latestCommitHash = commitData[0]["sha"];
                return latestCommitHash;
            }
            else {
                std::cerr << "No commits found for the specified file." << std::endl;
            }
        }

        // Cleanup
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return "";
}

inline constexpr auto CRCTable = []() {
    std::array<std::uint32_t, 256> retval{};
    std::generate(retval.begin(), retval.end(),
        [n = std::uint32_t{ 0 }]() mutable {
            auto c = n++;
            for (std::uint8_t k = 0; k < 8; ++k) {
                if (c & 1) c = std::uint32_t{ 0xedb88320 } ^ (c >> 1);
                else c >>= 1;
            }
            return c;
        });
    return retval;
}();

[[nodiscard]] constexpr std::uint32_t GetCRC(const std::ranges::input_range auto& rng)
noexcept requires std::convertible_to<std::ranges::range_value_t<decltype(rng)>, std::uint8_t> {
    std::uint32_t crc = ~std::accumulate(std::ranges::begin(rng), std::ranges::end(rng),
        ~std::uint32_t{ 0 } &std::uint32_t{ 0xff'ff'ff'ffu },
        [](std::uint32_t checksum, std::uint8_t value)
        { return CRCTable[(checksum ^ value) & 0xff] ^ (checksum >> 8); });
    return ~crc; // Apply 1's complement to the final result
}

std::string_view byteBufferToStringView(const std::vector<BYTE>& byteBuffer) {
    // Assuming byteBuffer.data() is the pointer to the data
    // and byteBuffer.size() is the size of the buffer
    return std::string_view(reinterpret_cast<const char*>(byteBuffer.data()), byteBuffer.size());
}

// Function to read gametypes from a binary file
bool readGametypesFromBinaryFile(const std::string& filename, std::vector<Gametype>& gametypes) {
    std::cout << "Reading blam.lst: " << filename << std::endl;
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open the binary file." << std::endl;
        return false;
    }

    gametypes.clear(); // Clear the existing gametypes vector

    while (file.peek() != EOF) {
        Gametype gametype;
        file.read(reinterpret_cast<char*>(&gametype), sizeof(Gametype));
        gametypes.push_back(gametype);
    }

    return true;
}

// Function to extract values stored as 32-bit unsigned integer
uint8_t VehicleValue(uint32_t value, uint8_t num) {
    switch (num) {
    case 0: return static_cast<uint8_t>(value & 7);
    case 1: return static_cast<uint8_t>((value & 14) >> 1);
    case 2: return static_cast<uint8_t>((value & 112) >> 4);
    case 3: return static_cast<uint8_t>((value & 896) >> 7);
    case 4: return static_cast<uint8_t>((value & 7168) >> 10);
    case 5: return static_cast<uint8_t>((value & 57344) >> 13);
    case 6: return static_cast<uint8_t>((value & 458752) >> 16);
    case 7: return static_cast<uint8_t>((value & 3670016) >> 19);
    default: return 0;
    }
}

// Function to convert values to 32-bit unsigned integer
uint32_t SetVehicleValue(uint32_t Setting, uint8_t VSet, uint8_t Hogs, uint8_t Ghosts, uint8_t Tanks, uint8_t RocketHogs, uint8_t Banshees, uint8_t Turrets) {
    // VehicleSet
    Setting &= ~7u; // Clear the last 3 bits
    Setting |= static_cast<uint32_t>(VSet); // Set the last 3 bits

    // Hogs
    Setting &= ~112u; // Clear bits 4-6
    Setting |= static_cast<uint32_t>(Hogs) << 4; // Set bits 4-6

    // Ghosts
    Setting &= ~896u; // Clear bits 7-9
    Setting |= static_cast<uint32_t>(Ghosts) << 7; // Set bits 7-9

    // Rocket Hogs
    Setting &= ~7168u; // Clear bits 10-12
    Setting |= static_cast<uint32_t>(Tanks) << 10; // Set bits 10-12

    // Banshees
    Setting &= ~57344u; // Clear bits 13-15
    Setting |= static_cast<uint32_t>(RocketHogs) << 13; // Set bits 13-15

    // Tanks
    Setting &= ~458752u; // Clear bits 16-18
    Setting |= static_cast<uint32_t>(Banshees) << 16; // Set bits 16-18

    // Turrets
    Setting &= ~3670016u; // Clear bits 19-21
    Setting |= static_cast<uint32_t>(Turrets) << 19; // Set bits 19-21

    return Setting;
}

static size_t WriteCallbackUpdate(void* contents, size_t size, size_t nmemb, void* userp) {
    if (userp) {
        std::string* data = static_cast<std::string*>(userp);

        if (contents && size > 0 && nmemb > 0) {
            data->append(static_cast<const char*>(contents), size * nmemb);
            return size * nmemb;
        }
    }
    return 0;
}

std::string UpdateCheck() {
    if (bUpdated) {
        if (jsonData.size() > 1) {
            json jsonObject = json::parse(jsonData);
            std::string tag_name = jsonObject["tag_name"];
            std::regex versionRegex("([0-9]+[.]*[0-9]*)");
            std::smatch match;
            if (std::regex_search(tag_name, match, versionRegex)) {
                std::cout << "Latest version: " << match[1].str() << std::endl;
                return match[1].str();
            }
        } else {
            std::cout << "Error! " << sLastError << "." << std::endl;
            return "Error! " + sLastError;
        }
    }
}

System::Drawing::Bitmap^ LoadPngResource(int iResource) {
    // Find the resource
    HRSRC hResource = FindResource(nullptr, MAKEINTRESOURCE(iResource), L"PNG");
    if (hResource == nullptr) {
        // Resource not found
        return nullptr;
    }

    // Load the resource
    DWORD imageSize = SizeofResource(nullptr, hResource);
    HGLOBAL hGlobal = LoadResource(nullptr, hResource);
    LPVOID lpResourceData = LockResource(hGlobal);

    // Copy resource data to managed array
    array<System::Byte>^ managedResourceData = gcnew array<System::Byte>(imageSize);
    pin_ptr<System::Byte> pinnedResourceData = &managedResourceData[0];
    memcpy(pinnedResourceData, lpResourceData, imageSize);

    // Create a MemoryStream from the data
    System::IO::MemoryStream^ memoryStream = gcnew System::IO::MemoryStream(managedResourceData);

    // Create a Bitmap from the MemoryStream
    System::Drawing::Bitmap^ bitmap = gcnew System::Drawing::Bitmap(memoryStream);

    // Clean up resources
    FreeResource(hGlobal);

    return bitmap;
}

