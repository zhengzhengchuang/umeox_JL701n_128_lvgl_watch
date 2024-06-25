@echo off
..\..\..\..\UITools\ResBuilder.exe
copy .\result.bin ..\..\..\..\..\ui_resource\watch1.view
echo 生成 watch1.view 成功！
pause