"C:\Program Files\MRE SDK V3.0.00\tools\DllPackage.exe" "D:\MyGitHub\Sttxt\Sttxt.vcproj"
if %errorlevel% == 0 (
 echo postbuild OK.
  copy Sttxt.vpp ..\..\..\MoDIS_VC9\WIN32FS\DRIVE_E\Sttxt.vpp /y
exit 0
)else (
echo postbuild error
  echo error code: %errorlevel%
  exit 1
)

