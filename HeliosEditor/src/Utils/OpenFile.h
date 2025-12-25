#pragma once

#include <filesystem>
#include <vector>

#ifdef HELIOS_PLATFORM_WINDOWS
#include <shobjidl.h>
#endif

namespace Helios
{
    
    
    namespace Utils
    {
        struct OpenFileDialogOptions
        {

        };

        struct OpenFileDialogResult
        {
            enum class Result { Ok, Canceled } result;
            std::vector<std::filesystem::path> paths;
        };
        
        OpenFileDialogResult OpenFile(OpenFileDialogOptions options)
        {
            #ifdef HELIOS_PLATFORM_WINDOWS
            
            IFileDialog* fileDialog;
            HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, __uuidof(IFileDialog), reinterpret_cast<void**>(&fileDialog));
            if (FAILED(hr))
            {
                return { .result = OpenFileDialogResult::Result::Canceled };  
            }

            hr = fileDialog->Show(nullptr);
            if (FAILED(hr))
            {
                return { .result = OpenFileDialogResult::Result::Canceled };  
            }

            IShellItem* item;
            hr = fileDialog->GetResult(&item);
            if (FAILED(hr))
            {
                return { .result = OpenFileDialogResult::Result::Canceled };  
            }

            PWSTR path;
            hr = item->GetDisplayName(SIGDN_FILESYSPATH, &path);
            if (FAILED(hr))
            {
                return { .result = OpenFileDialogResult::Result::Canceled };  
            }

            return { .result = OpenFileDialogResult::Result::Ok, .paths = { path } };

            #elif 
            #error "Not implemented yet"
            #endif
        }
    }
}