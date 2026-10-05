@goto Begin 
This is an example dos batch file of how to set environment variable RVCTDIR 
to build applications with app.mak and tools.mak
This batch file alters the PATH variable, restores it when done.
:Begin
@set OLDPATH=%PATH%
@rem set VRXSDKS to the Verix V SDK directory 
@set VRXSDK=C:\vvsdk386
@rem Set RVCTDIR to RVDS2.2
@set RVCTDIR=C:\Program Files\ARM\RVCT\Programs\4.0\400\win_32-pentium
@set MAKEDIR=C:\Program Files\ARM\bin\win_32-pentium
@rem or, Set RVCTDIR to RVDS2.1
@rem set RVCTDIR=C:\Program Files\ARM\RVCT\Programs\2.0.1\277\win_32-pentium
@set PATH=%VRXSDK%\bin\;%RVCTDIR%;%MAKEDIR%
@rem use app.mak to buid application
make Makefile.vx
@rem or, use vrxcc directly here to build a simple application
@rem %VRXSDK%\bin\vrxcc app.c
@set PATH=%OLDPATH%
@set RVCTDIR=
