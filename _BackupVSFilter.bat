@echo off

for /f %%i in ('Dir /b *.sln') do set ProjName=%%~ni

copy .\Intermediate\ProjectFiles\%ProjName%.vcxproj.filters .\%ProjName%.vcxproj.filters.backup

::Remove everything below this line if you don't want to hide the backup

::attrib +h %ProjName%.vcxproj.filters.backup