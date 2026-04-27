@echo off
echo ========================================================
echo Reconstruyendo los archivos de Visual Studio (C++)
echo ========================================================

:: Ruta absoluta a tu motor Unreal Engine 4.27
set UBT="C:\Program Files\Epic Games\UE_4.27\Engine\Binaries\DotNET\UnrealBuildTool.exe"

:: Ejecuta la herramienta usando la ruta dinamica de tu proyecto
%UBT% -projectfiles -project="%~dp0GalagaModificadoMac.uproject" -game -rocket -progress

echo.
echo Proceso finalizado. 
pause