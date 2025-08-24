param(
    [switch]$Development,
    [switch]$Shipping
)

# Check if neither switch is provided
if (-not $Development -and -not $Shipping) {
    Write-Error "❌ You must specify either -Development or -Shipping."
    exit 1
}

# Assign the method based on the switch
if ($Development) {
    $LsMethod = "Development"
} elseif ($Shipping) {
    $LsMethod = "Shipping"
}

Write-Host "✅ Selected mode: $LsMethod"

# Set working directory
$ProjectRoot = $PWD.Path

# Find .uproject file
$UProject = Get-ChildItem -Path $ProjectRoot -Filter *.uproject -File | Select-Object -First 1

if (-not $UProject) {
    Write-Error "❌ No .uproject file found in $ProjectRoot"
    Write-Host "`nPress any key to exit..."
    $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
    exit 1
}

$UProjectPath = $UProject.FullName
$ArchiveDirectory = Join-Path $ProjectRoot "Build"
$RunUatPath = "D:\UE\UnrealEngine\Engine\Build\BatchFiles\RunUAT.bat"

# BuildCookRun for Client
Write-Host "`n🚀 Running BuildCookRun for Client..."
$ClientProcess = Start-Process -FilePath $RunUatPath -ArgumentList @(
    "BuildCookRun",
    "-noP4",
    "-platform=Win64",
    "-clientconfig=$LsMethod",
    "-serverconfig=$LsMethod",
    "-cook", "-allmaps", "-build", "-stage", "-pak", "-archive",
    "-project=`"$UProjectPath`"",
    "-archivedirectory=`"$ArchiveDirectory`""
) -PassThru
$ClientProcess.WaitForExit()

if ($ClientProcess.ExitCode -ne 0) {
    Write-Error "❌ Client BuildCookRun failed with exit code $($ClientProcess.ExitCode). Aborting server build."
    Write-Host "`nPress any key to exit..."
    $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
    exit $ClientProcess.ExitCode
}

# BuildCookRun for Server
Write-Host "`n🛠️  Running BuildCookRun for Server..."
$ServerProcess = Start-Process -FilePath $RunUatPath -ArgumentList @(
    "BuildCookRun",
    "-noP4",
    "-platform=Win64",
    "-server",
    "-clientconfig=$LsMethod",
    "-serverconfig=$LsMethod",
    "-cook", "-allmaps", "-build", "-stage", "-pak",
    "-project=`"$UProjectPath`"",
    "-archivedirectory=`"$ArchiveDirectory`""
) -PassThru

$ServerProcess.WaitForExit()

if ($ServerProcess.ExitCode -ne 0) {
    Write-Error "❌ Server BuildCookRun failed with exit code $($ServerProcess.ExitCode)."
    Write-Host "`nPress any key to exit..."
    $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
    exit $ServerProcess.ExitCode
}

Write-Host "`n✅ BuildCookRun completed successfully for both Client and Server."
Write-Host "Press any key to exit..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")