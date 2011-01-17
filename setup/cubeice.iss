; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define SrcDir "C:\projects\cubeice"
#define MyAppName "CubeICE"
#define MyAppVersion "0.2.8.2"
#define MyAppPublisher "CubeSoft"
#define MyAppURL "http://www.cube-soft.jp/"
#define MyAppExeName "cubeice.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{C765184D-FE36-4C2C-95CE-9F0C05D61988}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}β
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName=CubeSoft\{#MyAppName}
AllowNoIcons=true
LicenseFile={#SrcDir}\release\GNU General Public License.txt
OutputDir={#SrcDir}
OutputBaseFilename=setup
SetupIconFile={#SrcDir}\release\cubeice.ico
Compression=lzma
SolidCompression=true
InternalCompressLevel=max
VersionInfoVersion={#MyAppVersion}
VersionInfoCompany=CubeSoft
VersionInfoDescription=CubeICE
VersionInfoCopyright=Copyright (c) 2010 CubeSoft
VersionInfoProductName=CubeICE
AlwaysRestart=false
RestartIfNeededByRun=false
PrivilegesRequired=admin

[Languages]
Name: japanese; MessagesFile: compiler:Languages\Japanese.isl

[Tasks]
Name: desktopicon; Description: デスクトップにショートカットを作成する

[Dirs]

[Files]
Source: {#SrcDir}\release\cubeice.exe; DestDir: {app}; Flags: ignoreversion
Source: {#SrcDir}\release\cubeice-setting.exe; DestDir: {app}; Flags: ignoreversion
Source: {#SrcDir}\release\cubeice-exec.exe; DestDir: {app}; Flags: ignoreversion
Source: ..\release\cubeice-checker.exe; DestDir: {app}; Flags: ignoreversion
Source: {#SrcDir}\release\cubeice-file.ico; DestDir: {app}; Flags: ignoreversion
Source: {#SrcDir}\release\cubeicectx.dll; DestDir: {app}; Flags: ignoreversion onlyifdoesntexist
Source: {#SrcDir}\release\GNU General Public License.txt; DestDir: {app}; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
Source: {#SrcDir}\release\7z.dll; DestDir: {app}; Flags: ignoreversion
Source: {#SrcDir}\release\7z.sfx; DestDir: {app}; Flags: ignoreversion
Source: ..\release\Readme.txt; DestDir: {app}; Flags: ignoreversion

[Icons]
Name: {group}\{#MyAppName} 設定; Filename: {app}\cubeice-setting.exe; IconIndex: 0
Name: {group}\{cm:ProgramOnTheWeb,CubeICE}; Filename: http://www.cube-soft.jp/
Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe}
Name: {userdesktop}\{#MyAppName} 圧縮; Filename: {app}\cubeice.exe; Parameters: /c:zip; IconFilename: {app}\cubeice.exe; IconIndex: 1; Tasks: desktopicon
Name: {userdesktop}\{#MyAppName} 解凍; Filename: {app}\cubeice.exe; Parameters: /x; IconFilename: {app}\cubeice.exe; IconIndex: 2; Tasks: desktopicon

[Run]
Filename: {app}\cubeice-setting.exe; Description: CubeICE 設定を起動する; Flags: postinstall nowait shellexec

[Registry]
Root: HKCR; Subkey: CLSID\{{F3DB85F4-4731-4e80-BC2E-754A7320D830}; ValueType: string; ValueData: CubeICE; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: CLSID\{{F3DB85F4-4731-4e80-BC2E-754A7320D830}\InProcServer32; ValueType: string; ValueData: """{app}\cubeicectx.dll"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: CLSID\{{F3DB85F4-4731-4e80-BC2E-754A7320D830}\InProcServer32; ValueType: string; ValueData: Apartment; Permissions: system-full admins-full users-read; Flags: uninsdeletekey; ValueName: ThreadingModel
Root: HKCR; Subkey: *\shellex\ContextMenuHandlers\cubeice; ValueType: string; ValueData: {{F3DB85F4-4731-4e80-BC2E-754A7320D830}; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: Folder\shellex\ContextMenuHandlers\cubeice; ValueType: string; ValueData: {{F3DB85F4-4731-4e80-BC2E-754A7320D830}; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_zip; ValueType: string; ValueData: ZIP ファイル; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_zip\shell; ValueType: string; ValueData: open; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_zip\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""/x"" ""%1"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_zip\DefaultIcon; ValueType: string; ValueData: {app}\cubeice-file.ico,0; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_lzh; ValueType: string; ValueData: LZH ファイル; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_lzh\shell; ValueType: string; ValueData: open; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_lzh\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""/x"" ""%1"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_lzh\DefaultIcon; ValueType: string; ValueData: {app}\cubeice-file.ico,0; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_rar; ValueType: string; ValueData: RAR ファイル; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_rar\shell; ValueType: string; ValueData: open; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_rar\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""/x"" ""%1"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_rar\DefaultIcon; ValueType: string; ValueData: {app}\cubeice-file.ico,0; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_tar; ValueType: string; ValueData: TAR ファイル; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_tar\shell; ValueType: string; ValueData: open; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_tar\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""/x"" ""%1"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_tar\DefaultIcon; ValueType: string; ValueData: {app}\cubeice-file.ico,0; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_gz; ValueType: string; ValueData: GZ ファイル; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_gz\shell; ValueType: string; ValueData: open; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_gz\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""/x"" ""%1"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_gz\DefaultIcon; ValueType: string; ValueData: {app}\cubeice-file.ico,0; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_7z; ValueType: string; ValueData: 7Z ファイル; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_7z\shell; ValueType: string; ValueData: open; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_7z\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""/x"" ""%1"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_7z\DefaultIcon; ValueType: string; ValueData: {app}\cubeice-file.ico,0; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_arj; ValueType: string; ValueData: ARJ ファイル; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_arj\shell; ValueType: string; ValueData: open; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_arj\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""/x"" ""%1"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_arj\DefaultIcon; ValueType: string; ValueData: {app}\cubeice-file.ico,0; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_bz2; ValueType: string; ValueData: BZ2 ファイル; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_bz2\shell; ValueType: string; ValueData: open; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_bz2\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""/x"" ""%1"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_bz2\DefaultIcon; ValueType: string; ValueData: {app}\cubeice-file.ico,0; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_cab; ValueType: string; ValueData: CAB ファイル; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_cab\shell; ValueType: string; ValueData: open; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_cab\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""/x"" ""%1"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_cab\DefaultIcon; ValueType: string; ValueData: {app}\cubeice-file.ico,0; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_chm; ValueType: string; ValueData: CHM ファイル; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_chm\shell; ValueType: string; ValueData: open; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_chm\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""/x"" ""%1"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_chm\DefaultIcon; ValueType: string; ValueData: {app}\cubeice-file.ico,0; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_cpio; ValueType: string; ValueData: CPIO ファイル; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_cpio\shell; ValueType: string; ValueData: open; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_cpio\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""/x"" ""%1"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_cpio\DefaultIcon; ValueType: string; ValueData: {app}\cubeice-file.ico,0; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_deb; ValueType: string; ValueData: DEB ファイル; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_deb\shell; ValueType: string; ValueData: open; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_deb\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""/x"" ""%1"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_deb\DefaultIcon; ValueType: string; ValueData: {app}\cubeice-file.ico,0; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_dmg; ValueType: string; ValueData: DMG ファイル; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_dmg\shell; ValueType: string; ValueData: open; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_dmg\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""/x"" ""%1"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_dmg\DefaultIcon; ValueType: string; ValueData: {app}\cubeice-file.ico,0; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_iso; ValueType: string; ValueData: ISO ファイル; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_iso\shell; ValueType: string; ValueData: open; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_iso\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""/x"" ""%1"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_iso\DefaultIcon; ValueType: string; ValueData: {app}\cubeice-file.ico,0; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_rpm; ValueType: string; ValueData: RPM ファイル; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_rpm\shell; ValueType: string; ValueData: open; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_rpm\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""/x"" ""%1"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_rpm\DefaultIcon; ValueType: string; ValueData: {app}\cubeice-file.ico,0; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_tbz; ValueType: string; ValueData: TBZ ファイル; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_tbz\shell; ValueType: string; ValueData: open; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_tbz\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""/x"" ""%1"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_tbz\DefaultIcon; ValueType: string; ValueData: {app}\cubeice-file.ico,0; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_tgz; ValueType: string; ValueData: TGZ ファイル; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_tgz\shell; ValueType: string; ValueData: open; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_tgz\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""/x"" ""%1"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_tgz\DefaultIcon; ValueType: string; ValueData: {app}\cubeice-file.ico,0; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_wim; ValueType: string; ValueData: WIM ファイル; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_wim\shell; ValueType: string; ValueData: open; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_wim\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""/x"" ""%1"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_wim\DefaultIcon; ValueType: string; ValueData: {app}\cubeice-file.ico,0; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_xar; ValueType: string; ValueData: XAR ファイル; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_xar\shell; ValueType: string; ValueData: open; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_xar\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""/x"" ""%1"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_xar\DefaultIcon; ValueType: string; ValueData: {app}\cubeice-file.ico,0; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_xz; ValueType: string; ValueData: XZ ファイル; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_xz\shell; ValueType: string; ValueData: open; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_xz\shell\open\command; ValueType: string; ValueData: """{app}\{#MyAppExeName}"" ""/x"" ""%1"""; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKCR; Subkey: cubeice_xz\DefaultIcon; ValueType: string; ValueData: {app}\cubeice-file.ico,0; Permissions: system-full admins-full users-read; Flags: uninsdeletekey
Root: HKLM; Subkey: Software\CubeSoft\CubeICE; Flags: uninsdeletekey
Root: HKLM; Subkey: Software\CubeSoft\CubeICE; ValueType: string; ValueName: InstallDirectory; ValueData: {app}; Flags: uninsdeletevalue
Root: HKLM; Subkey: Software\CubeSoft\CubeICE; ValueType: string; ValueName: Version; ValueData: {#MyAppVersion}β; Flags: uninsdeletevalue
Root: HKCU; Subkey: Software\CubeSoft\CubeICE; Flags: uninsdeletekey
Root: HKCU; Subkey: Software\CubeSoft\CubeICE; ValueType: dword; ValueName: ContextFlags; ValueData: 3; Flags: createvalueifdoesntexist uninsdeletevalue
Root: HKCU; Subkey: Software\CubeSoft\CubeICE; ValueType: string; ValueName: Filter; ValueData: .DS_Store<>__MACOSX<>desktop.ini; Flags: createvalueifdoesntexist uninsdeletevalue
Root: HKCU; Subkey: Software\CubeSoft\CubeICE; ValueType: dword; ValueName: Initialize; ValueData: 1; Flags: uninsdeletevalue
Root: HKCU; Subkey: Software\CubeSoft\CubeICE\Compress; ValueType: dword; ValueName: Details; ValueData: 129; Flags: createvalueifdoesntexist uninsdeletevalue
Root: HKCU; Subkey: Software\CubeSoft\CubeICE\Compress; ValueType: dword; ValueName: OutputCondition; Flags: createvalueifdoesntexist uninsdeletevalue; ValueData: 2
Root: HKCU; Subkey: Software\CubeSoft\CubeICE\Decompress; ValueType: dword; ValueName: Details; ValueData: 493; Flags: createvalueifdoesntexist uninsdeletevalue
Root: HKCU; Subkey: Software\CubeSoft\CubeICE\Decompress; ValueType: dword; ValueName: OutputCondition; Flags: createvalueifdoesntexist uninsdeletevalue; ValueData: 1

[UninstallDelete]
Name: {app}\*; Type: filesandordirs
Name: {app}; Type: dirifempty
[Code]
#include "windows.iss"
#include "process.iss"

// ------------------------------------------------------------------------- //
//  const variables
// ------------------------------------------------------------------------- //
const
  SHCNF_FLUSH = $1000;
  SHCNE_ASSOCCHANGED = $08000000;
  PREV_ARCHIVER = 'PrevArchiver';

// ------------------------------------------------------------------------- //
//  Win32APIs
// ------------------------------------------------------------------------- //
procedure SHChangeNotify(wEventId, uFlags, dwItem1, dwItem2: Integer);
	external 'SHChangeNotify@shell32.dll stdcall';


// -------------------------------------------------------------------------- //
// helper procedures for writing registory when installing
// -------------------------------------------------------------------------- //
procedure savePrevRegValue(extension, srcKey, destKey, filterValue: string);
var
  prevValue:string;
begin
  if RegValueExists(HKCR, extension, srcKey) then
    if (RegQueryStringValue(HKCR, extension, srcKey, prevValue)) then
      if (prevValue <> filterValue) then
        RegWriteStringValue(HKCR, extension, destKey, prevValue);
end;

procedure saveAndWriteValue(extension, writeKey, archiveKey, writeValue: string);
begin
  savePrevRegValue(extension, writeKey, archiveKey, writeValue);
  RegWriteStringValue(HKCR, extension, writeKey, writeValue);
end;

// ------------------------------------------------------------------------- //
//  InitializeSetup
// ------------------------------------------------------------------------- //
function InitializeSetup(): Boolean;
begin
    if (not TerminateRunningProcess('cubeice.exe', False)) then begin
		Result := False;
		exit;
    end;

    if (not TerminateRunningProcess('cubeice-setting.exe', False)) then begin
		Result := False;
		exit;
    end;

    if (not TerminateRunningProcess('cubeice-checker.exe', False)) then begin
		Result := False;
		exit;
    end;

    Result := True;
end;

// ------------------------------------------------------------------------- //
//  InitializeUninstall
// ------------------------------------------------------------------------- //
function InitializeUninstall(): Boolean;
begin
	TerminateRunningProcess('cubeice.exe', True);
	TerminateRunningProcess('cubeice-setting.exe', True);
	TerminateRunningProcess('cubeice-checker.exe', True);
	Result := True;
end;

// ------------------------------------------------------------------------- //
//  CurStepChanged
// ------------------------------------------------------------------------- //
procedure CurStepChanged(CurStep: TSetupStep);
var
    ErrCode: Integer;
begin
	if CurStep = ssDone then begin
		if (IsDotNetDetected('v2.0.50727', 0) = True) then begin
			RegWriteStringValue(HKCU, 'Software\Microsoft\Windows\CurrentVersion\Run', 'cubeice-checker', '"' + ExpandConstant('{app}\cubeice-checker.exe') + '"');
			Exec(ExpandConstant('{app}\cubeice-checker.exe'), '', '', SW_SHOW, ewNoWait, ErrCode);
		end;

		saveAndWriteValue('.zip', '', PREV_ARCHIVER, 'cubeice_zip');
		saveAndWriteValue('.7z', '', PREV_ARCHIVER, 'cubeice_7z');
		saveAndWriteValue('.lzh', '', PREV_ARCHIVER, 'cubeice_lzh');
		saveAndWriteValue('.tar', '', PREV_ARCHIVER, 'cubeice_tar');
		saveAndWriteValue('.rar', '', PREV_ARCHIVER, 'cubeice_rar');
		saveAndWriteValue('.cab', '', PREV_ARCHIVER, 'cubeice_cab');
		saveAndWriteValue('.gz', '', PREV_ARCHIVER, 'cubeice_gz');
		saveAndWriteValue('.bz2', '', PREV_ARCHIVER, 'cubeice_bz2');
		saveAndWriteValue('.xz', '', PREV_ARCHIVER, 'cubeice_xz');
		saveAndWriteValue('.tgz', '', PREV_ARCHIVER, 'cubeice_tgz');
		saveAndWriteValue('.tbz', '', PREV_ARCHIVER, 'cubeice_tbz');
		SHChangeNotify(SHCNE_ASSOCCHANGED,SHCNF_FLUSH,0,0);
	end;
end;

// ------------------------------------------------------------------------- //
//  helper procedures for unintalling
// ------------------------------------------------------------------------- //
procedure getbackPrevValue(extension, srcKey, archivedKey: string);
var
  prevValue: string;
begin
  if RegValueExists(HKCR, extension, archivedKey) then begin
    if (RegQueryStringValue(HKCR, extension, archivedKey, prevValue)) then begin
		RegWriteStringValue(HKCR, extension, srcKey, prevValue);
		RegDeleteValue(HKCR, extension ,archivedKey);
    end;
  end else begin
		RegDeleteValue(HKCR, extension, srcKey);
  end;
end;

// ------------------------------------------------------------------------- //
//  CurUninstallStepChanged
// ------------------------------------------------------------------------- //
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
	if CurUninstallStep = usPostUninstall then begin
		RegDeleteValue(HKCU, 'Software\Microsoft\Windows\CurrentVersion\Run', 'cubeice-checker');
		DeleteFile(ExpandConstant('{userdesktop}')+'\CubeICE 圧縮.lnk');
		DeleteFile(ExpandConstant('{userdesktop}')+'\CubeICE 解凍.lnk');
		DeleteFile(ExpandConstant('{userdesktop}')+'\CubeICE 設定.lnk');

		getbackPrevValue('.zip', '', PREV_ARCHIVER);
		getbackPrevValue('.lzh', '', PREV_ARCHIVER);
		getbackPrevValue('.rar', '', PREV_ARCHIVER);
		getbackPrevValue('.tar', '', PREV_ARCHIVER);
		getbackPrevValue('.gz', '', PREV_ARCHIVER);
		getbackPrevValue('.7z', '', PREV_ARCHIVER);
		getbackPrevValue('.arj', '', PREV_ARCHIVER);
		getbackPrevValue('.bz2', '', PREV_ARCHIVER);
		getbackPrevValue('.cab', '', PREV_ARCHIVER);
		getbackPrevValue('.chm', '', PREV_ARCHIVER);
		getbackPrevValue('.cpio', '', PREV_ARCHIVER);
		getbackPrevValue('.deb', '', PREV_ARCHIVER);
		getbackPrevValue('.dmg', '', PREV_ARCHIVER);
		getbackPrevValue('.iso', '', PREV_ARCHIVER);
		getbackPrevValue('.rpm', '', PREV_ARCHIVER);
		getbackPrevValue('.tbz', '', PREV_ARCHIVER);
		getbackPrevValue('.tgz', '', PREV_ARCHIVER);
		getbackPrevValue('.wim', '', PREV_ARCHIVER);
		getbackPrevValue('.xar', '', PREV_ARCHIVER);
		getbackPrevValue('.xz', '', PREV_ARCHIVER);

		SHChangeNotify(SHCNE_ASSOCCHANGED,SHCNF_FLUSH,0,0);
	end;
end;
