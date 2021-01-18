gcc -v
go version
go env
set GOARCH=386
set CGO_ENABLED=1
go build -buildmode=c-archive -o utils.a 
gcc utils.def utils.a -shared -lwinmm -lWs2_32 -o utils.dll -Wl,--out-implib,utils.dll.a
lib /def:utils.def /name:utils.dll /out:utils.lib /MACHINE:X86
REM mkdir x86
REM copy /y *.a x86\
REM del /q *.a
REM copy /y *.dll x86\
REM del /q *.dll
REM copy /y *.lib x86\
REM del /q *.lib
REM copy /y *.h x86\
REM del /q *.h
REM copy /y *.exp x86\
REM del /q *.exp
