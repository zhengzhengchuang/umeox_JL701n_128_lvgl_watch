cd ..\..\..\..\..\..\..\..\sdk
if exist ".\.vscode\winmk.bat" (
	call .\.vscode\winmk.bat all
) else (
	echo ".vscode\winmk.bat not exist"
)