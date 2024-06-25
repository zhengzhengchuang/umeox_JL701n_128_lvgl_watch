@echo off
..\..\..\..\UITools\ResBuilder.exe
copy .\result.bin ..\..\..\..\..\ui_resource\watch4.view
echo 生成 watch4.view 成功！
pause