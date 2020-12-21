$ErrorActionPreference = "Stop"

Set-Location 'C:\projects\rdkafka'

$task = New-Item 'task.bat' -Force
Add-Content $task "call phpize 2>&1"
Add-Content $task "call configure --with-php-build=C:\build-cache\deps --with-rdkafka --enable-debug-pack 2>&1"
Add-Content $task "nmake /nologo 2>&1"
Add-Content $task "exit %errorlevel%"
& "C:\build-cache\php-sdk-$env:BIN_SDK_VER\phpsdk-$env:VC-$env:ARCH.bat" -t $task
if (-not $?) {
    throw "build failed with errorlevel $LastExitCode"
}
