// TestService.cpp : WinMain ��ʵ��


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
		// TODO : ���� CoInitializeSecurity ��Ϊ�����ṩ�ʵ��İ�ȫ����
		// ���� - PKT ����������֤��
		// RPC_C_IMP_LEVEL_IDENTIFY ��ģ�⼶��
		// �Լ��ʵ��ķ� NULL ��ȫ��������

		return S_OK;
	}


//���˴���start

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
						 SERVICE_AUTO_START,// �޸ķ���Ϊ�Զ�����
						 NULL, NULL, NULL, NULL, NULL, NULL, NULL,
						 m_szServiceName); // ͨ���޸���ԴIDS_SERVICENAME �޸ķ������ʾ����

					 SERVICE_DESCRIPTION Description;
					 TCHAR szDescription[1024];
					 ZeroMemory(szDescription, 1024);
					 ZeroMemory(&Description, sizeof (SERVICE_DESCRIPTION));
					 lstrcpy(szDescription, _T("���Է���������Ϣ"));
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
	 // ����ͣ������ť����ʹ��
	 m_status.dwControlsAccepted = m_status.dwControlsAccepted | SERVICE_ACCEPT_PAUSE_CONTINUE;

	 HRESULT hr = __super::PreMessageLoop(nShowCmd);
	 // ΢��Bug
	 if (hr == S_FALSE)
		 hr = S_OK;

	 // ��������Լ��ĳ�ʼ������...

	 if (SUCCEEDED(hr))
	 {
		 // ���״̬һ��Ҫ�޸ģ���������1053����
		 // ��������һ��˺ܶ�ʱ��Ÿ㶨
		 SetServiceStatus(SERVICE_RUNNING);
	 }

	 return hr;
 }

 HRESULT CTestServiceModule::PostMessageLoop() throw()
 {
	 HRESULT hr = __super ::PostMessageLoop();

	 if (FAILED(hr))
		 return hr;

	 // ��������Լ����������

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


//���˴���end



CTestServiceModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	return _AtlModule.WinMain(nShowCmd);
}

