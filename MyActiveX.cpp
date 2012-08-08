#include <windows.h>
#include <atlbase.h>
#include "MyActiveX.hpp"
#include "CMyControl.hpp"

const IID IID_IMyControl = {0x2B26D028,0x4DA6,0x4D69,{0x95,0x13,0xD0,0xCA,0x55,0x09,0x49,0xD1}};
const IID LIBID_MyActiveXLib = {0x1E4E47F3,0x21AF,0x407C,{0x95,0x44,0x59,0xC3,0x4C,0x81,0xF3,0xFA}};
const IID  DIID__IMyControlEvents= {0xE7D13B5A,0x0A09,0x440A,{0x81,0xEA,0xC9,0xE3,0xB0,0x10,0x5D,0xB0}};
const CLSID CLSID_MyControl= {0x5747094E,0x84FB,0x47B4,{0xBC,0x0C,0xF8,0x9F,0xB5,0x83,0x89,0x5F}};

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
  OBJECT_ENTRY(CLSID_MyControl, CMyControl)
END_OBJECT_MAP()

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
  if(DLL_PROCESS_ATTACH == dwReason)
  {
    _Module.Init(ObjectMap, hInstance, &LIBID_MyActiveXLib);
    DisableThreadLibraryCalls(hInstance);
  }
  else if(DLL_PROCESS_DETACH == dwReason)
    _Module.Term();
  return TRUE;
}

STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

STDAPI DllRegisterServer(void)
{
    return _Module.RegisterServer(TRUE);
}

STDAPI DllUnregisterServer(void)
{
    return _Module.UnregisterServer(TRUE);
}
