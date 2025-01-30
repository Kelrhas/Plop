@echo off
cls

"%~dp0Tools\ListGameTargets\ListGameTargets.exe" %~dp0

mkdir build

IF "%~1"=="" (
	cmake build -Wno-dev -DOPENAL_INCLUDE_DIR="C:\Program Files (x86)\OpenAL\include" -DOPENAL_LIBRARY="c:\Program Files (x86)\OpenAL\lib\OpenAL32.lib"
) ELSE (
	echo Debug build:
	cmake build --debug-output --debug-find -Wno-dev -DOPENAL_INCLUDE_DIR="C:\Program Files (x86)\OpenAL\include" -DOPENAL_LIBRARY="c:\Program Files (x86)\OpenAL\lib\OpenAL32.lib"
)

rem Pause if launched from explorer
if /I Not "%CMDCMDLINE:"=%" == "%COMSPEC%" Pause