@ECHO OFF
IF "%3" == "T" GOTO MAIN
IF "%3" == "F" GOTO MAIN
GOTO ERRORIN

:MAIN
IF "%1" == "E" GOTO DATE
IF "%1" == "S" GOTO DATE
IF "%1" == "C" GOTO DATE
IF "%1" == "A" GOTO DATE
GOTO ERRORIN

:DATE
IF "%2" == "" GOTO ERROR1
IF EXIST "%2" GOTO ISSUB
MD "%2"
CD "%2"
GOTO %1

:ISSUB
CD "%2"
IF EXIST "%1" GOTO MADEDIR
GOTO %1

----------------- ALL START ----------------------
:A
IF "%3" == "T" GOTO AT
GOTO AF

:AT
MD "A"
MD "E"
CD "E"
xcopy c:\neurospace\Output\*.exe /d:%2 /s
xcopy c:\neurospace\Output\*.pdb /d:%2 /s
cd..
MD "S"
CD "S"
xcopy c:\neurospace\RESOURCE . /d:%2 /s
del *.scc /s
del *.bmp /s
del *.tga /s
del *.dds /s
cd..
MD "C"
CD "C"
xcopy c:\neurospace\ResClient . /d:%2 /s
del *.scc /s
cd..
GOTO GOOD

:AF
MD "A"
MD "E"
CD "E"
xcopy d:\neurospace\Output\*.exe /d:%2 /s
xcopy d:\neurospace\Output\*.pdb /d:%2 /s
cd..
MD "S"
CD "S"
xcopy d:\neurospace\RESOURCE . /d:%2 /s
del *.scc /s
del *.bmp /s
del *.tga /s
del *.dds /s
cd..
MD "C"
CD "C"
xcopy d:\neurospace\ResClient . /d:%2 /s
del *.scc /s
cd..
GOTO GOOD

------------------- ALL END ----------------------

------------------- EXE START ----------------------
:E
IF "%3" == "T" GOTO ET
GOTO EF

:ET
MD "A"
MD "%1"
CD "%1"
xcopy c:\neurospace\Output\*.exe /d:%2 /s
xcopy c:\neurospace\Output\*.pdb /d:%2 /s
ECHO Copy Complit
cd..
GOTO COMPLIT

:EF
MD "A"
MD "%1"
CD "%1"
xcopy d:\neurospace\Output\*.exe /d:%2 /s
xcopy d:\neurospace\Output\*.pdb /d:%2 /s
ECHO Copy Complit
cd..
GOTO COMPLIT

------------------- EXE END ----------------------


------------------- SERVER START -----------------

:S
IF "%3" == "T" GOTO ST
GOTO SF

:ST 
MD "A"
MD "%1"
CD "%1"
xcopy c:\neurospace\RESOURCE . /d:%2 /s
ECHO Copy Complit
del *.scc /s
del *.bmp /s
del *.tga /s
del *.dds /s
cd..
GOTO COMPLIT

:SF
MD "A"
MD "%1"
CD "%1"
xcopy d:\neurospace\RESOURCE . /d:%2 /s
ECHO Copy Complit
del *.scc /s
del *.bmp /s
del *.tga /s
del *.dds /s
cd..
GOTO COMPLIT

------------------- SERVER END ----------------------

------------------- CLIENT START ----------------------
:C
IF "%3" == "T" GOTO CT
GOTO CF

:CT
MD "A"
MD "%1"
CD "%1"
xcopy c:\neurospace\ResClient . /d:%2 /s
ECHO Copy Complit
del *.scc /s
cd..
GOTO COMPLIT

:CF
MD "A"
MD "%1"
CD "%1"
xcopy d:\neurospace\ResClient . /d:%2 /s
ECHO Copy Complit
del *.scc /s
cd..
GOTO COMPLIT

------------------- CLIENT END ----------------------

:MADEDIR
cd..
IF EXIST "%2_1" GOTO MADEDIR1
MD "%2_1"
CD "%2_1"
GOTO %1

:MADEDIR1
IF EXIST "%2_2" GOTO MADEDIR2
MD "%2_2"
CD "%2_2"
GOTO %1

:MADEDIR2
IF EXIST "%2_3" GOTO MADEDIR3
MD "%2_3"
CD "%2_3"
GOTO %1

:MADEDIR3
IF EXIST "%2_4" GOTO MADEDIR4
MD "%2_4"
CD "%2_4"
GOTO %1

:MADEDIR4
IF EXIST "%2_5" GOTO ERROR1
MD "%2_5"
CD "%2_5"
GOTO %1

:ERROR1
ECHO 날짜를 입력해 주세여
GOTO END

:ERRORIN
ECHO Error! Not Input
ECHO ----------------------------------------------
ECHO 첫번째 인자값 : S: SERVER, C: CLIENT, E : EXE , A : ALL
ECHO 두번째 인자값 : DATE ( 06-11-04 : 2004/06/11 )
ECHO 세번째 인자값 : 정식 : F, 테스트 : T
ECHO ----------------------------------------------
ECHO 예시 patch.bat A 06-11-04 T
ECHO ----------------------------------------------
GOTO END

:GOOD

:COMPLIT

:COMPLIT
cd..
ECHO %1 Complit!
GOTO END

:END
