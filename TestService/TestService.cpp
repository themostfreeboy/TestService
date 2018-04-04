// TestService.cpp : WinMain 的实现


#include "stdafx.h"
#include "resource.h"
#include "TestService_i.h"


using namespace ATL;

#include <stdio.h>

class CTestServiceModule : public ATL::CAtlServiceModuleT< CTestServiceModule, IDS_SERVICENAME >
{
public :
	DECLARE_LIBID(LIBID_TestServiceLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_TESTSERVICE, "{79EAE2B6-B3E0-46D3-A3F3-23225F809373}")
		HRESULT InitializeSecurity() throw()
	{
		// TODO : 调用 CoInitializeSecurity 并为服务提供适当的安全设置
		// 建议 - PKT 级别的身份验证、
		// RPC_C_IMP_LEVEL_IDENTIFY 的模拟级别
		// 以及适当的非 NULL 安全描述符。

		return S_OK;
	}


//个人代码start

    HRESULT RegisterAppId(bool bService = false) throw ();
    HRESULT PreMessageLoop(int nShowCmd) throw();
    HRESULT PostMessageLoop() throw();
    void OnStop() throw();
    void OnPause() throw();
    void OnContinue() throw();
 };

 HRESULT CTestServiceModule::RegisterAppId(bool bService) throw ()
 {
	 HRESULT hr = S_OK;
	 BOOL res = __super ::RegisterAppId(bService);
	 if (bService)
	 {
		 if (IsInstalled())
		 {
			 SC_HANDLE hSCM = ::OpenSCManagerW(NULL, NULL, SERVICE_CHANGE_CONFIG);
			 SC_HANDLE hService = NULL;
			 if (hSCM == NULL)
			 {
				 hr = AtlHresultFromLastError();
			 }
			 else
			 {
				 hService = ::OpenService(hSCM, m_szServiceName, SERVICE_CHANGE_CONFIG);
				 if (hService != NULL)
				 {
					 ::ChangeServiceConfig(hService, SERVICE_NO_CHANGE,
						 SERVICE_AUTO_START,// 修改服务为自动启动
						 NULL, NULL, NULL, NULL, NULL, NULL, NULL,
						 m_szServiceName); // 通过修改资源IDS_SERVICENAME 修改服务的显示名字

					 SERVICE_DESCRIPTION Description;
					 TCHAR szDescription[1024];
					 ZeroMemory(szDescription, 1024);
					 ZeroMemory(&Description, sizeof (SERVICE_DESCRIPTION));
					 lstrcpy(szDescription, _T("测试服务描述信息"));
					 Description.lpDescription = szDescription;
					 ::ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &Description);
					 ::CloseServiceHandle(hService);
				 }
				 else
				 {
					 hr = AtlHresultFromLastError();
				 }
				 ::CloseServiceHandle(hSCM);
			 }
		 }
	 }
	 return hr;
 }

 HRESULT CTestServiceModule::PreMessageLoop(int nShowCmd) throw()
 {
	 // 让暂停继续按钮可以使用
	 m_status.dwControlsAccepted = m_status.dwControlsAccepted | SERVICE_ACCEPT_PAUSE_CONTINUE;

	 HRESULT hr = __super::PreMessageLoop(nShowCmd);
	 // 微软Bug
	 if (hr == S_FALSE)
		 hr = S_OK;

	 // 这里添加自己的初始化代码...

	 if (SUCCEEDED(hr))
	 {
		 // 这个状态一定要修改，否则会出现1053错误，
		 // 这个错误我花了很多时间才搞定
		 SetServiceStatus(SERVICE_RUNNING);
	 }

	 return hr;
 }

 HRESULT CTestServiceModule::PostMessageLoop() throw()
 {
	 HRESULT hr = __super ::PostMessageLoop();

	 if (FAILED(hr))
		 return hr;

	 // 这里添加自己的清除代码

	 return hr;
 }

 void CTestServiceModule::OnStop() throw()
 {
	 __super::OnStop();
	 SetServiceStatus(SERVICE_STOPPED);
 }

 void CTestServiceModule::OnPause() throw()
 {
	 __super::OnPause();
	 SetServiceStatus(SERVICE_PAUSED);
 }

 void CTestServiceModule::OnContinue() throw()
 {
	 __super::OnContinue();
	 SetServiceStatus(SERVICE_RUNNING);
 }


//个人代码end



CTestServiceModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	return _AtlModule.WinMain(nShowCmd);
}

