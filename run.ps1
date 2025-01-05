Write-Host "DieKnow - win32 x86_64"
Write-Host "There is ABSOLUTELY NO WARRANTY, to the extent permitted by applicable law."

try {
    & py src\main.py
} catch {
    try {
        & python src\main.py
    } catch {
        Write-Host "FATAL: Both 'py' and 'python' command failed. Python may not be installed or in PATH system environment variable!"
    }
}
