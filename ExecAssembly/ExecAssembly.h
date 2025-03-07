#ifndef EXEC_ASSEMBLY_H
# define EXEC_ASSEMBLY_H

# include <iostream>
# include <fstream>
# include <metahost.h>
# include <corerror.h>
# include <comdef.h>

// Vars used to store raw files in memory
extern unsigned char* rawData;
extern size_t rawDataLength;

// DA FUNCTION
void DoReflectiveAssembly(int argc, char** argv);

void PatchAhEhmEssHeeScanBuffer();

#endif
