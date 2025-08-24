
param(
    [switch]$Game,
    [switch]$Server,
    [switch]$Editor
)

$DoEditor = $Editor -or (-not $Game -and -not $Server -and -not $Editor)
$DoGame   = $Game   -or (-not $Game -and -not $Server -and -not $Editor)
$DoServer = $Server -or (-not $Game -and -not $Server -and -not $Editor)

if (-not (Get-ChildItem -Path $PWD.Path -Filter *.uproject)) {
    Write-Host "No .uproject file found in $($PWD.Path). Exiting."
    exit 1
}

class Core
{
    static[string] $SsBuildTool = "D:\UE\UnrealEngine\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"
    static[string] $SsAssembly = "Win64"
    static[string[]] $SsExtraArgs = @("-Progress", "-NoHotReload")

    [string] $MsUProject;
    [string] $MsProjectName;

    Core()
    {
        $This.MsUProject = "$($PWD.Path)\$($(Get-ChildItem | Select-Object Name | Where-Object Name -like *uproject).Name)";
        # $This.MsProjectName = [System.IO.Path]::GetFileNameWithoutExtension($This.MsUProject);
        $SourceDirs = Get-ChildItem -Path "$($PWD.Path)\Source" -Directory
        if ($SourceDirs.Count -eq 1) {
            $This.MsProjectName = $SourceDirs[0].Name
        } else {
            throw "Expected exactly one directory in .\Source, found $($SourceDirs.Count)."
        }
    }

    static [void] PrintFullLine() {
        $width = $(Get-Host).UI.RawUI.WindowSize.Width
        Write-Host ('=' * $width)
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

    [string] GetBuildDevelopmentEditorCMD()
    {
        $LsExeName = $This.MsProjectName + "Editor"
        $LsBuildType = "Development"
        $LsProject = "-Project=" + $This.MsUProject
        $LsTarget = "-TargetType=Editor"

        $LsComand = [Core]::JoinWithSpaces(@(
            [Core]::SsBuildTool,
            $LsExeName,
            [Core]::SsAssembly,
            $LsBuildType,
            $LsProject,
            $LsTarget
        ) + [Core]::SsExtraArgs)
        
        Write-Host $LsComand "`n"
        return $LsComand
    }

    [string] GetBuildDebugGameCMD()
    {
        $LsExeName = $This.MsProjectName
        $LsBuildType = "DebugGame"
        $LsProject = "-Project=" + $This.MsUProject
        $LsTarget = "-TargetType=Game"

        $LsComand = [Core]::JoinWithSpaces(@(
            [Core]::SsBuildTool,
            $LsExeName,
            [Core]::SsAssembly,
            $LsBuildType,
            $LsProject,
            $LsTarget
        ) + [Core]::SsExtraArgs)

        Write-Host $LsComand "`n"
        return $LsComand
    }

    [string] GetBuildDebugServerCMD()
    {
        $LsExeName = $This.MsProjectName + "Server"
        $LsBuildType = "DebugGame"
        $LsProject = "-Project=" + $This.MsUProject
        $LsTarget = "-TargetType=Server"

        $LsComand = [Core]::JoinWithSpaces(@(
            [Core]::SsBuildTool,
            $LsExeName,
            [Core]::SsAssembly,
            $LsBuildType,
            $LsProject,
            $LsTarget
        ) + [Core]::SsExtraArgs)

        Write-Host $LsComand "`n"
        return $LsComand
    }
};

try 
{
    $LoCore = [Core]::new()
    if ($DoEditor) {
        [Core]::PrintFullLine()
        Invoke-Expression $LoCore.GetBuildDevelopmentEditorCMD()
    }
    if ($DoGame) {
        [Core]::PrintFullLine()
        Invoke-Expression $LoCore.GetBuildDebugGameCMD()
    }
    if ($DoServer) {
        [Core]::PrintFullLine()
        Invoke-Expression $LoCore.GetBuildDebugServerCMD()
    }
    [Core]::PrintFullLine()
}
catch {
    Write-Host "An error occurred: $_"
}


