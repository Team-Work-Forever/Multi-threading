
param (
    [string]$FOLDER = "out",
    [switch]$help = $false
)

$OUT_FOLDER= Test-Path ".\$FOLDER"

function helpMenu {

    Write-Output "Usage: generate [arguments]

    Generate a out folder with especific files
    
    options:
      -help   Show help menu
      -folder   Especifies out folder
      "
    
}

function generateReport {

    Write-Output "Creating output folder..."
    mkdir ".\$FOLDER" > $null

    Write-Output "Copy data to $FOLDER folder..."
    Copy-Item ".\src\*" ".\$FOLDER"

    Write-Output "Setup bin folder..."
    mkdir ".\$FOLDER\bin" > $null

    Write-Output "Copy binary executable to bin folder..."
    Copy-Item ".\build\Debug\main.exe" ".\$FOLDER\bin\."
    
}

if ($help)
{
    helpMenu;
    exit 0;
}

if ( $OUT_FOLDER )
{
    Write-Output "Already exists an $FOLDER folder!"
} else 
{
    generateReport;
    Write-Output "Report Generated with sucess!"
}