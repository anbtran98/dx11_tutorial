### PowerShell script for compiling and run project
# Clear Console
Clear-Host

# Regenerate cmake files in build folder
if ( ($args.count -gt 0) -and (($args[0]) -eq "regen") ) {
    Write-Host "`nRegenerate Build Folder ---------------------" -ForegroundColor Yellow
    cmake -B build/ -S .
    if (($args[1] -eq "stop")) {
        exit $LASTEXITCODE
    }
}

# Build Project  ---------------------------------------------------------
Write-Host "`nBuilding Project ------------------------------" -ForegroundColor Yellow
cmake --build build/
if ($LASTEXITCODE -ne 0) {
    Write-Host "`nCmake Build Error :: build FAILED with exit code: $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}
if (($args[0] -eq "build")) { exit $LASTEXITCODE }

# Running Project --------------------------------------------------------
Write-Host "`nBuild Done" -ForegroundColor Green
Write-Host "-----------------------------------------------" -ForegroundColor Green
Write-Host "Running ..." -Foreground Green
pushd .\src
..\bin\Debug\dx11
popd
if ($LASTEXITCODE -ne 0) {
    Write-Host "`nRuntime Error :: exit code: $LASTEXITCODE" -Foreground Red
    exit $LASTEXITCODE
}
Write-Host "`nProgram Stopped" -ForegroundColor Green
