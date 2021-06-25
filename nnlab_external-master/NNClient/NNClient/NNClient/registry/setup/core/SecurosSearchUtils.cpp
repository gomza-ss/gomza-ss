#include <NNClient/registry/setup/core/SecurosSearchUtils.h>

#include <QFileInfo>

#include <array>

#ifdef _WIN32
#include <Windows.h>
#elif
#error "Implement getSecurosPath() for your system"
#endif

#include <QString>

namespace
{

#ifdef _WIN32
inline bool findPathInRegedit(std::wstring &path, const std::wstring& regPath,
    const std::wstring& valueName, bool is32Bit) {
    HKEY hKey;
    LONG lRes = RegOpenKeyExW(HKEY_LOCAL_MACHINE, regPath.c_str(), 0,
        KEY_READ | (is32Bit ? 0 : KEY_WOW64_64KEY), &hKey);
    if (lRes != ERROR_SUCCESS) {
        return false;
    }
    WCHAR szBuffer[512];
    DWORD dwBufferSize = sizeof(szBuffer);
    ULONG nError = RegQueryValueExW(hKey, valueName.c_str(), 0, NULL,
        (LPBYTE)szBuffer, &dwBufferSize);
    if (ERROR_SUCCESS == nError) {
        path = szBuffer;
    }
    else {
        return false;
    }

    return true;
}

#endif

}

namespace nnlab
{

std::string getInstallPath()
{

    std::wstring securosPath;

#ifdef _WIN32
    securosPath = L"C:/Program Files (x86)/ISS/SecurOS/";

    static const std::array<std::pair<std::wstring, std::wstring>, 2> regSearchPaths =
    {{
        {L"SOFTWARE\\ISS\\SecurOS", L"InstallPath"},
        {L"SYSTEM\\CurrentControlSet\\Services\\stream_server_svc", L"ImagePath"}
    }};

    for (const auto& pair: regSearchPaths)
    {
        if (findPathInRegedit(securosPath, pair.first, pair.second, true))
        {
            break;
        }
        if (findPathInRegedit(securosPath, pair.first, pair.second, false))
        {
            break;
        }
    }

#endif

    QFileInfo fileInfo(QString::fromStdWString(securosPath));
    std::string ret;
    if (fileInfo.isFile())
    {
        ret = fileInfo.absolutePath().toStdString();
    }
    else
    {
        ret = QString::fromStdWString(securosPath).toStdString();
    }
    std::replace(ret.begin(), ret.end(), '\\', '/');

    return ret;
}

}
