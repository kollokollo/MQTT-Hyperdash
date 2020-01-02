; File for Inno-Setup for MQTT-Hyperdash for WINDOWS  (c) by Markus Hoffmann
; Innosetup kann mit wine benutzt werden.
;
;

#define MyAppName "MQTT-Hyperdash"
#define MyAppURL "https://gitlab.com/kollo/mqtt-hyperdash"
#define MyAppExeName "hyperdash.exe"
#define MyAppVersion "1.00"
#define MyAppRelease "1"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{4B22E38E-73C8-4199-8C9F-8E4B8A00A97C}
AppName={#MyAppName}
AppVerName={#MyAppName} V.{#MyAppVersion} 
AppVersion={#MyAppVersion}-{#MyAppRelease}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
AppPublisherURL={#MyAppURL}


; Since no icons will be created in "{group}", we don't need the wizard
; to ask for a Start Menu folder name:
; DisableProgramGroupPage=yes
UninstallDisplayIcon={app}\hyperdash.ico

OutputDir=Inno-Output
AppCopyright=Copyright (C) 2019-2020 Markus Hoffmann
LicenseFile=../COPYING 
InfoBeforeFile=../RELEASE_NOTES
InfoAfterFile=INTRO
OutputBaseFilename={#MyAppName}-{#MyAppVersion}-{#MyAppRelease}-setup


[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1


[Files]
Source: "hyperdash.exe"; DestDir: "{app}"
Source: "WINDOWS/lib/SDL.dll"; DestDir: "{sys}"; Flags: sharedfile onlyifdoesntexist  32bit
Source: "WINDOWS/demo.dash"; DestName: "demo.dash"; DestDir: "{app}/examples"
Source: "WINDOWS/readme-windows.txt"; DestDir: "{app}"; Flags: isreadme
Source: "WINDOWS/hyperdash.ico"; DestDir: "{app}"
Source: "../doc/manual/Outputs/MQTT-Hyperdash-manual-1.00.pdf"; DestName: "MQTT-Hyperdash-manual.pdf"; DestDir: "{app}/doc"
Source: "WINDOWS/bas.ico"; DestDir: "{app}"
Source: "WINDOWS/lib/README-SDL.txt"; DestDir: "{app}/contrib" ;  Flags: isreadme


[Icons]
Name: "{group}\X11-Basic"; Filename: "{app}\{#MyAppExeName}" ; IconFilename: "{app}\hyperdash.ico"
Name: "{commondesktop}\MQTT-Hyperdash"; Filename: "{app}\{#MyAppExeName}" ; IconFilename: "{app}\hyperdash.ico"
Name: "{group}\{cm:ProgramOnTheWeb,{#MyAppName}}"; Filename: "{#MyAppURL}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon
Name: "{group}\MQTT-Hyperdash manual"; Filename: "{app}\doc\MQTT-Hyperdash-manual.pdf" 
Name: "{group}\Demo"; Filename: "{app}\examples\demo.dash" 

[Registry]
Root: HKCR; Subkey: ".dash"; ValueType: string; ValueName: ""; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".dash\ShellNew"; ValueType: string; ValueName: "NullFile"; ValueData: ""; Flags: uninsdeletevalue

;Local: HKCR; Subkey: "SOFTWARE\\{#MyAppName}"; ValueType: string; ValueName: "path"; ValueData: "{app}\"; Flags: uninsdeletevalue
;Local: HKCR; Subkey: "SOFTWARE\\{#MyAppName}"; ValueType: string; ValueName: "librarypath"; ValueData: "{app}\lib\"; Flags: uninsdeletevalue
;Local: HKCR; Subkey: "SOFTWARE\\{#MyAppName}"; ValueType: string; ValueName: "font"; ValueData: "swiss13"; Flags: uninsdeletevalue
;Local: HKCR; Subkey: "SOFTWARE\\{#MyAppName}"; ValueType: string; ValueName: "geometry"; ValueData: "+10+10"; Flags: uninsdeletevalue
;Local: HKCR; Subkey: "SOFTWARE\\{#MyAppName}"; ValueType: string; ValueName: "version"; ValueData: "{MyAppVersion}"; Flags: uninsdeletevalue

Root: HKCR; Subkey: "{#MyAppName}"; ValueType: string; ValueName: ""; ValueData: "MQTT Hyperdashboard"; Flags: uninsdeletekey
Root: HKCR; Subkey: "{#MyAppName}\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\dash.ico,0"
Root: HKCR; Subkey: "{#MyAppName}\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"" ""%2"" ""%3""" 
Root: HKCR; Subkey: "{#MyAppName}\shell\New"; ValueType: string; ValueName: ""; ValueData: "&Edit" 
Root: HKCR; Subkey: "{#MyAppName}\shell\New\command"; ValueType: string; ValueName: ""; ValueData: "{sys}\Notepad.exe" 
Root: HKCR; Subkey: "{#MyAppName}\shell\ViewDocu"; ValueType: string; ValueName: ""; ValueData: "&View docu" 
Root: HKCR; Subkey: "{#MyAppName}\shell\ViewDocu\command"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName} ""-doc_%1""" 

[Run]
Filename: "{app}\hyperdash.exe"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
