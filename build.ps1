$ErrorActionPreference = 'Stop'
Push-Location $PSScriptRoot
try {
    New-Item -ItemType Directory -Force build | Out-Null
    & g++ -std=c++17 -O2 -Wall -Wextra -Wno-misleading-indentation -static -static-libgcc -static-libstdc++ -mwindows main.cpp -o build/SudokuStudio.exe -lgdi32 -luser32
    if ($LASTEXITCODE -ne 0) { throw 'Game build failed' }
    & g++ -std=c++17 -O2 -Wall -Wextra tests.cpp -o build/tests.exe
    if ($LASTEXITCODE -ne 0) { throw 'Test build failed' }
    & ./build/tests.exe
    if ($LASTEXITCODE -ne 0) { throw 'Tests failed' }
    Write-Host 'Ready: build/SudokuStudio.exe'
} finally { Pop-Location }
