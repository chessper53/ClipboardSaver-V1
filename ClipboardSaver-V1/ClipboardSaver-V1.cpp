// ClipBoardSaver.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <fstream>
#include <iostream>
#include "json\json.h"


using namespace std;


void loadOldClipboard();
void saveClipboard();
void addDataToJson(const std::string& entry);

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);

    if (GetCurrentProcessId() != processId) {
        SendMessage(hwnd, WM_CLOSE, 0, 0); // Send a close message
    }

    return TRUE;
}


int main() {

    //Get the current Path of the running application
    wchar_t path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    std::wstring pathStr(path);
    std::wstring literalStr;

     for (wchar_t c : pathStr) {
         if (c == L'\\') {
            literalStr += L"\\\\";
         }
         else {
             literalStr += c;
         }
     }

    //Add the program to the Autostart
    std::wstring progPath = literalStr;
    HKEY hkey = NULL;
    LONG createStatus = RegCreateKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey); //Creates a key       
    LONG status = RegSetValueEx(hkey, L"MyApp", 0, REG_SZ, (BYTE*)progPath.c_str(), (progPath.size() + 1) * sizeof(wchar_t));

    //Hide The Terminal
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    //Puts the old Clipboard into the new One
    loadOldClipboard();

    // Open the clipboard
    if (!OpenClipboard(nullptr)) {
        std::cerr << "Failed to open clipboard!" << std::endl;
    }
    // Get the current clipboard data
    HANDLE hData = GetClipboardData(CF_TEXT);
    char* pszData = static_cast<char*>(GlobalLock(hData));
    std::string clipboardContent = pszData;

    // Release the clipboard data
    GlobalUnlock(hData);
    CloseClipboard();


    // Wait for a change in the clipboard
    while (true) {
        if (!OpenClipboard(nullptr)) {
            std::cerr << "Failed to open clipboard!" << std::endl;
        }

        HANDLE hNewData = GetClipboardData(CF_TEXT);
        char* pszNewData = static_cast<char*>(GlobalLock(hNewData));
        std::string newClipboardContent = pszNewData;

        // Check if the clipboard content has changed
        if (newClipboardContent != clipboardContent) {
            clipboardContent = newClipboardContent;
            saveClipboard();

        }
        // Release the clipboard data and wait for a change
        GlobalUnlock(hNewData);
        CloseClipboard();
        Sleep(100); // Wait for 100ms before checking again
    }
    return 0; 
}


void loadOldClipboard() {
    // Load the JSON data from our File
    std::ifstream file("pastEntries.json");
    Json::Value root;
    file >> root;
    file.close();

    // Non-Bitmap
    for (const auto& text : root) {
        if (!OpenClipboard(0)) {
            std::cerr << "Failed to open clipboard\n";
            return;
        }

        // Empty the clipboard
        EmptyClipboard();

        std::string entry = text["clipboardEntry"].asString();
        //std::cout << "clipboardEntry: " << entry << std::endl;

        // Allocate global memory for the clipboard data
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, entry.size() + 1);
        if (hMem == NULL) {
            // Handle memory allocation error
            CloseClipboard();
            return;
        }

        // Copy the text to the global memory
        char* pMem = static_cast<char*>(GlobalLock(hMem));
        if (pMem == NULL) {
            // Handle memory locking error
            GlobalFree(hMem);
            CloseClipboard();
            return;
        }
        strcpy_s(pMem, entry.size() + 1, entry.c_str());
        GlobalUnlock(hMem);

        // Set the clipboard data
        SetClipboardData(CF_TEXT, hMem);

        HANDLE hData = GetClipboardData(CF_TEXT);
        char* pszData = static_cast<char*>(GlobalLock(hData));
        std::string clipboardContent = pszData;

        // Close the clipboard
        CloseClipboard();
        Sleep(1000); // Wait for 1000ms
    }
 }



void saveClipboard() {
    


    if (IsClipboardFormatAvailable(CF_BITMAP)) {
        //Clipboard contains a Bitmap
        
        //std::cerr << "Image Found";
        
    }
    else {
        //Clipboard contains Text

        // Get the clipboard data as a text format
        HANDLE handle = GetClipboardData(CF_TEXT);
        // Lock the handle to get a pointer to the data
        char* data = (char*)GlobalLock(handle);
        // Copy the data to a string
        string clipboardData = data;
        // Unlock and close the clipboard
        GlobalUnlock(handle);

        //Insert the Data Into a JSON File
        addDataToJson(clipboardData);
    }
}
void addDataToJson(const std::string& entry) {
    //Get Previous Entries
    std::ifstream file("pastEntries.json");
    Json::Value root;
    file >> root;
    file.close();

    //Create a new JSON object and add it to the array
    Json::Value obj;
    obj["clipboardEntry"] = entry;
    root.append(obj);

    //Save the updated array of objects to the file
    std::ofstream outFile("pastEntries.json");
    if (outFile.is_open()) {
        outFile << root;
        outFile.close();
    }
    else {
        
    }
}



