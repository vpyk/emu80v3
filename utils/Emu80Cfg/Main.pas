{
   Emu80Cfg
   Copyright (C) 2006-2013 Viktor Pykhonin <pyk@mail.ru>
 
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
}

// main.pas

unit Main;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Buttons, ExtCtrls;

type

  { TMainForm }

  TMainForm = class(TForm)
    Bevel1: TBevel;
    Panel1: TPanel;
    GroupBox1: TGroupBox;
    VideoRadioGroup: TRadioGroup;
    AudioRadioGroup: TRadioGroup;
    GroupBox2: TGroupBox;
    SpeedRadioGroup: TRadioGroup;
    TextRadioGroup: TRadioGroup;
    TapeRadioGroup: TRadioGroup;
    SoundRadioGroup: TRadioGroup;
    ModelRadioGroup: TRadioGroup;
    BitBtn1: TBitBtn;
    BitBtn2: TBitBtn;
    Panel2: TPanel;
    GroupBox3: TGroupBox;
    RkCheckBox: TCheckBox;
    RkrCheckBox: TCheckBox;
    RkpCheckBox: TCheckBox;
    RkmCheckBox: TCheckBox;
    RkaCheckBox: TCheckBox;
    RksCheckBox: TCheckBox;
    RkoCheckBox: TCheckBox;
    RkuCheckBox: TCheckBox;
    Rk8CheckBox: TCheckBox;
    GamCheckBox: TCheckBox;
    BruCheckBox: TCheckBox;
    OdiCheckBox: TCheckBox;
    RssCheckBox: TCheckBox;
    BitBtn3: TBitBtn;
    BitBtn4: TBitBtn;
    BitBtn5: TBitBtn;
    BitBtn6: TBitBtn;
    SaveDialog: TSaveDialog;
    FullscreenRadioGroup: TRadioGroup;
    Label1: TLabel;
    NameLabel: TLabel;
    Button1: TButton;
    OpenDialog: TOpenDialog;
    procedure FormCreate(Sender: TObject);
    procedure BitBtn1Click(Sender: TObject);
    procedure WriteCfgFile(sCfgFile:string);
    procedure BitBtn2Click(Sender: TObject);
    procedure BitBtn5Click(Sender: TObject);
    procedure BitBtn3Click(Sender: TObject);
    procedure BitBtn4Click(Sender: TObject);
    procedure Button1Click(Sender: TObject);
  private
    { Private declarations }
    sDir:string;
    bDirRegged:boolean;
    procedure ReadCfg;
    procedure DisplayDir;
  public
    { Public declarations }
  end;

var
  MainForm: TMainForm;

implementation

{$R *.lfm}

Uses IniFiles, Registry, ShlObj;

procedure TMainForm.FormCreate(Sender: TObject);
var reg:TRegistry;
    s:string;
begin
bDirRegged:=true;
sDir:='';
reg:=TRegistry.Create;
reg.RootKey:=HKEY_LOCAL_MACHINE;
if reg.OpenKey('Software\Emu80', false) then
  begin
  try
    sDir:=reg.ReadString('InstallDir');
  except
  end;
  reg.CloseKey;
  end;
reg.Free;
if sDir='' then
  begin
  GetDir(0,sDir); //получаем текущую директорию
  bDirRegged:=false;
  end;
DisplayDir;
ReadCfg;
end;

procedure TMainForm.BitBtn1Click(Sender: TObject);
var sDir2:string;
begin
GetDir(0,sDir2); //получаем текущую директорию
WriteCfgFile(sDir2+'\emu80.cfg');
end;

procedure TMainForm.WriteCfgFile(sCfgFile:string);
var ifCfg:TIniFile;
    s:string;
begin
ifCfg:=TiniFile.Create(sCfgFile);

s:='';
case VideoRadioGroup.ItemIndex of
  0: s:='auto';
  1: s:='directx';
  2: s:='windib';
end;
ifCfg.WriteString('config', 'videodriver', s);

s:='';
case AudioRadioGroup.ItemIndex of
  0: s:='auto';
  1: s:='dsound';
  2: s:='waveout';
  3: s:='none';
end;
ifCfg.WriteString('config', 'audiodriver', s);

s:='';
case FullscreenRadioGroup.ItemIndex of
  0: s:='off';
  1: s:='on';
end;
ifCfg.WriteString('config', 'fullscreen', s);

s:='';
case ModelRadioGroup.ItemIndex of
  0: s:='rk86';
  1: s:='partner';
  2: s:='mikrosha';
  3: s:='apogey';
  4: s:='spec';
  5: s:='orion';
  6: s:='ut88';
  7: s:='mikro80';
end;
ifCfg.WriteString('ini', 'model', s);

s:='';
case TextRadioGroup.ItemIndex of
  0: s:='8x16';
  1: s:='original';
end;
ifCfg.WriteString('ini', 'textmode', s);

s:='';
case TapeRadioGroup.ItemIndex of
  0: s:='file';
  1: s:='out';
end;
ifCfg.WriteString('ini', 'tape', s);

s:='';
case SoundRadioGroup.ItemIndex of
  0: s:='on';
  1: s:='off';
end;
ifCfg.WriteString('ini', 'sound', s);

s:='';
case SpeedRadioGroup.ItemIndex of
  0: s:='normal';
  1: s:='medium';
  2: s:='high';
end;
ifCfg.WriteString('ini', 'speed', s);

ifCfg.Destroy;
end;

procedure TMainForm.BitBtn2Click(Sender: TObject);
begin
if SaveDialog.Execute then
  WriteCfgFile(SaveDialog.FileName);
end;

procedure TMainForm.BitBtn5Click(Sender: TObject);
//const FTA_AlwaysShowExt=$00040000;
var reg: TRegistry;
    sExePath:string;
    sDir2:string;
//    dwEditFlags:dword;

  procedure AssociateExt(sExt:string; sParam:string; sDesc:string; nIco:integer);
  begin
  if sParam<>'' then sParam:=' %1 '+sParam else sParam:=' %1';
  if reg.OpenKey(sExt, true) then
    begin
    reg.WriteString('', 'Emu80'+sExt);
    reg.CloseKey;
    if reg.OpenKey('Emu80'+sExt, true) then
      begin
      reg.WriteString('', sDesc);
//      dwEditFlags:=FTA_AlwaysShowExt;
//      reg.WriteInteger('EditFlags',FTA_AlwaysShowExt);
      reg.CloseKey;
      end;
    if reg.OpenKey('Emu80'+sExt+'\Shell\Open\Command', true) then
      begin
      reg.WriteString('', sExePath+sParam);
      reg.CloseKey;
      end;
    if reg.OpenKey('Emu80'+sExt+'\DefaultIcon', true) then
      begin
      reg.WriteString('', sExePath+','+IntToStr(nIco));
      reg.CloseKey;
      end;
    end;
  end;

  procedure DeAssociateExt(sExt:string);
  begin
  reg.DeleteKey(sExt);
  reg.DeleteKey('Emu80'+sExt);
  end;

begin
GetDir(0,sDir2);
sExePath:=sDir2+'\Emu80SDL.exe';
//sExePath:='d:\projects\emu80sdl\emu80sdl.exe';

reg:=TRegistry.Create;
reg.RootKey:=HKEY_CLASSES_ROOT;
//reg.LazyWrite:=false;

if RkCheckBox.Checked then AssociateExt('.rk','-r', 'Emu80 Tape File',1) else DeAssociateExt('.rk');
if RkrCheckBox.Checked then AssociateExt('.rkr','-r', 'RK-86 File',2) else DeAssociateExt('.rkr');
if RkpCheckBox.Checked then AssociateExt('.rkp','-p', 'Partner File',3) else DeAssociateExt('.rkp');
if RkmCheckBox.Checked then AssociateExt('.rkm','-m', 'Mikrosha File',4) else DeAssociateExt('.rkm');
if RkaCheckBox.Checked then AssociateExt('.rka','-a', 'Apogey File',5) else DeAssociateExt('.rka');
if RksCheckBox.Checked then AssociateExt('.rks','-s', 'Specialist File',6) else DeAssociateExt('.rks');
if RkoCheckBox.Checked then AssociateExt('.rko','-o', 'Orion File',7) else DeAssociateExt('.rko');
if RkuCheckBox.Checked then AssociateExt('.rku','-u', 'UT-88 File',8) else DeAssociateExt('.rku');
if Rk8CheckBox.Checked then AssociateExt('.rk8','-8', 'Mikro-80 File',9) else DeAssociateExt('.rk8');
if BruCheckBox.Checked then AssociateExt('.bru','-o', 'Orion Ordos File',10) else DeAssociateExt('.bru');
if GamCheckBox.Checked then AssociateExt('.gam','-r', 'RK-86 Game File',11) else DeAssociateExt('.gam');
if OdiCheckBox.Checked then AssociateExt('.odi','-o', 'Orion Disk Image',12) else DeAssociateExt('.odi');
if RssCheckBox.Checked then AssociateExt('.rss','', 'Emu80 Snapshot',13) else DeAssociateExt('.rss');

reg.Free;
SHChangeNotify(SHCNE_ASSOCCHANGED,SHCNF_IDLIST,nil,nil);
end;

procedure TMainForm.BitBtn3Click(Sender: TObject);
begin
RkCheckBox.Checked:=true;
RkrCheckBox.Checked:=true;
RkpCheckBox.Checked:=true;
RkmCheckBox.Checked:=true;
RkaCheckBox.Checked:=true;
RksCheckBox.Checked:=true;
RkoCheckBox.Checked:=true;
RkuCheckBox.Checked:=true;
Rk8CheckBox.Checked:=true;
BruCheckBox.Checked:=true;
OdiCheckBox.Checked:=true;
GamCheckBox.Checked:=true;
RssCheckBox.Checked:=true;
end;

procedure TMainForm.BitBtn4Click(Sender: TObject);
begin
RkCheckBox.Checked:=false;
RkrCheckBox.Checked:=false;
RkpCheckBox.Checked:=false;
RkmCheckBox.Checked:=false;
RkaCheckBox.Checked:=false;
RksCheckBox.Checked:=false;
RkoCheckBox.Checked:=false;
RkuCheckBox.Checked:=false;
Rk8CheckBox.Checked:=false;
BruCheckBox.Checked:=false;
OdiCheckBox.Checked:=false;
GamCheckBox.Checked:=false;
RssCheckBox.Checked:=false;
end;

procedure TMainForm.Button1Click(Sender: TObject);
var p:integer;
    reg:TRegistry;
begin
if OpenDialog.Execute then
  begin
  sDir:=OpenDialog.FileName;
  p:=Length(sDir);
  while (p>0) and (sDir[p]<>'\') do dec(p);
  if (p<>0) then
    sDir:=copy(sDir,1,p-1);
  DisplayDir;
  ReadCfg;
  reg:=TRegistry.Create;
  reg.RootKey:=HKEY_LOCAL_MACHINE;
  if reg.OpenKey('Software\Emu80', true) then
    begin
    reg.WriteString('InstallDir', sDir);
    reg.CloseKey;
    bDirRegged:=true;
    end;
  end;
end;

procedure TMainForm.DisplayDir;
begin
NameLabel.Caption:=sDir;
if (bDirRegged) then
  NameLabel.Font.Color:=clWindowText
else
  NameLabel.Font.Color:=clInactiveCaption;
end;

procedure TMainForm.ReadCfg;
var ifCfg:TIniFile;
    reg:TRegistry;
    sSpeed,sModel,sText,sTape,sSound,sVideo,sAudio,sFullscreen:string;

  function IsAssociated(sExt:string):boolean;
  begin
  Result:=reg.OpenKey(sExt, false);
  if Result then
    reg.CloseKey;
  end;

begin
ifCfg:=TiniFile.Create(sDir+'\emu80.cfg'); // ini-файл в тек. дир-ии

sVideo:=ifCfg.ReadString('config', 'videodriver', 'directx');
sAudio:=ifCfg.ReadString('config', 'audiodriver', 'auto');
sFullscreen:=ifCfg.ReadString('config', 'fullscreen', 'off');

sSpeed:=ifCfg.ReadString('ini', 'speed', 'normal');
sModel:=ifCfg.ReadString('ini', 'model', 'rk86');
sText:=ifCfg.ReadString('ini', 'textmode', '8x16');
sTape:=ifCfg.ReadString('ini', 'tape', 'file');
sSound:=ifCfg.ReadString('ini', 'sound', 'on');

if sVideo='auto' then VideoRadioGroup.ItemIndex:=0
else if sVideo='directx' then VideoRadioGroup.ItemIndex:=1
else if sVideo='windib' then VideoRadioGroup.ItemIndex:=2;

if sAudio='auto' then AudioRadioGroup.ItemIndex:=0
else if sAudio='dsound' then AudioRadioGroup.ItemIndex:=1
else if sAudio='waveout' then AudioRadioGroup.ItemIndex:=2
else if sAudio='none' then AudioRadioGroup.ItemIndex:=3;

if sFullscreen='off' then FullscreenRadioGroup.ItemIndex:=0
else if sFullscreen='on' then FullscreenRadioGroup.ItemIndex:=1;

if sModel='rk86' then ModelRadioGroup.ItemIndex:=0
else if sModel='partner' then ModelRadioGroup.ItemIndex:=1
else if sModel='mikrosha' then ModelRadioGroup.ItemIndex:=2
else if sModel='apogey' then ModelRadioGroup.ItemIndex:=3
else if sModel='spec' then ModelRadioGroup.ItemIndex:=4
else if sModel='orion' then ModelRadioGroup.ItemIndex:=5
else if sModel='ut88' then ModelRadioGroup.ItemIndex:=6
else if sModel='mikro80' then ModelRadioGroup.ItemIndex:=7;

if sText='8x16' then TextRadioGroup.ItemIndex:=0
else if sText='original' then TextRadioGroup.ItemIndex:=1;

if sTape='file' then TapeRadioGroup.ItemIndex:=0
else if sTape='out' then TapeRadioGroup.ItemIndex:=1;
TapeRadioGroup.Controls[2].Enabled:=false;
AudioRadioGroup.Controls[2].Enabled:=false;

if sSound='on' then SoundRadioGroup.ItemIndex:=0
else if sSound='off' then SoundRadioGroup.ItemIndex:=1;

if sSpeed='normal' then SpeedRadioGroup.ItemIndex:=0
else if sSpeed='medium' then SpeedRadioGroup.ItemIndex:=1
else if sSpeed='high' then SpeedRadioGroup.ItemIndex:=2;

ifCfg.Free;

reg:=TRegistry.Create;
reg.RootKey:=HKEY_CLASSES_ROOT;

RkCheckBox.Checked:=IsAssociated('.rk');
RkrCheckBox.Checked:=IsAssociated('.rkr');
RkpCheckBox.Checked:=IsAssociated('.rkp');
RkmCheckBox.Checked:=IsAssociated('.rkm');
RkaCheckBox.Checked:=IsAssociated('.rka');
RksCheckBox.Checked:=IsAssociated('.rks');
RkoCheckBox.Checked:=IsAssociated('.rko');
RkuCheckBox.Checked:=IsAssociated('.rku');
Rk8CheckBox.Checked:=IsAssociated('.rk8');
RssCheckBox.Checked:=IsAssociated('.rss');
OdiCheckBox.Checked:=IsAssociated('.odi');
GamCheckBox.Checked:=IsAssociated('.gam');
BruCheckBox.Checked:=IsAssociated('.bru');

reg.CloseKey;
reg.free;
end;

end.
