#ifndef __CMY_CONTROL_HPP__
#define __CMY_CONTROL_HPP__

#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include "Resource.h"
#include "MyActiveX.hpp"
#include "MyControl.hpp"

class DECLSPEC_UUID("5747094E-84FB-47B4-BC0C-F89FB583895F") CMyControl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CStockPropImpl<CMyControl, IMyControl, &IID_IMyControl, &LIBID_MyActiveXLib>,
  public CComControl<CMyControl>,
  public IPersistStreamInitImpl<CMyControl>,
  public IPersistPropertyBagImpl<CMyControl>,
  public IObjectSafetyImpl<CMyControl, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>,
  public IOleControlImpl<CMyControl>,
  public IOleObjectImpl<CMyControl>,
  public IOleInPlaceActiveObjectImpl<CMyControl>,
  public IViewObjectExImpl<CMyControl>,
  public IOleInPlaceObjectWindowlessImpl<CMyControl>,
  public IConnectionPointContainerImpl<CMyControl>,
  public IPersistStorageImpl<CMyControl>,
  public ISpecifyPropertyPagesImpl<CMyControl>,
  public IQuickActivateImpl<CMyControl>,
  public IDataObjectImpl<CMyControl>,
  public IProvideClassInfo2Impl<&CLSID_MyControl, &DIID__IMyControlEvents, &LIBID_MyActiveXLib>,
  public IPropertyNotifySinkCP<CMyControl>,
  public CComCoClass<CMyControl, &CLSID_MyControl>
{
public:
DECLARE_REGISTRY_RESOURCEID(IDR_MYCONTROL)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMyControl)
	COM_INTERFACE_ENTRY(IMyControl)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY(IPersistPropertyBag)
	COM_INTERFACE_ENTRY(IObjectSafety)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY(IQuickActivate)
	COM_INTERFACE_ENTRY(IPersistStorage)
	COM_INTERFACE_ENTRY(IDataObject)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
END_COM_MAP()

BEGIN_PROP_MAP(CMyControl)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CMyControl)
  CONNECTION_POINT_ENTRY(IID_IPropertyNotifySink)
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CMyControl)  
	MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
  MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
  MESSAGE_HANDLER(WM_SIZE, OnSize)
  MESSAGE_HANDLER(WM_TIMER, OnTimer)
	CHAIN_MSG_MAP(CComControl<CMyControl>)
END_MSG_MAP()

DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

  CMyControl();
  ~CMyControl();
  STDMETHOD(Run)();
  STDMETHOD(Stop)();
  HRESULT OnDraw(ATL_DRAWINFO& di);

private:
  LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  CMyControl(const CMyControl& copy);
  class CMyControlImpl;
  CMyControlImpl *_impl;
};

#endif // __CMY_CONTROL_HPP__
