#include "Utils.h"

LPWSTR CstrToLpwstr(char* str)
{
	wchar_t* tmp = new wchar_t[4096];

	MultiByteToWideChar(CP_ACP, 0, str, -1, tmp, 4096);
	return (tmp);
}

const char* drunk_strcpy(char* dest, const char* src)
{
	int i;

	i = 0;
	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}
