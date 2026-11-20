%~d0
cd %~p0
rmdir /Q /S ..\..\doc\html
mkdir ..\..\doc\html
attrib -r ..\..\doc\html /d
mkdir ..\..\doc\html\src
attrib -r ..\..\doc\html\src /d
xcopy ..\..\src ..\..\doc\html\src /s /e /i /y
mkdir ..\..\doc\html\coverage
attrib -r ..\..\doc\html\coverage /d
xcopy ..\..\doc\coverage ..\..\doc\html\coverage /s /e /i /y
cd doxygen
echo off
echo.
echo.
echo ########## DOXYGEN MESSAGES ##########
echo on
.\doxygen.exe doxygen_config.Doxyfile

echo off
echo.
echo ########## END DOXYGEN MESSAGES ##########
echo.
echo Check the above Doxygen messages!
echo If there's any error, NO new HTML documentation will be generated.
echo In that case, fix the error, then repeat.
echo.
pause
