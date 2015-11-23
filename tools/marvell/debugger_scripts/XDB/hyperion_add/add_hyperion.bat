
@echo off

echo Add Hyperion to XDB in %MARVELL_XDB_HOME%

set THOME=%MARVELL_XDB_HOME%\xdb\tci\cs\jtag

initool "%THOME%\config1\jtag.ini" -m jtag1.ini
initool "%THOME%\config2\jtag.ini" -m jtag2.ini
initool "%THOME%\config3\jtag.ini" -m jtag3.ini

set THOME=%MARVELL_XDB_HOME%xdb\configurations\jtag\SoC\hyperion
if not exist "%THOME%" mkdir "%THOME%"

copy hyperion_3core.xsf "%THOME%"

@echo on

