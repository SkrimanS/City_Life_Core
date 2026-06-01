@echo off
setlocal EnableExtensions EnableDelayedExpansion

rem City Life Core full local validation wrapper for Windows.
rem
rem Usage:
rem   scripts\full_validation.bat [build_dir] [config]
rem
rem Defaults:
rem   build_dir = build-full-validation
rem   config    = Release
rem
rem This wrapper runs quick_validation.bat with all optional checks enabled,
rem captures console output, copies useful test/build logs, and creates a ZIP
rem archive that can be shared for review.

set "BUILD_DIR=%~1"
if "%BUILD_DIR%"=="" set "BUILD_DIR=build-full-validation"

set "CONFIG=%~2"
if "%CONFIG%"=="" set "CONFIG=Release"

set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%..") do set "REPO_ROOT=%%~fI"
for %%I in ("%REPO_ROOT%\%BUILD_DIR%") do set "BUILD_PATH=%%~fI"

set "LOG_DIR=%BUILD_PATH%\validation-logs"
set "RUN_LOG=%LOG_DIR%\full-validation-output.log"
set "SUMMARY_LOG=%LOG_DIR%\summary.txt"
set "ARCHIVE_PATH=%BUILD_PATH%\city-life-core-validation-logs.zip"

if not exist "%BUILD_PATH%" mkdir "%BUILD_PATH%"
if exist "%LOG_DIR%" rmdir /s /q "%LOG_DIR%"
mkdir "%LOG_DIR%"

echo City Life Core full validation > "%SUMMARY_LOG%"
echo Repository: %REPO_ROOT% >> "%SUMMARY_LOG%"
echo Build dir: %BUILD_PATH% >> "%SUMMARY_LOG%"
echo Config: %CONFIG% >> "%SUMMARY_LOG%"
echo Started: %DATE% %TIME% >> "%SUMMARY_LOG%"
echo. >> "%SUMMARY_LOG%"

echo.
echo City Life Core full validation
echo Repository: %REPO_ROOT%
echo Build dir:  %BUILD_PATH%
echo Config:     %CONFIG%
echo Log dir:    %LOG_DIR%
echo.
echo Running all quick validation checks. Console output is being saved to:
echo %RUN_LOG%
echo.

set "CLC_RUN_REPLAY_EXAMPLE=1"
set "CLC_RUN_INSTALL_CONSUMERS=1"

call "%SCRIPT_DIR%quick_validation.bat" "%BUILD_DIR%" "%CONFIG%" > "%RUN_LOG%" 2>&1
set "VALIDATION_EXIT_CODE=%ERRORLEVEL%"

echo Exit code: %VALIDATION_EXIT_CODE% >> "%SUMMARY_LOG%"
echo Finished: %DATE% %TIME% >> "%SUMMARY_LOG%"

call :copy_if_exists "%BUILD_PATH%\Testing\Temporary\LastTest.log" "%LOG_DIR%\LastTest.log"
call :copy_if_exists "%BUILD_PATH%\CMakeFiles\CMakeOutput.log" "%LOG_DIR%\CMakeOutput.log"
call :copy_if_exists "%BUILD_PATH%\CMakeFiles\CMakeError.log" "%LOG_DIR%\CMakeError.log"
call :copy_if_exists "%BUILD_PATH%\CMakeCache.txt" "%LOG_DIR%\CMakeCache.txt"

if exist "%ARCHIVE_PATH%" del /f /q "%ARCHIVE_PATH%"

powershell -NoProfile -ExecutionPolicy Bypass -Command "Compress-Archive -Path '%LOG_DIR%\*' -DestinationPath '%ARCHIVE_PATH%' -Force"
if errorlevel 1 (
    echo.
    echo Failed to create ZIP archive with PowerShell.
    echo Logs are still available in:
    echo %LOG_DIR%
    exit /b %VALIDATION_EXIT_CODE%
)

echo.
echo Validation log archive:
echo %ARCHIVE_PATH%
echo.

if not "%VALIDATION_EXIT_CODE%"=="0" (
    echo Full validation finished with failures. Send this archive for review:
    echo %ARCHIVE_PATH%
    exit /b %VALIDATION_EXIT_CODE%
)

echo Full validation completed successfully. Send this archive if review is needed:
echo %ARCHIVE_PATH%
exit /b 0

:copy_if_exists
set "SOURCE_FILE=%~1"
set "TARGET_FILE=%~2"
if exist "%SOURCE_FILE%" copy /y "%SOURCE_FILE%" "%TARGET_FILE%" >nul
exit /b 0
