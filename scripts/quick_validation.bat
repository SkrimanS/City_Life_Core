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
rem
rem   set CLC_RUN_INSTALL_CONSUMERS=1
rem     Also installs the SDK into the quick build directory and builds/runs
rem     the installed C++ and C ABI consumer examples.

set "BUILD_DIR=%~1"
if "%BUILD_DIR%"=="" set "BUILD_DIR=build-quick-validation"

set "CONFIG=%~2"
if "%CONFIG%"=="" set "CONFIG=Release"

set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%..") do set "REPO_ROOT=%%~fI"
for %%I in ("%REPO_ROOT%\%BUILD_DIR%") do set "BUILD_PATH=%%~fI"
set "INSTALL_PREFIX=%BUILD_PATH%\installed-sdk"
set "CPP_CONSUMER_BUILD=%BUILD_PATH%\consumer-cpp"
set "C_ABI_CONSUMER_BUILD=%BUILD_PATH%\consumer-c-abi"

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

if "%CLC_RUN_INSTALL_CONSUMERS%"=="1" (
    call :run_installed_consumers
    if errorlevel 1 exit /b 1
) else (
    echo.
    echo Skipping installed consumer checks. Set CLC_RUN_INSTALL_CONSUMERS=1 to include them.
)

echo.
echo Quick validation completed successfully.
exit /b 0

:run
echo.
echo + %*
%*
if errorlevel 1 (
    set "EXIT_CODE=!ERRORLEVEL!"
    echo Command failed with exit code !EXIT_CODE!: %*
    exit /b !EXIT_CODE!
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
    set "EXIT_CODE=!ERRORLEVEL!"
    echo Example failed with exit code !EXIT_CODE!: %EXAMPLE_NAME%
    exit /b !EXIT_CODE!
)
exit /b 0

:run_installed_consumers
echo.
echo Running installed consumer checks...

call :run cmake --install "%BUILD_PATH%" --config "%CONFIG%" --prefix "%INSTALL_PREFIX%"
if errorlevel 1 exit /b 1

call :run cmake -S "%REPO_ROOT%\examples\find_package_consumer" -B "%CPP_CONSUMER_BUILD%" -DCMAKE_PREFIX_PATH="%INSTALL_PREFIX%"
if errorlevel 1 exit /b 1
call :run cmake --build "%CPP_CONSUMER_BUILD%" --config "%CONFIG%"
if errorlevel 1 exit /b 1
call :run_consumer "%CPP_CONSUMER_BUILD%" city_life_core_consumer
if errorlevel 1 exit /b 1

call :run cmake -S "%REPO_ROOT%\examples\c_abi_consumer" -B "%C_ABI_CONSUMER_BUILD%" -DCMAKE_PREFIX_PATH="%INSTALL_PREFIX%"
if errorlevel 1 exit /b 1
call :run cmake --build "%C_ABI_CONSUMER_BUILD%" --config "%CONFIG%"
if errorlevel 1 exit /b 1
call :run_consumer "%C_ABI_CONSUMER_BUILD%" city_life_core_c_abi_consumer
if errorlevel 1 exit /b 1

exit /b 0

:run_consumer
set "CONSUMER_BUILD=%~1"
set "CONSUMER_NAME=%~2"
set "CONSUMER_EXE="

for %%P in (
    "%CONSUMER_BUILD%\%CONFIG%\%CONSUMER_NAME%.exe"
    "%CONSUMER_BUILD%\%CONSUMER_NAME%.exe"
) do (
    if exist "%%~fP" (
        set "CONSUMER_EXE=%%~fP"
        goto :found_consumer
    )
)

echo Missing consumer executable: %CONSUMER_NAME%
echo Checked common CMake output locations under: %CONSUMER_BUILD%
exit /b 1

:found_consumer
echo.
echo + "!CONSUMER_EXE!"
"!CONSUMER_EXE!"
if errorlevel 1 (
    set "EXIT_CODE=!ERRORLEVEL!"
    echo Consumer failed with exit code !EXIT_CODE!: %CONSUMER_NAME%
    exit /b !EXIT_CODE!
)
exit /b 0
