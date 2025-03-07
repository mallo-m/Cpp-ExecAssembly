#pragma once

#ifndef DOWNLOADER_H
# define DOWNLOADER_H

# include <Windows.h>
# include <winhttp.h>
# include <iostream>
# include <vector>

# pragma comment(lib, "winhttp.lib")

std::vector<BYTE> DoHttpDownload(LPCWSTR baseAddress, LPCWSTR filepath);

#endif
