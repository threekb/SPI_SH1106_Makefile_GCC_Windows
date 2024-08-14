@echo off
REM Clean build directory by removing .o, .d, and .lst files

REM Define the build directory
set BUILD_DIR=build

REM Check if the build directory exists
if not exist %BUILD_DIR% (
    echo Build directory does not exist.
    exit /b 1
)

REM Remove .o files
echo Deleting .o files...
del /q %BUILD_DIR%\*.o

REM Remove .d files
echo Deleting .d files...
del /q %BUILD_DIR%\*.d

REM Remove .lst files
echo Deleting .lst files...
del /q %BUILD_DIR%\*.lst

echo Clean completed.
exit /b 0
