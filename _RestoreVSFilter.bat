@echo off

for /f %%i in ('Dir /b *.sln') do set ProjName=%%~ni

attrib -h ./Intermediate/ProjectFiles/%ProjName%.vcxproj.filters

copy %ProjName%.vcxproj.filters.backup .\Intermediate\ProjectFiles\%ProjName%.vcxproj.filters

::If you don't want to keep the backup uncomment the line below by removing the preceding colons

::del %ProjName%.vcxproj.filters.backup

