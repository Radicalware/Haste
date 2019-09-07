$loc = $(Get-Location).path

set-location $PSScriptRoot
copy-item .\Build\Windows\Release\Release\bin\* "$home\programs\"
set-location $loc
