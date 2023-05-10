@ECHO OFF
REM "Building app..."

make -f "./Makefile.mak" all
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

ECHO "App has been built successfully."
