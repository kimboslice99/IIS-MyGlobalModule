#define _WINSOCKAPI_
#define WIN32_LEAN_AND_MEAN
#include <sal.h>
#include "pch.h"
#include <httpserv.h>
#include <windows.h>

// Create the module's global class.
class MyGlobalModule : public CGlobalModule
{
public:

    // Process a GL_APPLICATION_START notification.
    GLOBAL_NOTIFICATION_STATUS
        OnGlobalPreBeginRequest(
            IN IPreBeginRequestProvider* pProvider
        )
    {
        UNREFERENCED_PARAMETER(pProvider);
        WriteEventViewerLog(L"Hello World!");
        return GL_NOTIFICATION_CONTINUE;
    }

    VOID Terminate()
    {
        // Remove the class from memory.
        delete this;
    }

    MyGlobalModule()
    {
        LPCWSTR ST = L"IISADMIN";
        // Open a handle to the Event Viewer.
        m_hEventLog = RegisterEventSource(NULL, ST);
    }

    ~MyGlobalModule()
    {
        // Test whether the handle for the Event Viewer is open.
        if (NULL != m_hEventLog)
        {
            // Close the handle to the Event Viewer.
            DeregisterEventSource(m_hEventLog);
            m_hEventLog = NULL;
        }
    }

private:

    // Create a handle for the event viewer.
    HANDLE m_hEventLog;

    // Define a method that writes to the Event Viewer.
    BOOL WriteEventViewerLog(LPCWSTR szNotification)
    {
        // Test whether the handle for the Event Viewer is open.
        if (NULL != m_hEventLog)
        {
            // Write any strings to the Event Viewer and return.
            return ReportEvent(
                m_hEventLog,
                EVENTLOG_INFORMATION_TYPE, 0, 0,
                NULL, 1, 0, &szNotification, NULL);
        }
        return FALSE;
    }
};

// Create the module's exported registration function.
HRESULT
__stdcall
RegisterModule(
    DWORD dwServerVersion,
    IHttpModuleRegistrationInfo* pModuleInfo,
    IHttpServer* pGlobalInfo
)
{
    UNREFERENCED_PARAMETER(dwServerVersion);
    UNREFERENCED_PARAMETER(pGlobalInfo);

    // Create an instance of the global module class.
    MyGlobalModule* pGlobalModule = new MyGlobalModule;
    // Test for an error.
    if (NULL == pGlobalModule)
    {
        return HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
    }
    // Set the global notifications and exit.
    return pModuleInfo->SetGlobalNotifications(
        pGlobalModule, GL_PRE_BEGIN_REQUEST);
}