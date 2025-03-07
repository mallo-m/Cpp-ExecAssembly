#include "ExecAssembly.h"
#include "Utils.h"
#include "stdafx.h"

static ICLRMetaHost* InitMetaHost()
{
    ICLRMetaHost* pMetaHost;
    HRESULT hr;

    pMetaHost = NULL;
    hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (void**)&pMetaHost);
    if (FAILED(hr))
    {
        printf("[!] InitMetaHost failed\n");
        return (NULL);
    }
    printf("[+] InitMetaHost success\n");

    return (pMetaHost);
}

/* Get ICLRRuntimeInfo instance */
static ICLRRuntimeInfo* InitRuntime(ICLRMetaHost* pMetaHost)
{
    BOOL bLoadable;
    ICLRRuntimeInfo* pRuntimeInfo;

    pRuntimeInfo = NULL;
    if (pMetaHost == NULL || FAILED(pMetaHost->GetRuntime(L"v4.0.30319", IID_ICLRRuntimeInfo, (void**)&pRuntimeInfo)))
    {
        printf("[!] InitRuntime failed\n");
        return (NULL);
    }

    printf("[+] InitRuntime success\n");

    /* Check if the specified runtime can be loaded */
    if (FAILED(pRuntimeInfo->IsLoadable(&bLoadable)) || !bLoadable)
    {
        printf("[!] Runtime does not appear loadable\n");
        return (NULL);
    }
    printf("[+] Runtime is loadable\n");

    return (pRuntimeInfo);
}

/* Get ICorRuntimeHost instance */
static ICorRuntimeHost* InitInterface(ICLRRuntimeInfo* pRuntimeInfo)
{
    HRESULT hr;
    ICorRuntimeHost* pRuntimeHost;

    pRuntimeHost = NULL;
    if (pRuntimeInfo == NULL || FAILED(pRuntimeInfo->GetInterface(CLSID_CorRuntimeHost, IID_ICorRuntimeHost, (void**)&pRuntimeHost)))
    {
        printf("[!] InitInterface failed\n");
        return (NULL);
    }

    printf("[+] InitInterface success\n");

    /* Start the CLR */
    hr = pRuntimeHost->Start();
    if (FAILED(hr))
    {
        printf("[!] Could not start the Runtime\n");
        return (NULL);
    }

    printf("[+] Runtime started\n");
    return (pRuntimeHost);
}

static _AppDomainPtr GetDefaultDomain(ICorRuntimeHost* pRuntimeHost)
{
    IUnknownPtr pAppDomainThunk;
    _AppDomainPtr pDefaultAppDomain = NULL;

    pAppDomainThunk = NULL;
    if (!pRuntimeHost || FAILED(pRuntimeHost->GetDefaultDomain(&pAppDomainThunk)))
    {
        printf("[!] Could not load Default Domain failed\n");
        return (NULL);
    }
    printf("[+] Opened Default Domain\n");

    /* Equivalent of System.AppDomain.CurrentDomain in C# */
    if (!pAppDomainThunk || FAILED(pAppDomainThunk->QueryInterface(__uuidof(_AppDomain), (void**)&pDefaultAppDomain)))
    {
        printf("[!] Default Domain has no interface ???\n");
        return (NULL);
    }

    printf("[+] Interface opened on Default Domain\n");
    return (pDefaultAppDomain);
}

static SAFEARRAY* InitAssemblyMemory()
{
    void* pvData;
    SAFEARRAY* pSafeArray;
    SAFEARRAYBOUND rgsabound[1];

    pvData = NULL;
    rgsabound[0].cElements = rawDataLength;
    rgsabound[0].lLbound = 0;
    pSafeArray = SafeArrayCreate(VT_UI1, 1, rgsabound);
    if (FAILED(SafeArrayAccessData(pSafeArray, &pvData)))
    {
        printf("[!] InitAssemblyMemory failed\n");
        return (NULL);
    }
    printf("[+] InitAssemblyMemory success\n");

    memcpy(pvData, rawData, rawDataLength);
    SafeArrayUnaccessData(pSafeArray);
    return (pSafeArray);
}

static _AssemblyPtr LoadAssembly(_AppDomainPtr pDefaultAppDomain, SAFEARRAY* pSafeArray)
{
    _AssemblyPtr pAssembly;

    pAssembly = NULL;
    /* Equivalent of System.AppDomain.CurrentDomain.Load(byte[] rawAssembly) */
    if (!pDefaultAppDomain || !pSafeArray || FAILED(pDefaultAppDomain->Load_3(pSafeArray, &pAssembly)))
    {
        printf("[!] Could not load assembly\n");
        return (NULL);
    }

    printf("[+] Assembly loaded\n");
    return (pAssembly);
}

static _MethodInfoPtr ParseEntryPoint(_AssemblyPtr pAssembly)
{
    _MethodInfoPtr pMethodInfo;

    /* Assembly.EntryPoint Property */
    pMethodInfo = NULL;
    if (!pAssembly || FAILED(pAssembly->get_EntryPoint(&pMethodInfo)))
    {
        printf("[!] Could not retrieve assembly's entry point\n");
        return (NULL);
    }
    printf("[+] Assembly's entry point parsed\n");
    return (pMethodInfo);
}

static SAFEARRAY* CreateArguments(int argc, char** argv)
{
    long index = 0;
    VARIANT vtPsa;
    SAFEARRAYBOUND saBound;
    SAFEARRAY* psaStaticMethodArgs;

    saBound.cElements = 1;
    saBound.lLbound = 0;
    psaStaticMethodArgs = SafeArrayCreateVector(VT_VARIANT, 0, 1);

    vtPsa.vt = (VT_ARRAY | VT_BSTR);
    vtPsa.parray = SafeArrayCreateVector(VT_BSTR, 0, argc);
    for (long i = 0; i < argc; i++)
    {
        SafeArrayPutElement(vtPsa.parray, &i, SysAllocString(CstrToLpwstr(argv[i])));
    }

    long idx[1] = { 0 };
    SafeArrayPutElement(psaStaticMethodArgs, idx, &vtPsa);
    return (psaStaticMethodArgs);
}

void DoReflectiveAssembly(int argc, char** argv)
{
    ICLRMetaHost* pMetaHost = NULL;
    ICLRRuntimeInfo* pRuntimeInfo = NULL;
    ICorRuntimeHost* pRuntimeHost = NULL;
    _AppDomainPtr pDefaultAppDomain = NULL;
    SAFEARRAY* pSafeArray;
    _AssemblyPtr pAssembly;
    _MethodInfoPtr pMethodInfo;
    SAFEARRAY* psaStaticMethodArgs;
    VARIANT obj;
    VARIANT retVal;
    HRESULT hr;

    pMetaHost = InitMetaHost();
    pRuntimeInfo = InitRuntime(pMetaHost);
    pRuntimeHost = InitInterface(pRuntimeInfo);
    pDefaultAppDomain = GetDefaultDomain(pRuntimeHost);
    pSafeArray = InitAssemblyMemory();
    pAssembly = LoadAssembly(pDefaultAppDomain, pSafeArray);
    pMethodInfo = ParseEntryPoint(pAssembly);
    psaStaticMethodArgs = CreateArguments(argc, argv);

    if (pMethodInfo == NULL)
        return;

    /* EntryPoint.Invoke(null, new object[0]) */
    ZeroMemory(&obj, sizeof(VARIANT));
    ZeroMemory(&retVal, sizeof(VARIANT));
    obj.vt = VT_NULL;

    hr = pMethodInfo->Invoke_3(obj, psaStaticMethodArgs, &retVal);
    if (FAILED(hr))
    {
        printf("[!] Failed to invoke Assembly, is Main's signature matching: static void Main(string[] args) ? hr = %X\n", hr);
        return;
    }
    printf("[+] Invocation success\n");
}
