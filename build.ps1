# Function to extract version from main.cpp
function Get-Version
{
    $filePath = "src/main.cpp"
    $content = Get-Content -Path $filePath -Raw

    # Use regex to extract version
    $versionPattern = '.*string VERSION = "(.+?)";'
    $versionMatch = [regex]::Match($content, $versionPattern)

    if ($versionMatch.Success)
    {
        return $versionMatch.Groups[1].Value
    }
    else
    {
        Write-Host "Error: Version not found in main.cpp. Please check the file."
        exit 1
    }
}

# Function to increment version
function Increment-Version
{
    $version = Get-Version

    # Check if version has 3 or 4 digits
    if ($version -match '^\d+\.\d+\.\d+$') {
        # Increment the last digit by 0.1
        $newVersion = [version]$version
        $newVersion = $newVersion.Major,$newVersion.Minor,($newVersion.Build + 0.1) -join '.'
    }
    elseif ($version -match '^\d+\.\d+\.\d+\.\d+$') {
        # Increment the last digit by 1
        $newVersion = [version]$version
        $newVersion = $newVersion.Major,$newVersion.Minor,$newVersion.Build,($newVersion.Revision + 1)
        $newVersion = $newVersion -join '.'
    }
    else {
        Write-Host "Error: Unsupported version format. Please use a version with 3 or 4 digits."
        exit 1
    }

    # Update the version in main.cpp
    $filePath = "src/main.cpp"
    (Get-Content -Path $filePath -Raw) -replace 'string VERSION = "(.+?)";', "string VERSION = `"$newVersion`";" | Set-Content -Path $filePath
}

# Function to build the C++ program
function Build-Program
{
    $outputFolder = "builds"
    if (-not(Test-Path $outputFolder))
    {
        New-Item -ItemType Directory -Path $outputFolder | Out-Null
    }

    # Get all .cpp files in the src/ directory
    $cppFiles = Get-ChildItem -Path "src\" -Filter "*.cpp" | ForEach-Object { $_.FullName }

    $cppFileList = $cppFiles -join " "
    $version = Get-Version
    $outputFileName = Join-Path $outputFolder "PortKnocker-$version.exe"
    $compileCommand = "g++ -o $outputFileName $cppFileList -I.\libcurl\include -static-libgcc -static-libstdc++ -lpthread -lws2_32 -Wcpp -w"

    Invoke-Expression $compileCommand

    if ($LASTEXITCODE -eq 0)
    {
        Write-Host "Build successful. Output file: $outputFileName"
    }
    else
    {
        Write-Host "Build failed. Please check the compilation command and errors."
        exit 1
    }
}

# Build the C++ program
Build-Program
Increment-Version