@echo off
echo fontsize:12 16x16(ºº×Ö) 8x16(·ûºÅ)
echo fontsize:24 32x32(ºº×Ö) 16x32(·ûºÅ)
echo fontsize:48 64x64(ºº×Ö) 32x64(·ûºÅ)
set fontsize=24
ota_text_matrix.exe -intxt text.txt -fontsize %fontsize% -outfile text_matrix.h -format IMC_OSD1
echo ×ÖÄ£Éú³ÉÍê±Ï.
pause