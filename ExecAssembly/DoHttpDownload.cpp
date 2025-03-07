#include "Downloader.h"

std::vector<BYTE> DoHttpDownload(LPCWSTR baseAddress, LPCWSTR filepath) {

    // initialise session
    HINTERNET hSession = WinHttpOpen(
        NULL,
        WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,    // proxy aware
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0
        //WINHTTP_FLAG_SECURE_DEFAULTS            // enable ssl
    );

    // create session for target
    HINTERNET hConnect = WinHttpConnect(
        hSession,
        baseAddress,
        INTERNET_DEFAULT_HTTP_PORT,            // port 80
        //INTERNET_DEFAULT_HTTPS_PORT,            // port 443
        0);

    // create request handle
    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect,
        L"GET",
        filepath,
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        0
        //WINHTTP_FLAG_SECURE                    //ssl
    );

    // send the request
    WinHttpSendRequest(
        hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS,
        0,
        WINHTTP_NO_REQUEST_DATA,
        0,
        0,
        0);

    // receive response
    WinHttpReceiveResponse(
        hRequest,
        NULL);

    // read the data
    std::vector<BYTE> buffer;
    DWORD bytesRead = 0;

    do {

        BYTE temp[4096]{};
        WinHttpReadData(hRequest, temp, sizeof(temp), &bytesRead);

        if (bytesRead > 0) {
            buffer.insert(buffer.end(), temp, temp + bytesRead);
        }

    } while (bytesRead > 0);

    // close all the handles
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return buffer;
}
