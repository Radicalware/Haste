# Presented by Joel Leagues under the Apache v2 Licence

param (
    [string] $Project, # this is only here so it assumes the first arg
    [string] $Map,
    [string] $IpAddress = "127.0.0.1",
    [string] $Port = "7777",

    [switch] $Second,
    [switch] $Third,

    [switch] $Game,
    [switch] $Server,
    [switch] $Editor,

    [switch] $NoSteam,

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

    -Second    = Open game on the second Screen
    -Third     = Open game on the third Screen

    -NoSteam   = Don't use Steam API

    "
    return;
}


class Core
{
    [string] $mode;
    [string] $Editor;
    [string] $uproject;
    [string] $display;

    Core([string] $Game, [string] $Server, [string] $Editor,
         [switch] $Second, [switch] $Third
    ){

        if ($Game -eq $false -and $Server -eq $false -and $Editor -eq $false)
        {
            Write-Host "You must select a Game or Server Mode!"
            exit
        }elseif($Game -eq $true) # The "-eq $true" is required
        {
            $this.mode = "-game"
            if($Second -eq $true)
            {
                $this.display = "-WinX=1920 -WinY=0 -SAVEWINPOS=1"
            }elseif($Third -eq $true){
                $this.display = "-WinX=3840 -WinY=1 -SAVEWINPOS=2"
            }else{
                $this.display = "-SAVEWINPOS=0"
            }

        }elseif($Server -eq $true)
        {
            $this.mode = "-server"
        }# UE4E's default is the editor

        $this.editor = 'D:\UE\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe';
        $this.uproject = "$($PWD.Path)\$($(Get-ChildItem | Select-Object Name | Where-Object Name -like *uproject).Name)";
    }

    [string] CommandStr([string] $Map, [string] $IpAddress, [bool] $NoSteam)
    {
        $Map = $Map -replace "\\", "/" -replace "./Content", "/Game" -replace ".umap", ""


        if ($this.mode -eq "-server" -and $Map.Length -gt 0)
        {
            $Map = $Map + "?Listen"
        }

        if($IpAddress.Length -gt 0)
        {
            $IpAddress += ":7777"
        }

        $NetworkOption = ""
        if($NoSteam -eq $true)
        {
            $NetworkOption = " -nosteam"
        }


        $execution = $this.editor + " " + $this.uproject + " " +  $IpAddress + " " + $Map  + " " + $this.mode + " -log " + $this.display + $NetworkOption
        Write-Host $execution;
        return $execution;
    }
}

$core = [Core]::new($Game, $Server, $Editor, $Second, $Third);



Invoke-Expression $core.CommandStr($Map, $IpAddress, $NoSteam);

