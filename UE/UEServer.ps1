# Joel Leagues under the Apache v2 Licence

param (
    [string] $Map,
    [string] $IpAddress = "127.0.0.1",
    [string] $Port = "7777",

    [switch] $Second,
    [switch] $Third,

    [switch] $Game,
    [switch] $Server,
    [switch] $Editor,

    [switch] $NoSteam,
    [switch] $NoExec,

    [switch] $help
)

if($help -eq $true)
{
    write-host "

UServer.ps1
    
    -Project   = Project Name (not required)
    -Map       = Give a Map Name (as a Server)
    -IpAddress = Give an Server IP   (Default = 127.0.0.1)
    -Port      = Give an Server Port (Default = 7777)

    -Game      = start as a client
    -Server    = start as a server
    -Editor    = Open the UE5 Editor 

    -First     = Open game on the First Screen (default)
    -Second    = Open game on the second Screen
    -Third     = Open game on the third Screen

    -NoSteam   = Don't use Steam API

    "
    return;
}

if (-not (Get-ChildItem -Path $PWD.Path -Filter *.uproject)) {
    Write-Host "No .uproject file found in $($PWD.Path). Exiting."
    exit 1
}

$LnDisplay = 0
if($Third){
    $LnDisplay = 3
}elseif($Second){
    $LnDisplay = 2
}else{
    $LnDisplay = 1
}

class Core
{
    static[string] $SsEditor = "D:\UE\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe"
    static[string] $SsPort = "7777"
    
    static[string[]] $SvDebugArgs  = @("-log", "-stdout", "-FullStdOutLogOutput")
    static[string[]] $SvGameArgs   = @("-windowed", "-resx=1280", "-resy=720")
    static[string[]] $SvServerArgs = @("-unattended")
    
    [string] $MsUProject;
    [string] $MsMode;
    [bool]   $MbEditor;
    [string] $MsDisplay;
    [string] $MsNoSteam;
    [string] $MsConfigName;

    Core([string] $FsGame, [string] $FbServer, [bool] $MbEditor,
         [int] $FnDisplay, [bool] $FbNoSteam
    ){

        $This.MbEditor = $MbEditor;
        if ($FsGame -eq $false -and $FbServer -eq $false)
        {
            Write-Host "You must select a Game or Server Mode!"
            exit
        }elseif($FsGame -eq $true) # The "-eq $true" is required
        {
            $This.MsMode = "-game"
            if($FnDisplay -eq 2)
            {
                $This.MsDisplay = "-WinX=1920 -WinY=0 -SAVEWINPOS=1"
            }elseif($FnDisplay -eq 3){
                $This.MsDisplay = "-WinX=3840 -WinY=1 -SAVEWINPOS=2"
            }else{
                $This.MsDisplay = "-SAVEWINPOS=0"
            }

        }elseif($FbServer -eq $true){
            $This.MsMode = "-server"
        } # UE's default is the editor

        if ($This.MsNoSteam -and $This.MsNoSteam.Length -gt 0) {
            $This.MsNoSteam = " -nosteam"
        }

        $This.MsUProject = "$($PWD.Path)\$($(Get-ChildItem | Select-Object Name | Where-Object Name -like *uproject).Name)";
        $This.MsConfigName = (Get-ChildItem -Path "$($PWD.Path)\Source" -Directory)[0].Name
    }

    static [string] JoinWithSpaces([string[]]$FvArgs) 
    {
        [string] $LsResult = ""
        for($i = 0; $i -lt $FvArgs.Length; $i++) {
            if ($i -eq 0) {
                $LsResult = $FvArgs[$i]
            } else {
                $LsResult += " ```n"
                $LsResult += "    " + $FvArgs[$i]
            }
        }
        return $LsResult
    }

    [string] GetServerEditorCMD([string] $FsMap)
    {
        $LsMap = $FsMap -replace "\\", "/" -replace "./Content", "/Game" -replace ".umap", "?Listen"
        $LsPort = "-port=" + [Core]::SsPort

        $LvExecution = @([Core]::SsEditor, $This.MsUProject)
        if ($LsMap -and $LsMap.Length -gt 0) {
            $LvExecution += $LsMap
        }
        $LvExecution += @(
            $This.MsMode,
            "-log",
            $LsMap,
            $LsPort
        )
        if ($This.MsNoSteam -and $This.MsNoSteam.Length -gt 0) {
            $LvExecution += $This.MsNoSteam
        }
        return [Core]::JoinWithSpaces($LvExecution);
    }

    [string] GetClientEditorCMD([string] $FsMap, [string] $IpAddress)
    {
        $LsMap = $FsMap -replace "\\", "/" -replace "./Content", "/Game" -replace ".umap", ""
        if($IpAddress.Length -eq 0){
            $IpAddress = "127.0.0.1:" + [Core]::SsPort
        }
        else{
            $IpAddress += ':' + [Core]::SsPort
        }
        $LvExecution = @([Core]::SsEditor, $This.MsUProject)
        if ($LsMap -and $LsMap.Length -gt 0) {
            $LvExecution += "$($LsMap)?$($IpAddress)"
        }
        else{
            $LvExecution += $IpAddress
        }
        $LvExecution += @(
            $This.MsMode,
            "-log",
            $This.MsDisplay
        )
        if($This.MsNoSteam.Length){
            $LvExecution += $This.MsNoSteam
        }
        return [Core]::JoinWithSpaces($LvExecution);
    }

    [string] CommandEditorStr([string] $Map, [string] $IpAddress)
    {
        $LsCommandStr = ""
        if($This.MsMode -eq "-server"){
            $LsCommandStr = $This.GetServerEditorCMD($Map)
        }
        else{
            $LsCommandStr = $This.GetClientEditorCMD($Map, $IpAddress)
        }
        return $LsCommandStr;
    }

    [string] GetServerDebugCMD([string] $FsMap)
    {
        $LsBinary = ".\Binaries\Win64\{0}Server-Win64-DebugGame.exe" -f $This.MsConfigName
        $LsPort = "-port=" + [Core]::SsPort
        $LvExecution = @(
            $LsBinary,
            "-log",
            "-stdout",
            "-FullStdOutLogOutput"
            "-unattended",
            $LsPort
        )
        return [Core]::JoinWithSpaces($LvExecution);
    }

    [string] GetClientDebugCMD([string] $FsMap, [string] $IpAddress)
    {
        $LsBinary = ".\Binaries\Win64\{0}-Win64-DebugGame.exe" -f $This.MsConfigName
        $LsPort = "-port=" + [Core]::SsPort
        $LvExecution = @(
            $LsBinary,
            "-log",
            "-stdout",
            "-FullStdOutLogOutput"
            "-windowed",
            "-resx=1280",
            "-resy=720",
            $LsPort
        )
        return [Core]::JoinWithSpaces($LvExecution);
    }
    
    [string] CommandDebugStr([string] $Map, [string] $IpAddress)
    {
        $LsCommandStr = ""
        if($This.MsMode -eq "-server"){
            $LsCommandStr = $This.GetServerDebugCMD($Map)
        }
        else{
            $LsCommandStr = $This.GetClientDebugCMD($Map, $IpAddress)
        }
        return $LsCommandStr;
    }

    [string] GetCommandStr([string] $Map, [string] $IpAddress)
    {
        if ($This.MbEditor){
            return $This.CommandEditorStr($Map, $IpAddress);
        }
        return $This.CommandDebugStr($Map, $IpAddress);
    }
}

$core = [Core]::new($Game, $Server, $Editor, $LnDisplay, $NoSteam);

$LsCommand = $core.GetCommandStr($Map, $IpAddress);

if($NoExec){
    return $LsCommand
}
else{
    Write-Host $LsCommand "`n"
    Invoke-Expression $LsCommand
}