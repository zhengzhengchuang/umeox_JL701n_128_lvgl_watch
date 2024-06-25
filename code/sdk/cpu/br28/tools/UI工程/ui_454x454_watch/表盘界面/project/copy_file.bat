@echo off

setlocal enabledelayedexpansion
..\..\..\UITools\style_table -stylefile project.bin -prj 0x1
copy .\project.bin ..\..\..\..\ui_resource\watch.sty
copy .\result.bin  ..\..\..\..\ui_resource\watch.res
copy .\result.str  ..\..\..\..\ui_resource\watch.str
copy .\version.txt  ..\..\..\..\ui_resource\watch.json
copy .\project.tab ..\..\..\..\ui_resource\watch.tab
..\..\..\UITools\redefined -infile ename.h -prefix dial -outfile rename.h
if exist "..\..\..\..\..\..\..\apps\watch\include\ui\" (
	..\..\..\UITools\FileCompare.exe .\rename.h
	if !errorlevel! neq 1 (
    	copy .\rename.h     ..\..\..\..\..\..\..\apps\watch\include\ui\style_watch.h
	)
)

call ..\..\..\UITools\sdk_compile.bat
exit
