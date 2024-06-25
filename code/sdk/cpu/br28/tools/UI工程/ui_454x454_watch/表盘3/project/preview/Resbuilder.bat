@echo off
..\..\..\..\UITools\ResBuilder.exe
copy .\result.bin ..\..\..\..\..\ui_resource\watch3.view
echo 生成 watch3.view 成功！
pause