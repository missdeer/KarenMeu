gcc -v
go version
go env
set GOARCH=amd64
set CGO_ENABLED=1
go build -buildmode=c-archive -o utils.a 
gcc utils.def utils.a -shared -lwinmm -lWs2_32 -o utils.dll -Wl,--out-implib,utils.dll.a
lib /def:utils.def /name:utils.dll /out:utils.lib /MACHINE:X64
REM mkdir x64
REM copy /y *.a x64\
REM del /q *.a
REM copy /y *.dll x64\
REM del /q *.dll
REM copy /y *.lib x64\
REM del /q *.lib
REM copy /y *.h x64\
REM del /q *.h
REM copy /y *.exp x64\
REM del /q *.exp
