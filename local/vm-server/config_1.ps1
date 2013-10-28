cp c:\python26\lib\site-packages\pywin32_system32\* c:\python26\lib\site-packages\win32\
set-executionpolicy unrestricted
reg add hklm\software\microsoft\.netframework /v onlyuselatestclr /t reg_dword /d 1
cp \\10.0.2.4\qemu\server\powershell.exe.config c:\windows\system32\windowspowershell\v1.0\
