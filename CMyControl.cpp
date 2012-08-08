#include <vector>
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include "CMyControl.hpp"

struct CMyVertex
{
  XMFLOAT3 pos;
  XMFLOAT3 norm;
};

class CMyControl::CMyControlImpl
{
public:
  CMyControl *_this;
  BOOL _rotate;
  BOOL _init;
  DWORD _nIdx;
  DWORD _nVtx;
  DWORD _nTri;
  DWORD _timeStart;
  CComPtr<ID3D11Device> _device;
  CComPtr<ID3D11DeviceContext> _dc;
  CComPtr<IDXGISwapChain> _swc;
  CComPtr<ID3D11RenderTargetView> _rtv;
  CComPtr<ID3D11Buffer> _vb;
  CComPtr<ID3D11Buffer> _ib;
  CComPtr<ID3D11Buffer> _vsCB;
  CComPtr<ID3D11VertexShader> _vs;
  CComPtr<ID3D11PixelShader> _ps;
  CComPtr<ID3D11InputLayout> _il;
  CComPtr<ID3D11RasterizerState> _rs;
  D3D11_VIEWPORT _vp;
  XMMATRIX _world;
  XMMATRIX _view;
  XMMATRIX _proj;
  UINT_PTR _timer;

  CMyControlImpl(CMyControl *wrapper)
  {
    _this = wrapper;
    _rotate = FALSE;
    _init = FALSE;
    _timeStart = 0;
  }

  LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL&)
  {
    _timer = _this->SetTimer(0, 10);
    HRESULT hr = InitD3D();
    return SUCCEEDED(hr) ? 0 : -1;
  }

  LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL&)
  {
    _this->KillTimer(_timer);
    return 0;
  }

  LRESULT OnSize(UINT, WPARAM, LPARAM, BOOL&)
  {
    if(!_init)
      return 0;
    RECT rc;
    _this->GetClientRect(&rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;
    if(width < 10 || height < 10)
      return 0;
    _rtv.Release();
    HRESULT hr = _swc->ResizeBuffers(1, width, height, DXGI_FORMAT_UNKNOWN, 0);
    CComPtr<ID3D11Texture2D> bb;
    _swc->GetBuffer(0, IID_PPV_ARGS(&bb));
    _device->CreateRenderTargetView(bb, NULL, &_rtv);
    _vp.Width = (FLOAT)width;
    _vp.Height = (FLOAT)height;
    _proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, _vp.Width/_vp.Height, 0.001f, 100.0f);
    return 0;
  }

  LRESULT OnTimer(UINT, WPARAM, LPARAM, BOOL&)
  {
    Render();
    return 0;
  }

  HRESULT OnDraw(ATL_DRAWINFO& di)
  {    
    Render();
    return S_OK;
  }

  HRESULT Run()
  {
    _rotate = TRUE;
    return S_OK;
  }

  HRESULT Stop()
  {
    _rotate = FALSE;
    return S_OK;
  }

  void Render()
  {
    if(!_init)
      return;
    DWORD timeCur = GetTickCount();
    if(0 == _timeStart)
      _timeStart = timeCur;
    FLOAT angle = (timeCur - _timeStart) / 1000.0f;    
    XMMATRIX rot = _rotate ? XMMatrixRotationRollPitchYaw(angle, angle, angle)                    
                           : XMMatrixIdentity();

    FLOAT bgcolor[4] = {0.0f, 0.8f, 1.0f, 1.0f};
    _dc->OMSetRenderTargets(1, &_rtv.p, NULL);
    _dc->ClearRenderTargetView(_rtv, bgcolor);

    _world = rot;

    XMMATRIX matrices[] =
    {      
        _world,
        _world * _view * _proj,
    };
    UINT stride = sizeof(CMyVertex);
    UINT offset = 0;
    _dc->UpdateSubresource(_vsCB, 0, NULL, &matrices, 0, 0);
    _dc->RSSetViewports(1, &_vp);
    _dc->IASetInputLayout(_il);
    _dc->IASetVertexBuffers(0, 1, &_vb.p, &stride, &offset);
    _dc->IASetIndexBuffer(_ib, DXGI_FORMAT_R32_UINT, 0);
    _dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    _dc->VSSetShader(_vs, NULL, 0);
    _dc->VSSetConstantBuffers(0, 1, &_vsCB.p);
    _dc->PSSetShader(_ps, NULL, 0);
    _dc->RSSetState(_rs);

    _dc->DrawIndexed(_nIdx, 0, 0);

    _swc->Present(0, 0);
  }

  HRESULT InitD3D()
  {
    if(_init)
      return S_OK;
    D3D_FEATURE_LEVEL fl[] =
    {
      D3D_FEATURE_LEVEL_10_1,
    };
    D3D_FEATURE_LEVEL oFl;
    DWORD df = 0;
#if defined(DEBUG) || defined(_DEBUG)
    df |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    RECT rc;
    _this->GetClientRect(&rc);
    DXGI_SWAP_CHAIN_DESC sd;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.SampleDesc.Count = 4;
    sd.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
    sd.OutputWindow = _this->m_hWnd;
    sd.Flags = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.Height = rc.bottom - rc.top;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Width = rc.right - rc.left;
    D3D_DRIVER_TYPE dt[] = 
    {
      D3D_DRIVER_TYPE_HARDWARE,
      D3D_DRIVER_TYPE_WARP,
      D3D_DRIVER_TYPE_REFERENCE,
    };
    HRESULT hr;
    for(int i = 0; i < ARRAYSIZE(dt); ++i)
    {
      hr = D3D11CreateDeviceAndSwapChain(
        NULL,
        dt[i],
        NULL,
        df,
        fl,
        ARRAYSIZE(fl),
        D3D11_SDK_VERSION,
        &sd,
        &_swc,
        &_device,
        &oFl,
        &_dc);
      if(SUCCEEDED(hr))
        break;
    }
    if(FAILED(hr)) return hr;
    
    CComPtr<ID3D11Texture2D> bb;
    hr = _swc->GetBuffer(0, IID_PPV_ARGS(&bb));
    if(FAILED(hr)) return hr;
    hr = _device->CreateRenderTargetView(bb, NULL, &_rtv);
    if(FAILED(hr)) return hr;
    hr = InitShaders();
    if(FAILED(hr)) return hr;
    hr = InitGeometry();
    if(FAILED(hr)) return hr;
    _init = TRUE;
    _vp.Height = (FLOAT)sd.BufferDesc.Height;
    _vp.Width = (FLOAT)sd.BufferDesc.Width;
    _vp.TopLeftX = 0.0f;
    _vp.TopLeftY = 0.0f;
    _vp.MinDepth = 0.0f;
    _vp.MaxDepth = 1.0f;
    _world = XMMatrixIdentity();
    _view = XMMatrixLookAtLH(
      XMVectorSet(1, -1, -3, 1),
      XMVectorSet(0, 0, 0, 1),
      XMVectorSet(0, 1, 0, 1));
    _proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, _vp.Width/_vp.Height, 0.001f, 100.0f);
    return S_OK;
  }

  HRESULT InitShaders()
  {
    CComPtr<ID3DBlob> codeBlob;
    CComPtr<ID3DBlob> errorBlob;
    HRSRC hrsrc = FindResource(_Module.m_hInst, MAKEINTRESOURCE(IDS_SHADER), MAKEINTRESOURCE(SHADER_RESOURCE));
    if(!hrsrc)
      return E_FAIL;
    HGLOBAL res = LoadResource(_Module.m_hInst, hrsrc);
    if(!res)
      return E_FAIL;
    LPVOID code = LockResource(res);
    DWORD size = SizeofResource(_Module.m_hInst, hrsrc);
    HRESULT hr = D3DCompile(code, size, "Vertex shader", NULL, NULL, "SceneVS", "vs_4_0", 0, 0, &codeBlob, &errorBlob);
    if(FAILED(hr))
    {
#if defined(DEBUG) || defined(_DEBUG)
      OutputDebugStringA(errorBlob ? (LPCSTR)errorBlob->GetBufferPointer() : "Unknown compiler error.\n");
#endif
      return hr;
    }
    hr = _device->CreateVertexShader(codeBlob->GetBufferPointer(), codeBlob->GetBufferSize(), NULL, &_vs);
    if(FAILED(hr))
      return hr;
    D3D11_INPUT_ELEMENT_DESC layout[] = 
    {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    hr = _device->CreateInputLayout(layout, ARRAYSIZE(layout), codeBlob->GetBufferPointer(), codeBlob->GetBufferSize(), &_il);
    if(FAILED(hr))
      return hr;
    codeBlob.Release();
    errorBlob.Release();
    hr = D3DCompile(code, size, "Pixel shader", NULL, NULL, "ScenePS", "ps_4_0", 0, 0, &codeBlob, &errorBlob);
    if(FAILED(hr))
    {
#if defined(DEBUG) || defined(_DEBUG)
      OutputDebugStringA(errorBlob ? (LPCSTR)errorBlob->GetBufferPointer() : "Unknown compiler error.\n");
#endif
      return hr;
    }
    hr = _device->CreatePixelShader(codeBlob->GetBufferPointer(), codeBlob->GetBufferSize(), NULL, &_ps);
    if(FAILED(hr))
      return hr;
    D3D11_BUFFER_DESC bd = {0};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.ByteWidth = sizeof(XMMATRIX)*2;
    hr = _device->CreateBuffer(&bd, NULL, &_vsCB);
    if(FAILED(hr))
      return hr;
    D3D11_RASTERIZER_DESC rd;
    SecureZeroMemory(&rd, sizeof(rd));
    rd.CullMode = D3D11_CULL_NONE;
    rd.FillMode = D3D11_FILL_WIREFRAME;
    hr = _device->CreateRasterizerState(&rd, &_rs);
    if(FAILED(hr))
      return hr;
    return S_OK;
  }

  HRESULT InitGeometry()
  {
    const int lat = 32;
    const int lon = 16;
    const float r = 1.0f;
    int nVtx = (lon - 2) * lat + 2;
    int nTri = (lon - 2) * (lat - 1) * 2;
    std::vector<CMyVertex> vertices;
    std::vector<DWORD> indices;
    _nVtx = nVtx;
    _nTri = nTri;
    _nIdx = nTri*3;
    for(int j = 1; j<lon-1; ++j)
    {
      float theta = float(j)/(lon-1) * XM_PI;
      for(int i = 0; i<lat; ++i)
      {
        float phi = float(i)/(lat-1) * XM_2PI;
        CMyVertex v;
        v.pos = XMFLOAT3(
          sinf(theta) * cosf(phi),
          cosf(theta),
          -sinf(theta) * sinf(phi));
        XMStoreFloat3(&v.norm,XMVector3Normalize(XMLoadFloat3(&v.pos)));
        vertices.push_back(v);
      }
    }
    CMyVertex v;
    v.pos = XMFLOAT3(0, 1, 0);
    XMStoreFloat3(&v.norm,XMVector3Normalize(XMLoadFloat3(&v.pos)));
    vertices.push_back(v);
    v.pos = XMFLOAT3(0, -1, 0);
    XMStoreFloat3(&v.norm,XMVector3Normalize(XMLoadFloat3(&v.pos)));
    vertices.push_back(v);
    for(int j = 0; j<lon-3; ++j)
    {
      for(int i = 0; i<lat-1; ++i)
      {
        indices.push_back(j*lat+i);
        indices.push_back(j*lat+i+1);
        indices.push_back((j+1)*lat+i+1);

        indices.push_back(j*lat+i);
        indices.push_back((j+1)*lat+i+1);
        indices.push_back((j+1)*lat+i);
      }
    }
    for(int i = 0; i<lat-1; ++i)
    {
      indices.push_back((lon-2)*lat);
      indices.push_back(i+1);
      indices.push_back(i);

      indices.push_back((lon-2)*lat+1);
      indices.push_back((lon-3)*lat+i);
      indices.push_back((lon-3)*lat+i+1);
    }
    D3D11_BUFFER_DESC bd = {0};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.ByteWidth = vertices.size() * sizeof(CMyVertex);
    D3D11_SUBRESOURCE_DATA sd = {0};
    sd.pSysMem = vertices.data();
    HRESULT hr = _device->CreateBuffer(&bd, &sd, &_vb);
    if(FAILED(hr))
      return hr;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.ByteWidth = indices.size() * sizeof(DWORD);
    sd.pSysMem = indices.data();
    hr = _device->CreateBuffer(&bd, &sd, &_ib);
    if(FAILED(hr))
      return hr;
    return S_OK;
  }
};

CMyControl::CMyControl()
{
  m_bWindowOnly = TRUE;
  _impl = new CMyControlImpl(this);
}

CMyControl::~CMyControl()
{
  delete _impl;
}

STDMETHODIMP CMyControl::Run()
{
  return _impl->Run();
}

STDMETHODIMP CMyControl::Stop()
{
  return _impl->Stop();
}

HRESULT CMyControl::OnDraw(ATL_DRAWINFO& di)
{
  return _impl->OnDraw(di);
}

LRESULT CMyControl::OnCreate(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& handled)
{
  return _impl->OnCreate(msg, wParam, lParam, handled);
} 

LRESULT CMyControl::OnDestroy(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& handled)
{
  return _impl->OnDestroy(msg, wParam, lParam, handled);
} 

LRESULT CMyControl::OnSize(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& handled)
{
  return _impl->OnSize(msg, wParam, lParam, handled);
} 

LRESULT CMyControl::OnTimer(UINT msg, WPARAM wParam, LPARAM lParam, BOOL& handled)
{
  return _impl->OnTimer(msg, wParam, lParam, handled);
} 
