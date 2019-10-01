
$fasm = Join-Path $PSScriptRoot "fasm"
$fasmTools = Join-Path $PSScriptRoot "fasm-tools"
$fasmExe = Join-Path $fasm "fasm.exe"
$Env:include = Join-Path $fasm "include"
$listingExe = Join-Path $fasmTools "listing.exe"
$prepsrcExe = Join-Path $fasmTools "prepsrc.exe"
$symbolsExe = Join-Path $fasmTools "symbols.exe"

if (-not (Test-Path $fasm) -or (Get-ChildItem $fasm | Measure-Object).Count -eq 0) {
    $url = "https://flatassembler.net/fasmw17316.zip"
    $file = [System.IO.Path]::GetTempFileName()

    Write-Host "Downloading $url to $file..."
    $clnt = new-object System.Net.WebClient
    $clnt.DownloadFile($url,$file)

    Write-Host "Unzipping to $fasm..."
    Expand-Archive $file -DestinationPath $fasm

    Write-Host "Deleting $file..."
    [System.IO.File]::Delete($file)

    Write-Host "Done installing FASM"
} else {
    Write-Host "$fasm is non-empty. Skipping FASM install."
}

if (-not (Test-Path $fasmTools)) {
    Write-Host "Creating $fasmTools folder"
    New-Item -path $PSScriptRoot -Name "fasm-tools" -ItemType "directory" | Out-Null
}

if (-not (Test-Path $listingExe)) {
    Write-Host "Compiling listing.exe"
    & $fasmExe (Join-Path $fasm "tools\win32\listing.asm") $listingExe
}

if (-not (Test-Path $prepsrcExe)) {
    Write-Host "Compiling prepsrc.exe"
    & $fasmExe (Join-Path $fasm "tools\win32\prepsrc.asm") $prepsrcExe
}

if (-not (Test-Path $symbolsExe)) {
    Write-Host "Compiling symbols.exe"
    & $fasmExe (Join-Path $fasm "tools\win32\symbols.asm") $symbolsExe
}

Write-Host "Done"
