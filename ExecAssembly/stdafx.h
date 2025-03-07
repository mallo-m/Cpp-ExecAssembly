#include <SDKDDKVer.h>

#include <stdio.h>
#include <tchar.h>

#include <metahost.h>
#pragma comment(lib, "MSCorEE.lib")

#import "C:\Windows\Microsoft.NET\Framework\v4.0.30319\mscorlib.tlb" raw_interfaces_only \
    high_property_prefixes("_get","_put","_putref")		\
    rename("ReportEvent", "InteropServices_ReportEvent") \
    auto_rename
using namespace mscorlib;
