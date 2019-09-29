
$target = Join-Path $PSScriptRoot "fasm"
if ((Test-Path $target) -and (Get-ChildItem $target | Measure-Object).Count -gt 0) {
    Write-Host "$target is non-empty. Quitting."
    Exit
}

$url = "https://flatassembler.net/fasmw17316.zip"
$file = [System.IO.Path]::GetTempFileName()

Write-Host "Downloading $url to $file..."
$clnt = new-object System.Net.WebClient
$clnt.DownloadFile($url,$file)

Write-Host "Unzipping to $target..."
Expand-Archive $file -DestinationPath $target

Write-Host "Deleting $file..."
[System.IO.File]::Delete($file)

Write-Host "Done"
