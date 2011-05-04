@echo off
setlocal
set PARSER=%~dp0..\parser
set BUILD=FormalInput
set BIN=%~dp0bin

if exist %BUILD% (
	rmdir /S /Q %BUILD%
)

call h2xs -Afn %BUILD%

for /F %%f in ('dir /B "%PARSER%"') do (
	copy "%PARSER%\%%f" "%BUILD%"
)

call h2xs -Oan %BUILD%
copy typemap %BUILD%
echo typemap >> %BUILD%\manifest

copy %BUILD%.xs %BUILD%\FormalInput.xs

pushd %BUILD%
perl Makefile.PL
sed -re "s/\{\{\@ARGV/{@ARGV/g" Makefile > Makefile2 || exit 1
move Makefile2 Makefile && nmake || exit 1
mkdir %BIN% 2>nul
copy blib\arch\auto\FormalInput\FormalInput.dll %BIN%
popd

endlocal
