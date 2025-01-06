Write-Host "DieKnow - build win32.x86_64"
Write-Host "==========================================================================="
Write-Host "This is free software; see the source for copying conditions."
Write-Host "There is ABSOLUTELY NO WARRANTY, to the extent permitted by applicable law."

$architecture = if ([System.IntPtr]::Size -eq 8) { "64-bit" } else { "32-bit" }

if ($architecture -ne "64-bit") {
    Write-Host "FATAL: You are attempting to run a 64-bit program on a 32-bit system." -ForegroundColor Red
    $global:LASTEXITCODE = 1
    Exit
}

$cpython = Get-Command python -ErrorAction SilentlyContinue
$cpy = Get-Command py -ErrorAction SilentlyContinue

if($cpy) {
    $py = $cpy
} elseif ($cpython) {
    $py = $cpython
} else {
    Write-Host "FATAL: Python is not installed or not in PATH system environment variable!" -ForegroundColor Red
    Write-Host "Please install it from https://www.python.org/downloads/ or from your organization's Software Center." -ForegroundColor Red
    $global:LASTEXITCODE = 1
    return
}

do {
    Write-Host ""
    Write-Host "Please select a runtime mode and press Enter:"
    Write-Host "1 - DieKnow Shell *"
    Write-Host "2 - DieKnow GUI"
    Write-Host "3 - Generate documentation"
    Write-Host "4 - Exit"
    Write-Host ""
    $choice = Read-Host "Enter a number (1-4)"

    switch ($choice) {
        "1" {
            & $py src/main.py
        }
        "2" {
            & $py src/gui.pyw
        }
        "3" {
            & $py src/dieknow.py -docs
        }
        "4" {
            break
        }
        Default {
            Write-Host "Invalid number! Please enter a number 1 through 4." -ForegroundColor Red
        }
    }
} while ($choice -ne "4")
