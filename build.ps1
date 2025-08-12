# Trading Time Counter - Build Script
# PowerShell script for Windows development

param(
    [string]$BuildType = "Release",
    [switch]$Clean,
    [switch]$Configure,
    [switch]$Install,
    [switch]$Test,
    [string]$Generator = "Visual Studio 17 2022",
    [string]$Architecture = "x64"
)

$ProjectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$BuildDir = Join-Path $ProjectRoot "build"
$InstallDir = Join-Path $ProjectRoot "install"

Write-Host "=== Trading Time Counter Build Script ===" -ForegroundColor Green
Write-Host "Project Root: $ProjectRoot" -ForegroundColor Cyan
Write-Host "Build Type: $BuildType" -ForegroundColor Cyan
Write-Host "Generator: $Generator" -ForegroundColor Cyan
Write-Host "Architecture: $Architecture" -ForegroundColor Cyan

# 清理构建目录
if ($Clean) {
    Write-Host "Cleaning build directory..." -ForegroundColor Yellow
    if (Test-Path $BuildDir) {
        Remove-Item -Path $BuildDir -Recurse -Force
    }
    if (Test-Path $InstallDir) {
        Remove-Item -Path $InstallDir -Recurse -Force
    }
}

# 创建构建目录
if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

# CMake配置
if ($Configure -or $Clean -or -not (Test-Path (Join-Path $BuildDir "CMakeCache.txt"))) {
    Write-Host "Configuring project with CMake..." -ForegroundColor Yellow
    
    Push-Location $BuildDir
    try {
        $configArgs = @(
            "..",
            "-G", $Generator,
            "-A", $Architecture,
            "-DCMAKE_BUILD_TYPE=$BuildType",
            "-DCMAKE_INSTALL_PREFIX=$InstallDir"
        )
        
        & cmake @configArgs
        if ($LASTEXITCODE -ne 0) {
            throw "CMake configuration failed"
        }
    }
    finally {
        Pop-Location
    }
}

# 构建项目
Write-Host "Building project..." -ForegroundColor Yellow
Push-Location $BuildDir
try {
    & cmake --build . --config $BuildType --parallel
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed"
    }
}
finally {
    Pop-Location
}

# 安装
if ($Install) {
    Write-Host "Installing project..." -ForegroundColor Yellow
    Push-Location $BuildDir
    try {
        & cmake --install . --config $BuildType
        if ($LASTEXITCODE -ne 0) {
            throw "Installation failed"
        }
    }
    finally {
        Pop-Location
    }
}

# 运行测试
if ($Test) {
    Write-Host "Running tests..." -ForegroundColor Yellow
    Push-Location $BuildDir
    try {
        & ctest --build-config $BuildType --verbose
    }
    finally {
        Pop-Location
    }
}

Write-Host "Build completed successfully!" -ForegroundColor Green
Write-Host "Executable location: $BuildDir\bin\$BuildType\tradingTimeCounter.exe" -ForegroundColor Cyan

# 显示生成的文件
if (Test-Path "$BuildDir\bin\$BuildType") {
    Write-Host "`nGenerated files:" -ForegroundColor Cyan
    Get-ChildItem "$BuildDir\bin\$BuildType" | ForEach-Object {
        Write-Host "  $($_.Name)" -ForegroundColor White
    }
}
