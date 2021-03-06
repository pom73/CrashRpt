/************************************************************************************* 
  This file is a part of CrashRpt library.

  CrashRpt is Copyright (c) 2003, Michael Carruth
  All rights reserved.
 
  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:
 
   * Redistributions of source code must retain the above copyright notice, this 
     list of conditions and the following disclaimer.
 
   * Redistributions in binary form must reproduce the above copyright notice, 
     this list of conditions and the following disclaimer in the documentation 
     and/or other materials provided with the distribution.
 
   * Neither the name of the author nor the names of its contributors 
     may be used to endorse or promote products derived from this software without 
     specific prior written permission.
 

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
  SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************************/

#include "stdafx.h"
#include "DetailDlg.h"
#include "Utility.h"
#include "CrashInfoReader.h"
#include "ErrorReportSender.h"

LRESULT CDetailDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  CString sRTL = Utility::GetINIString(_T("Settings"), _T("RTLReading"));
  if(sRTL.CompareNoCase(_T("1"))==0)
  {
    Utility::SetLayoutRTL(m_hWnd);
  }

  SetWindowText(Utility::GetINIString(_T("DetailDlg"), _T("DlgCaption")));

  m_linkPrivacyPolicy.SubclassWindow(GetDlgItem(IDC_PRIVACYPOLICY));
  m_linkPrivacyPolicy.SetHyperLink(g_CrashInfo.m_sPrivacyPolicyURL);
  m_linkPrivacyPolicy.SetLabel(Utility::GetINIString(_T("DetailDlg"), _T("PrivacyPolicy")));

  if(!g_CrashInfo.m_sPrivacyPolicyURL.IsEmpty())
    m_linkPrivacyPolicy.ShowWindow(SW_SHOW);
  else
    m_linkPrivacyPolicy.ShowWindow(SW_HIDE);

  CStatic statHeader = GetDlgItem(IDC_HEADERTEXT);
  statHeader.SetWindowText(Utility::GetINIString(_T("DetailDlg"), _T("DoubleClickAnItem")));  

  m_list = GetDlgItem(IDC_FILE_LIST);
  m_list.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

  m_list.InsertColumn(0, Utility::GetINIString(_T("DetailDlg"), _T("FieldName")), LVCFMT_LEFT, 120);
  m_list.InsertColumn(1, Utility::GetINIString(_T("DetailDlg"), _T("FieldDescription")), LVCFMT_LEFT, 80);
  m_list.InsertColumn(2, Utility::GetINIString(_T("DetailDlg"), _T("FieldType")), LVCFMT_LEFT, 80);
  m_list.InsertColumn(3, Utility::GetINIString(_T("DetailDlg"), _T("FieldSize")), LVCFMT_RIGHT, 80);

  m_iconList.Create(16, 16, ILC_COLOR32|ILC_MASK, 3, 1);
  m_list.SetImageList(m_iconList, LVSIL_SMALL);

  // Insert items
  WIN32_FIND_DATA   findFileData   = {0};
  HANDLE            hFind          = NULL;
  CString           sSize;
  
  std::map<CString, FileItem>::iterator p;
  unsigned i;
  for (i = 0, p = g_CrashInfo.m_FileItems.begin(); p != g_CrashInfo.m_FileItems.end(); p++, i++)
  {     
	  CString sDestFile = p->first;
    CString sSrcFile = p->second.m_sSrcFile;
    CString sFileDesc = p->second.m_sDesc;

    SHFILEINFO sfi;
    SHGetFileInfo(sSrcFile, 0, &sfi, sizeof(sfi),
      SHGFI_DISPLAYNAME | SHGFI_ICON | SHGFI_TYPENAME | SHGFI_SMALLICON);

    int iImage = -1;
    if(sfi.hIcon)
    {
      iImage = m_iconList.AddIcon(sfi.hIcon);
      DestroyIcon(sfi.hIcon);
    }

    int nItem = m_list.InsertItem(i, sDestFile, iImage);
    
	  CString sFileType = sfi.szTypeName;
    m_list.SetItemText(nItem, 1, sFileDesc);
    m_list.SetItemText(nItem, 2, sFileType);

    hFind = FindFirstFile(sSrcFile, &findFileData);
    if (INVALID_HANDLE_VALUE != hFind)
    {
      FindClose(hFind);
      sSize.Format(TEXT("%d KB"), findFileData.nFileSizeLow/1024);
      m_list.SetItemText(nItem, 3, sSize);
    }    
  }

  m_list.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

  CStatic statPreview = GetDlgItem(IDC_PREVIEWTEXT);
  statPreview.SetWindowText(Utility::GetINIString(_T("DetailDlg"), _T("Preview")));  

  m_btnClose = GetDlgItem(IDOK);
  m_btnClose.SetWindowText(Utility::GetINIString(_T("DetailDlg"), _T("Close")));  

  m_btnExport = GetDlgItem(IDC_EXPORT);
  m_btnExport.SetWindowText(Utility::GetINIString(_T("DetailDlg"), _T("Export")));  

  // center the dialog on the screen
	CenterWindow();  

  return TRUE;
}

LRESULT CDetailDlg::OnItemChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
  LPNMLISTVIEW lpItem           = (LPNMLISTVIEW)pnmh; 
  int iItem                     = lpItem->iItem;

  if (lpItem->uChanged & LVIF_STATE
     && lpItem->uNewState & LVIS_SELECTED)
  {
     SelectItem(iItem);
  }

  return 0;
}

LRESULT CDetailDlg::OnItemDblClicked(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
  LPNMLISTVIEW lpItem           = (LPNMLISTVIEW)pnmh; 
  int iItem                     = lpItem->iItem;
  DWORD_PTR dwRet               = 0;

  if (iItem < 0 || (int)g_CrashInfo.m_FileItems.size() < iItem)
     return 0;

  std::map<CString, FileItem>::iterator p = g_CrashInfo.m_FileItems.begin();
  for (int i = 0; i < iItem; i++, p++);

  CString sFileName = p->second.m_sSrcFile;
  dwRet = (DWORD_PTR)::ShellExecute(0, _T("open"), sFileName,
    0, 0, SW_SHOWNORMAL);
  ATLASSERT(dwRet > 32);

  return 0;
}

void CDetailDlg::SelectItem(int iItem)
{
  const int MAX_FILE_SIZE          = 32768; // 32k file preview max
  DWORD dwBytesRead                = 0;
  BYTE buffer[MAX_FILE_SIZE + 1]  = "";

  // Sanity check
  if (iItem < 0 || (int)g_CrashInfo.m_FileItems.size() < iItem)
      return;

  std::map<CString, FileItem>::iterator p = g_CrashInfo.m_FileItems.begin();
  for (int i = 0; i < iItem; i++, p++);

  //
  // Display file contents in preview window
  //
  CString sFileName = p->second.m_sSrcFile;
  HANDLE hFile = CreateFile(
     sFileName,
     GENERIC_READ,
     FILE_SHARE_READ | FILE_SHARE_WRITE,
     NULL,
     OPEN_EXISTING,
     FILE_ATTRIBUTE_NORMAL,
     0);

  if (hFile!=INVALID_HANDLE_VALUE)
  {
     // Read up to first 32 KB
     ReadFile(hFile, buffer, MAX_FILE_SIZE, &dwBytesRead, 0);
     buffer[dwBytesRead] = 0;
     CloseHandle(hFile);
  }

  // Update edit control with file contents
  ::SetWindowTextA(GetDlgItem(IDC_FILE_EDIT), (char*)buffer);
}

LRESULT CDetailDlg::OnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  EndDialog(0);
  return 0;
}

LRESULT CDetailDlg::OnExport(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  CString sFileName = g_CrashInfo.m_sCrashGUID + _T(".zip");

  CFileDialog dlg(FALSE, _T("*.zip"), sFileName,
      OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT,
      _T("ZIP Files (*.zip)\0*.zip\0All Files (*.*)\0*.*\0\0"), m_hWnd);
  
  INT_PTR result = dlg.DoModal();
  if(result==IDOK)
  {
    CString sExportFileName = dlg.m_szFileName;
    g_ErrorReportSender.SetExportFlag(TRUE, sExportFileName);
    g_ErrorReportSender.DoWork(COMPRESS_REPORT);
  }

  return 0;
}
