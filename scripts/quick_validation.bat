@echo off
setlocal EnableExtensions EnableDelayedExpansion

rem City Life Core quick local validation for Windows.
rem
rem Usage:
rem   scripts\quick_validation.bat [build_dir] [config]
rem
rem Defaults:
rem   build_dir = build-quick-validation
rem   config    = Release
rem
rem Optional:
rem   set CLC_RUN_REPLAY_EXAMPLE=1
rem     Also runs clc_example_replay_persistence.

set "BUILD_DIR=%~1"
if "%BUILD_DIR%"=="" set "BUILD_DIR=build-quick-validation"

set "CONFIG=%~2"
if "%CONFIG%"=="" set "CONFIG=Release"

set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%..") do set "REPO_ROOT=%%~fI"
for %%I in ("%REPO_ROOT%\%BUILD_DIR%") do set "BUILD_PATH=%%~fI"

echo.
echo City Life Core quick validation
echo Repository: %REPO_ROOT%
echo Build dir:  %BUILD_PATH%
echo Config:     %CONFIG%
echo.

call :run cmake -S "%REPO_ROOT%" -B "%BUILD_PATH%" -DCMAKE_BUILD_TYPE=%CONFIG% -DCLC_BUILD_TESTS=ON -DCLC_BUILD_EXAMPLES=ON
if errorlevel 1 exit /b 1

call :run cmake --build "%BUILD_PATH%" --config "%CONFIG%"
if errorlevel 1 exit /b 1

call :run ctest --test-dir "%BUILD_PATH%" --output-on-failure -C "%CONFIG%"
if errorlevel 1 exit /b 1

echo.
echo Running example smoke checks...

call :run_example clc_example_action_bridge
if errorlevel 1 exit /b 1
call :run_example clc_example_basic_runtime
if errorlevel 1 exit /b 1
call :run_example clc_example_custom_data_registry
if errorlevel 1 exit /b 1
call :run_example clc_example_faction_ownership_contracts
if errorlevel 1 exit /b 1
call :run_example clc_example_full_runtime_flow
if errorlevel 1 exit /b 1
call :run_example clc_example_game_profiles
if errorlevel 1 exit /b 1
call :run_example clc_example_save_load_roundtrip
if errorlevel 1 exit /b 1
call :run_example clc_example_tick_runtime
if errorlevel 1 exit /b 1
call :run_example clc_example_validation_hardening
if errorlevel 1 exit /b 1

if "%CLC_RUN_REPLAY_EXAMPLE%"=="1" (
    call :run_example clc_example_replay_persistence
    if errorlevel 1 exit /b 1
) else (
    echo.
    echo Skipping clc_example_replay_persistence. Set CLC_RUN_REPLAY_EXAMPLE=1 to include it.
)

echo.
echo Quick validation completed successfully.
exit /b 0

:run
echo.
echo + %*
%*
if errorlevel 1 (
    echo Command failed with exit code %ERRORLEVEL%: %*
    exit /b %ERRORLEVEL%
)
exit /b 0

:run_example
set "EXAMPLE_NAME=%~1"
set "EXAMPLE_EXE="

for %%P in (
    "%BUILD_PATH%\examples\%CONFIG%\%EXAMPLE_NAME%.exe"
    "%BUILD_PATH%\examples\%EXAMPLE_NAME%.exe"
    "%BUILD_PATH%\%CONFIG%\%EXAMPLE_NAME%.exe"
    "%BUILD_PATH%\%EXAMPLE_NAME%.exe"
) do (
    if exist "%%~fP" (
        set "EXAMPLE_EXE=%%~fP"
        goto :found_example
    )
)

echo Missing example executable: %EXAMPLE_NAME%
echo Checked common CMake output locations under: %BUILD_PATH%
exit /b 1

:found_example
echo.
echo + "!EXAMPLE_EXE!"
"!EXAMPLE_EXE!"
if errorlevel 1 (
    echo Example failed with exit code %ERRORLEVEL%: %EXAMPLE_NAME%
    exit /b %ERRORLEVEL%
)
exit /b 0
