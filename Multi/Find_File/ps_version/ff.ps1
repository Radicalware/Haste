param($useri, $show_file)

if (!$useri){
    echo '''
By Scourge
Licence: GPLv3

This is the File Finder tool

ff.ps1 <file to search for>

    '''
    exit
}


$folder_array = [System.Collections.ArrayList]@()

$lsFolders = [System.Collections.ArrayList]@()

$loc = (pwd | Select-String -Pattern "^C.*" | Out-String) -replace ("(`n)|(`r`n)","")

$file_content = @((Get-ChildItem -path $loc -Recurse | Out-String).split("`n"))

foreach ($item in $file_content){
    if ($item | Select-String '    Directory:'){
        $lsFolders.Add($item) | Out-Null
    }
}
foreach ($item in $lsFolders){
    $item = $item.replace('    Directory: ','')
    $folder_array.Add($item) | Out-Null
}

#foreach ($folder in $folder_array){$folder}

$Targets = [System.Collections.ArrayList]@()


foreach ($location in $folder_array){
   
    $location = $location -replace (".$")
    $files = @((Get-ChildItem -Path $location| Out-String).split("`n"))
    $file_path = [System.Collections.ArrayList]@()
    foreach($file in $files){
        if ($file -match '-a'){
            $file = (($file -replace '[^:].[0-9]\s{1}','***').split('***')[6]) 
            $file_path.Add("$location\$file")|Out-Null
        }        
    }
    
    $item = ''
    foreach ($item in $file_path){
        $item = $item -replace ("\s*$",'')
        $file_segment = $item -replace  ".*\\",""
        if ($file_segment | Select-String -Pattern $useri){
             if (Test-Path $item){
                $targets.Add($item)|Out-Null
            }
        }
    }
}
echo ''


foreach ($item in $targets){
    write-host ('"'+$item+'"') -ForegroundColor Yellow
}
