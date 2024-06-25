@echo off
if not exist project\animation md project\animation
cd project\animation
if not exist ResBuilder.xml copy ..\..\..\..\UITools\ResBuilder.xml
start "" ..\..\..\..\UITools\动画生成工具.exe