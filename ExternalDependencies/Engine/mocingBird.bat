@echo off
setlocal DisableDelayedExpansion
for /f "tokens=*" %%a IN ('dir /b *.h') DO (
find /c "Q_OBJECT" %%a
if not errorlevel 1 (
echo "inside conditional moc" %%a
C:\moc.exe %%a > moc_%%~na.cpp
)
)
exit 0