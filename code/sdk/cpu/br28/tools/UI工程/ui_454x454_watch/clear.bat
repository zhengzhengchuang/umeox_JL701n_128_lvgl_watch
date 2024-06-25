@echo off

del *.txt /s
del *.h /s
del *.csv /s
del *.bin /s
del *.str /s
del *.xls /s
del *.bak /s
del result.* /s
del *.tab /s
del *.deal /s
del *.dat /s
del *.xml /s
del gmon.out /s

for /d %%i in (*) do del %%i\project\config\*.png
