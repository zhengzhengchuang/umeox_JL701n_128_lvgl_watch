@echo off
..\..\..\..\UITools\ResBuilder.exe
copy .\result.bin ..\..\..\..\..\ui_resource\watch2.view
echo 生成 watch2.view 成功！
pause