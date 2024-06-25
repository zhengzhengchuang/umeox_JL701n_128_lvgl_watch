@echo off
..\..\..\..\UITools\ResBuilder.exe
copy .\result.bin ..\..\..\..\..\ui_resource\watch5.view
echo 生成 watch5.view 成功！
pause