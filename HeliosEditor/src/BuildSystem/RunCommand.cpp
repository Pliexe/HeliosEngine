#include "RunCommand.h"

using namespace Helios;

CommandResult Helios::RunCommand(const std::u8string &command, std::function<void(const std::string &)> onOutput)
{
    CommandResult result;

    #ifdef HELIOS_PLATFORM_WINDOWS
    std::wstring wcommand = conversions::from_utf8_to_utf16(command);
    
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = nullptr;

    HANDLE hStdoutRead, hStdoutWrite;
    if (!CreatePipe(&hStdoutRead, &hStdoutWrite, &saAttr, 0))
    {
        result.exitCode = GetLastError();
        return result;
    }

    SetHandleInformation(hStdoutRead, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdOutput = hStdoutWrite;
    si.hStdError = hStdoutWrite; // Redirect stderr too

    if (!CreateProcessW(nullptr, wcommand.data(), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi))
    {
        result.exitCode = GetLastError();
        CloseHandle(hStdoutRead);
        CloseHandle(hStdoutWrite);
        return result;
    }

    CloseHandle(hStdoutWrite);

    DWORD bytesRead = 0;
    char buffer[1024];
    std::string lineBuffer;
    std::ostringstream fullOutput;

    // read output line by ine
    while (ReadFile(hStdoutRead, buffer, sizeof(buffer), &bytesRead, nullptr) && bytesRead > 0)
    {
        std::string chunk(buffer, bytesRead);
        fullOutput << chunk;

        for (char ch : chunk)
        {
            if (ch == '\n')
            {
                if (onOutput) onOutput(lineBuffer);
                lineBuffer.clear();
            }
            else
            {
                lineBuffer += ch;
            }
        }
    }

    // send last remaining buffered line
    if (!lineBuffer.empty() && onOutput)
    {
        onOutput(lineBuffer);
    }

    result.output = fullOutput.str();

    CloseHandle(hStdoutRead);
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode = 0;
    if (GetExitCodeProcess(pi.hProcess, &exitCode))
    {
        result.exitCode = exitCode;
    }
    else
    {
        result.exitCode = GetLastError();
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    #elif defined(HELIOS_PLATFORM_LINUX)
    char buffer[1024];
    FILE* pipe = popen(reinterpret_cast<const char*>(command.c_str()), "r");
    if (!pipe)
    {
        result.exitCode = -1;
        return result;
    }

    std::ostringstream fullOutput;
    std::string lineBuffer;

    while (fgets(buffer, sizeof(buffer), pipe))
    {
        fullOutput << buffer;
        std::string chunk(buffer);

        for (char ch : chunk)
        {
            if (ch == '\n')
            {
                if (onOutput) onOutput(lineBuffer);
                lineBuffer.clear();
            }
            else
            {
                lineBuffer += ch;
            }
        }
    }

    if (!lineBuffer.empty() && onOutput)
    {
        onOutput(lineBuffer);
    }

    result.output = fullOutput.str();
    result.exitCode = pclose(pipe);

    #else
    result.exitCode = system(reinterpret_cast<const char*>(command.c_str()));
    #endif

    return result;
}

CommandResult Helios::RunCommand(const std::u8string &command)
{
    return RunCommand(command, nullptr);
}
