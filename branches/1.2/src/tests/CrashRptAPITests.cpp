#include "stdafx.h"
#include "Tests.h"
#include "CrashRpt.h"
#include "Utility.h"
#include "strconv.h"

REGISTER_TEST(Test_crInstall_null);
void Test_crInstall_null()
{   
  // Test crInstall with NULL info - should fail
  
  int nInstallResult = crInstallW(NULL);
  TEST_ASSERT(nInstallResult!=0);

  int nInstallResult2 = crInstallA(NULL);
  TEST_ASSERT(nInstallResult2!=0); 

  __TEST_CLEANUP__;
}

REGISTER_TEST(Test_crInstall_wrong_cb);
void Test_crInstall_wrong_cb()
{   
  // Test crInstall with wrong cb parameter - should fail
  
  CR_INSTALL_INFO info;
  memset(&info, 0, sizeof(CR_INSTALL_INFO));
  info.cb = 1000;

  int nInstallResult = crInstall(&info);
  TEST_ASSERT(nInstallResult!=0);

  __TEST_CLEANUP__;
}

REGISTER_TEST(Test_crInstall_missing_app_ver);
void Test_crInstall_missing_app_ver()
{   
  // Test crInstall with with missing app version
  // As this console app has missing EXE product version - should fail
  
  CR_INSTALL_INFO info;
  memset(&info, 0, sizeof(CR_INSTALL_INFO));
  info.cb = sizeof(CR_INSTALL_INFO);
  
  int nInstallResult = crInstall(&info);
  TEST_ASSERT(nInstallResult!=0);
  
  __TEST_CLEANUP__;

}

REGISTER_TEST(Test_crInstallW_zero_info);
void Test_crInstallW_zero_info()
{   
  // Test crInstallW with zero info
  
  CR_INSTALL_INFOW infoW;
  memset(&infoW, 0, sizeof(CR_INSTALL_INFOW));
  infoW.cb = sizeof(CR_INSTALL_INFOW);
  infoW.pszAppVersion = L"1.0.0"; // Specify app version, otherwise it will fail.

  int nInstallResult = crInstallW(&infoW);
  TEST_ASSERT(nInstallResult==0);

  __TEST_CLEANUP__

  int nUninstallResult = crUninstall();
  TEST_ASSERT(nUninstallResult==0); 
}

REGISTER_TEST(Test_crInstallA_zero_info);
void Test_crInstallA_zero_info()
{   
  // Test crInstallA with zero info
  
  CR_INSTALL_INFOA infoA;
  memset(&infoA, 0, sizeof(CR_INSTALL_INFOA));
  infoA.cb = sizeof(CR_INSTALL_INFOA);
  infoA.pszAppVersion = "1.0.0"; // Specify app version, otherwise it will fail.

  int nInstallResult = crInstallA(&infoA);
  TEST_ASSERT(nInstallResult==0);

  __TEST_CLEANUP__

  int nUninstallResult = crUninstall();
  TEST_ASSERT(nUninstallResult==0); 
}

REGISTER_TEST(Test_crInstallA_twice);
void Test_crInstallA_twice()
{   
  // Call crInstallA two times - the second one should fail
  
  CR_INSTALL_INFOA infoA;
  memset(&infoA, 0, sizeof(CR_INSTALL_INFOA));
  infoA.cb = sizeof(CR_INSTALL_INFOA);
  infoA.pszAppVersion = "1.0.0"; // Specify app version, otherwise it will fail.

  int nInstallResult = crInstallA(&infoA);
  TEST_ASSERT(nInstallResult==0);

  int nInstallResult2 = crInstallA(&infoA);
  TEST_ASSERT(nInstallResult2!=0);

  __TEST_CLEANUP__

  int nUninstallResult = crUninstall();
  TEST_ASSERT(nUninstallResult==0); 
}

REGISTER_TEST(Test_crUninstall);
void Test_crUninstall()
{   
  // Call crUninstall - should fail, because crInstall should be called first
    
  int nUninstallResult = crUninstall();
  TEST_ASSERT(nUninstallResult!=0); 

  // And another time... 
  int nUninstallResult2 = crUninstall();
  TEST_ASSERT(nUninstallResult2!=0); 

  __TEST_CLEANUP__;
}

REGISTER_TEST(Test_crAddFileA);
void Test_crAddFileA()
{ 
  strconv_t strconv;
  CString sFileName;

  // Should fail, because crInstall() should be called first
  int nResult = crAddFileA("a.txt", "invalid file");
  TEST_ASSERT(nResult!=0);

  // Install crash handler
  CR_INSTALL_INFOA infoA;
  memset(&infoA, 0, sizeof(CR_INSTALL_INFOA));
  infoA.cb = sizeof(CR_INSTALL_INFOA);
  infoA.pszAppVersion = "1.0.0"; // Specify app version, otherwise it will fail.

  int nInstallResult = crInstallA(&infoA);
  TEST_ASSERT(nInstallResult==0);
  
  // Add not existing file, crAddFileA should fail
  int nResult2 = crAddFileA("a.txt", "invalid file");
  TEST_ASSERT(nResult2!=0);

  // Add existing file, crAddFileA should succeed
  
  sFileName = Utility::GetModulePath(NULL)+_T("\\dummy.ini");
  LPCSTR szFileName = strconv.t2a(sFileName);
  int nResult3 = crAddFileA(szFileName, "Dummy INI File");
  TEST_ASSERT(nResult3==0);

  __TEST_CLEANUP__;

  // Uninstall
  int nUninstallResult = crUninstall();
  TEST_ASSERT(nUninstallResult==0); 
}

REGISTER_TEST(Test_crAddFileW);
void Test_crAddFileW()
{ 
  strconv_t strconv;
  CString sFileName;

  // Should fail, because crInstall() should be called first
  int nResult = crAddFileW(L"a.txt", L"invalid file");
  TEST_ASSERT(nResult!=0);

  // Install crash handler
  CR_INSTALL_INFOW infoW;
  memset(&infoW, 0, sizeof(CR_INSTALL_INFOW));
  infoW.cb = sizeof(CR_INSTALL_INFOW);
  infoW.pszAppVersion = L"1.0.0"; // Specify app version, otherwise it will fail.

  int nInstallResult = crInstallW(&infoW);
  TEST_ASSERT(nInstallResult==0);
  
  // Add not existing file, crAddFileA should fail
  int nResult2 = crAddFileW(L"a.txt", L"invalid file");
  TEST_ASSERT(nResult2!=0);

  // Add existing file, crAddFileA should succeed
  
  sFileName = Utility::GetModulePath(NULL)+_T("\\dummy.ini");
  LPCWSTR szFileName = strconv.t2w(sFileName);
  int nResult3 = crAddFileW(szFileName, L"Dummy INI File");
  TEST_ASSERT(nResult3==0);

  __TEST_CLEANUP__;

  // Uninstall
  int nUninstallResult = crUninstall();
  TEST_ASSERT(nUninstallResult==0); 
}

REGISTER_TEST(Test_crAddPropertyA);
void Test_crAddPropertyA()
{   
  // Should fail, because crInstall() should be called first
  int nResult = crAddPropertyA("VideoAdapter", "nVidia GeForce GTS 250");
  TEST_ASSERT(nResult!=0);

  // Install crash handler
  CR_INSTALL_INFOA infoA;
  memset(&infoA, 0, sizeof(CR_INSTALL_INFOA));
  infoA.cb = sizeof(CR_INSTALL_INFOA);
  infoA.pszAppVersion = "1.0.0"; // Specify app version, otherwise it will fail.

  int nInstallResult = crInstallA(&infoA);
  TEST_ASSERT(nInstallResult==0);
  
  // Should fail, because property name is empty
  int nResult2 = crAddPropertyA("", "nVidia GeForce GTS 250");
  TEST_ASSERT(nResult2!=0);
  
  // Should succeed
  int nResult3 = crAddPropertyA("VideoAdapter", "nVidia GeForce GTS 250");
  TEST_ASSERT(nResult3==0);

  __TEST_CLEANUP__;

  // Uninstall
  int nUninstallResult = crUninstall();
  TEST_ASSERT(nUninstallResult==0); 
}

REGISTER_TEST(Test_crAddPropertyW);
void Test_crAddPropertyW()
{   
  // Should fail, because crInstall() should be called first
  int nResult = crAddPropertyW(L"VideoAdapter", L"nVidia GeForce GTS 250");
  TEST_ASSERT(nResult!=0);

  // Install crash handler
  CR_INSTALL_INFOW infoW;
  memset(&infoW, 0, sizeof(CR_INSTALL_INFOW));
  infoW.cb = sizeof(CR_INSTALL_INFOW);
  infoW.pszAppVersion = L"1.0.0"; // Specify app version, otherwise it will fail.

  int nInstallResult = crInstallW(&infoW);
  TEST_ASSERT(nInstallResult==0);
  
  // Should fail, because property name is empty
  int nResult2 = crAddPropertyW(L"", L"nVidia GeForce GTS 250");
  TEST_ASSERT(nResult2!=0);
  
  // Should succeed
  int nResult3 = crAddPropertyW(L"VideoAdapter", L"nVidia GeForce GTS 250");
  TEST_ASSERT(nResult3==0);

  __TEST_CLEANUP__;

  // Uninstall
  int nUninstallResult = crUninstall();
  TEST_ASSERT(nUninstallResult==0); 
}

REGISTER_TEST(Test_crAddRegKeyA);
void Test_crAddRegKeyA()
{   
  // Should fail, because crInstall() should be called first
  int nResult = crAddRegKeyA("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows", "regkey.xml", 0);
  TEST_ASSERT(nResult!=0);

  // Install crash handler
  CR_INSTALL_INFOA infoA;
  memset(&infoA, 0, sizeof(CR_INSTALL_INFOA));
  infoA.cb = sizeof(CR_INSTALL_INFOA);
  infoA.pszAppVersion = "1.0.0"; // Specify app version, otherwise it will fail.

  int nInstallResult = crInstallA(&infoA);
  TEST_ASSERT(nInstallResult==0);
  
  // Should fail, because registry key name is NULL
  int nResult2 = crAddRegKeyA(NULL, "regkey.xml", 0);
  TEST_ASSERT(nResult2!=0);
  
  // Should fail, because registry key name is empty
  int nResult3 = crAddRegKeyA("", "regkey.xml", 0);
  TEST_ASSERT(nResult3!=0);

  // Should succeed
  int nResult4 = crAddRegKeyA("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows", "regkey.xml", 0);
  TEST_ASSERT(nResult4==0);

  // Should fail, because registry key doesn't exist
  int nResult5 = crAddRegKeyA("HKEY_LOCAL_MACHINE\\Softweeere\\", "regkey.xml", 0);
  TEST_ASSERT(nResult5!=0);

  // Should fail, because registry key is a parent key
  int nResult6 = crAddRegKeyA("HKEY_LOCAL_MACHINE\\", "regkey.xml", 0);
  TEST_ASSERT(nResult6!=0);

  __TEST_CLEANUP__;

  // Uninstall
  int nUninstallResult = crUninstall();
  TEST_ASSERT(nUninstallResult==0); 
}

REGISTER_TEST(Test_crAddRegKeyW);
void Test_crAddRegKeyW()
{   
  // Should fail, because crInstall() should be called first
  int nResult = crAddRegKeyW(L"HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows", L"regkey.xml", 0);
  TEST_ASSERT(nResult!=0);

  // Install crash handler
  CR_INSTALL_INFOW infoW;
  memset(&infoW, 0, sizeof(CR_INSTALL_INFOW));
  infoW.cb = sizeof(CR_INSTALL_INFOW);
  infoW.pszAppVersion = L"1.0.0"; // Specify app version, otherwise it will fail.

  int nInstallResult = crInstallW(&infoW);
  TEST_ASSERT(nInstallResult==0);
  
  // Should fail, because registry key name is NULL
  int nResult2 = crAddRegKeyW(NULL, L"regkey.xml", 0);
  TEST_ASSERT(nResult2!=0);
  
  // Should fail, because registry key name is empty
  int nResult3 = crAddRegKeyW(L"", L"regkey.xml", 0);
  TEST_ASSERT(nResult3!=0);

  // Should succeed
  int nResult4 = crAddRegKeyW(L"HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows", L"regkey.xml", 0);
  TEST_ASSERT(nResult4==0);

  // Should fail, because registry key doesn't exist
  int nResult5 = crAddRegKeyW(L"HKEY_LOCAL_MACHINE\\Softweeere\\", L"regkey.xml", 0);
  TEST_ASSERT(nResult5!=0);

  // Should fail, because registry key is a parent key
  int nResult6 = crAddRegKeyW(L"HKEY_LOCAL_MACHINE\\", L"regkey.xml", 0);
  TEST_ASSERT(nResult6!=0);

  __TEST_CLEANUP__;

  // Uninstall
  int nUninstallResult = crUninstall();
  TEST_ASSERT(nUninstallResult==0); 
}

REGISTER_TEST(Test_crGetLastErrorMsgA);
void Test_crGetLastErrorMsgA()
{ 
  // Get error message before Install
  char szErrMsg[256] = "";
  int nResult = crGetLastErrorMsgA(szErrMsg, 256);
  TEST_ASSERT(nResult>0);

  // Install crash handler
  CR_INSTALL_INFOA infoA;
  memset(&infoA, 0, sizeof(CR_INSTALL_INFOA));
  infoA.cb = sizeof(CR_INSTALL_INFOA);
  infoA.pszAppVersion = "1.0.0"; // Specify app version, otherwise it will fail.

  int nInstallResult = crInstallA(&infoA);
  TEST_ASSERT(nInstallResult==0);
  
  // Get error message
  char szErrMsg2[256] = "";
  int nResult2 = crGetLastErrorMsgA(szErrMsg2, 256);
  TEST_ASSERT(nResult2>0);

  // Get error message to NULL buffer - must fail  
  int nResult3 = crGetLastErrorMsgA(NULL, 256);
  TEST_ASSERT(nResult3<0);

  // Get error message to a buffer, but zero length - must fail  
  char szErrMsg3[256] = "";
  int nResult4 = crGetLastErrorMsgA(szErrMsg3, 0);
  TEST_ASSERT(nResult4<0);

  // Get error message to a single-char buffer, must trunkate message and succeed
  char szErrMsg5[1] = "";
  int nResult5 = crGetLastErrorMsgA(szErrMsg5, 1);
  TEST_ASSERT(nResult5>0);

  // Get error message to a small buffer, must trunkate message and succeed
  char szErrMsg6[2] = "";
  int nResult6 = crGetLastErrorMsgA(szErrMsg6, 2);
  TEST_ASSERT(nResult6>0);

  __TEST_CLEANUP__;

  // Uninstall
  int nUninstallResult = crUninstall();
  TEST_ASSERT(nUninstallResult==0); 
}

REGISTER_TEST(Test_crGetLastErrorMsgW);
void Test_crGetLastErrorMsgW()
{ 
  // Get error message before Install
  WCHAR szErrMsg[256] = L"";
  int nResult = crGetLastErrorMsgW(szErrMsg, 256);
  TEST_ASSERT(nResult>0);

  // Install crash handler
  CR_INSTALL_INFOW infoW;
  memset(&infoW, 0, sizeof(CR_INSTALL_INFOW));
  infoW.cb = sizeof(CR_INSTALL_INFOW);
  infoW.pszAppVersion = L"1.0.0"; // Specify app version, otherwise it will fail.

  int nInstallResult = crInstallW(&infoW);
  TEST_ASSERT(nInstallResult==0);
  
  // Get error message
  WCHAR szErrMsg2[256] = L"";
  int nResult2 = crGetLastErrorMsgW(szErrMsg2, 256);
  TEST_ASSERT(nResult2>0);

  // Get error message to NULL buffer - must fail  
  int nResult3 = crGetLastErrorMsgW(NULL, 256);
  TEST_ASSERT(nResult3<0);

  // Get error message to a buffer, but zero length - must fail  
  WCHAR szErrMsg3[256] = L"";
  int nResult4 = crGetLastErrorMsgW(szErrMsg3, 0);
  TEST_ASSERT(nResult4<0);

  // Get error message to a single-char buffer, must trunkate message and succeed
  WCHAR szErrMsg5[1] = L"";
  int nResult5 = crGetLastErrorMsgW(szErrMsg5, 1);
  TEST_ASSERT(nResult5>0);

  // Get error message to a small buffer, must trunkate message and succeed
  WCHAR szErrMsg6[2] = L"";
  int nResult6 = crGetLastErrorMsgW(szErrMsg6, 2);
  TEST_ASSERT(nResult6>0);

  __TEST_CLEANUP__;

  // Uninstall
  int nUninstallResult = crUninstall();
  TEST_ASSERT(nUninstallResult==0); 
}

REGISTER_TEST(Test_CrAutoInstallHelper);
void Test_CrAutoInstallHelper()
{   
  // Install crash handler

  CR_INSTALL_INFO info;
  memset(&info, 0, sizeof(CR_INSTALL_INFO));
  info.cb = sizeof(CR_INSTALL_INFO);
  info.pszAppVersion = _T("1.0.0"); // Specify app version, otherwise it will fail.

  CrAutoInstallHelper cr_install_helper(&info);
  TEST_ASSERT(cr_install_helper.m_nInstallStatus==0);
    
  __TEST_CLEANUP__;  
}

REGISTER_TEST(Test_CrThreadAutoInstallHelper);

DWORD WINAPI ThreadProc1(LPVOID lpParam)
{
  // Install thread exception handlers
  CrThreadAutoInstallHelper cr_thread_install(0);
 
  int* pnResult = (int*)lpParam;
  *pnResult = cr_thread_install.m_nInstallStatus;

  // Do something else
  int a = 5 + 10;
  int b = 10*16;
  
  return 0;
}

void Test_CrThreadAutoInstallHelper()
{   
  // Install crash handler for the main thread

  CR_INSTALL_INFO info;
  memset(&info, 0, sizeof(CR_INSTALL_INFO));
  info.cb = sizeof(CR_INSTALL_INFO);
  info.pszAppVersion = _T("1.0.0"); // Specify app version, otherwise it will fail.

  CrAutoInstallHelper cr_install_helper(&info);
  TEST_ASSERT(cr_install_helper.m_nInstallStatus==0);
  
  // Run a worker thread
  int nResult = -1;
  HANDLE hThread = CreateThread(NULL, 0, ThreadProc1, &nResult, 0, NULL);

  // Wait until thread exits
  WaitForSingleObject(hThread, INFINITE);

  TEST_ASSERT(nResult==0);

  __TEST_CLEANUP__;  
}

REGISTER_TEST(Test_crEmulateCrash);
void Test_crEmulateCrash()
{   
  // Test it with invalid argument - should fail
  int nResult = crEmulateCrash(-1);
  TEST_ASSERT(nResult!=0);
  
  // Test it with invalid argument - should fail
  int nResult2 = crEmulateCrash(CR_THROW+1);
  TEST_ASSERT(nResult2!=0);

  __TEST_CLEANUP__;  
}

REGISTER_TEST(Test_crInstallToCurrentThread);

DWORD WINAPI ThreadProc2(LPVOID lpParam)
{  
  // Uninstall before install - should fail
  int nUnResult = crUninstallFromCurrentThread();
  TEST_ASSERT(nUnResult!=0);

  // Install thread exception handlers - should succeed
  int nResult = crInstallToCurrentThread();
  TEST_ASSERT(nResult==0);
  
  // Install thread exception handlers the second time - should fail
  int nResult2 = crInstallToCurrentThread();
  TEST_ASSERT(nResult2!=0);

  // Do something else
  int a = 5 + 10;
  int b = 10*16;

  __TEST_CLEANUP__;

  // Uninstall - should succeed
  int nUnResult2 = crUninstallFromCurrentThread();
  TEST_ASSERT(nUnResult2==0);

  return 0;
}

void Test_crInstallToCurrentThread()
{ 
  // Call before install - must fail
  int nResult = crInstallToCurrentThread();
  TEST_ASSERT(nResult!=0);

  // Call before install - must fail
  int nResult2 = crInstallToCurrentThread2(0);
  TEST_ASSERT(nResult2!=0);

  // Install crash handler for the main thread

  CR_INSTALL_INFO info;
  memset(&info, 0, sizeof(CR_INSTALL_INFO));
  info.cb = sizeof(CR_INSTALL_INFO);
  info.pszAppVersion = _T("1.0.0"); // Specify app version, otherwise it will fail.

  int nInstResult = crInstall(&info);
  TEST_ASSERT(nInstResult==0);
  
  // Call in the main thread - must fail
  int nResult3 = crInstallToCurrentThread2(0);
  TEST_ASSERT(nResult3!=0);

  // Run a worker thread
  HANDLE hThread = CreateThread(NULL, 0, ThreadProc2, NULL, 0, NULL);

  // Wait until thread exits
  WaitForSingleObject(hThread, INFINITE);

  __TEST_CLEANUP__;  

  // Uninstall should succeed
  int nUnResult = crUninstall();
  TEST_ASSERT(nUnResult==0);
}