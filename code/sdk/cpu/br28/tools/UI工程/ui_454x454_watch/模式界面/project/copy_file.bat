@echo off

setlocal enabledelayedexpansion

..\..\..\UITools\style_table -stylefile project.bin -prj 0x0
copy .\project.bin ..\..\..\..\ui_resource\JL.sty
copy .\result.bin  ..\..\..\..\ui_resource\JL.res
copy .\result.str  ..\..\..\..\ui_resource\JL.str
if exist "..\..\..\..\..\..\..\apps\watch\include\ui\" (
	..\..\..\UITools\FileCompare.exe .\ename.h
	if !errorlevel! neq 1 (
		copy .\ename.h     ..\..\..\..\..\..\..\apps\watch\include\ui\style_JL_new.h
	)
	..\..\..\UITools\FileCompare.exe .\result_pic_index.h
	if !errorlevel! neq 1 (
		copy .\result_pic_index.h     ..\..\..\..\..\..\..\apps\watch\include\ui\result_pic_index.h
	)
	..\..\..\UITools\FileCompare.exe .\result_str_index.h
	if !errorlevel! neq 1 (
		copy .\result_str_index.h     ..\..\..\..\..\..\..\apps\watch\include\ui\result_str_index.h
	)
)

call ..\..\..\UITools\sdk_compile.bat
exit
