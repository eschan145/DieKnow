Write-Host "DieKnow - win32.x86_64"
Write-Host "There is ABSOLUTELY NO WARRANTY, to the extent permitted by applicable law."

$architecture = if ([System.IntPtr]::Size -eq 8) { "64-bit" } else { "32-bit" }

if ($architecture -ne "64-bit") {
    Write-Host "FATAL: You are attempting to run a 64-bit program on a 32-bit system."
    $global:LASTEXITCODE = 1
    Exit
}

$python = Get-Command python -ErrorAction SilentlyContinue
$py = Get-Command py -ErrorAction SilentlyContinue

if (-not $python -and -not $py) {
    Write-Host "FATAL: Python is not installed or not in PATH system environment variable!"
    Write-Host "Please install it from https://www.python.org/downloads/ or from your organization's Software Center."
    $global:LASTEXITCODE = 1
    return
}

try {
    if ($py) {
        & py src\main.py
    } elseif ($python) {
        & python src\main.py
    }
} catch {
    Write-Host "FATAL: Failed to run."
    $global:LASTEXITCODE = 1
}
