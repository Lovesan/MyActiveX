CPP=cl.exe /c /nologo
CPPFLAGS=/DUNICODE \
				 /D_UNICODE \
				 /D_ATL_APARTMENT_THREADED \
				 /D_WIN32_WINNT=0x0600 \
				 /DWINVER=0x0600 \
				 /D_XM_NO_INTRINSICS_ \
				 /I"$(DXSDK_DIR)\Include" \
				 /I"$(WINSDK_DIR)\Include" \
				 /I"$(WINDDK_DIR)\inc\atl71"
CXXFLAGS=/O2 /EHsc
LD=link.exe
LDFLAGS=/nologo /dll /dynamicbase /tlbid:1 \
				/nxcompat /machine:x86 \
				/release /subsystem:windows \
				/libpath:"$(DXSDK_DIR)\Lib\x86" \
				/libpath:"$(WINSDK_DIR)\Lib" \
				/libpath:"$(WINDDK_DIR)\lib\atl\i386"
LIBS=atls.lib atlthunk.lib d3d11.lib d3dcompiler.lib libcmt.lib
RC=rc.exe /nologo
MIDL=midl.exe /nologo
DEL=del

PROJECT_NAME=MyActiveX
DLL=MyActiveX.dll
PDB=MyActiveX.pdb
OBJS=MyActiveX.obj CMyControl.obj
RES=MyActiveX.res
TLB=MyActiveX.tlb
DEF=MyActiveX.def
IDL=MyActiveX.idl
BMP=DirectX.bmp
RGS=MyControl.rgs
EXP=MyActiveX.exp
EXPLIB=MyActiveX.lib


all: $(DLL)

$(DLL):$(OBJS) $(RES) $(DEF)
	$(LD) $(LDFLAGS) /out:$(DLL) /def:$(DEF) /pdb:$(PDB) $(LIBS) $(OBJS) $(RES)

$(RES): $(TLB) $(BMP) $(RGS)
	$(RC) $(PROJECT_NAME).rc

$(TLB): $(IDL)
	$(MIDL) /tlb $(TLB) $(IDL)

.cpp.obj:
	$(CPP) $(CPPFLAGS) $(CXXFLAGS) $<

clean:
	$(DEL) /F /Q $(DLL) $(PDB) $(OBJS) $(RES) $(TLB) $(EXP) $(EXPLIB)
