gcc -v
go version
go env
set GOARCH=386
set CGO_ENABLED=1
go build -buildmode=c-archive -o utils.a 
gcc utils.def utils.a -shared -lwinmm -lWs2_32 -o utils.dll -Wl,--out-implib,utils.dll.a
lib /def:utils.def /name:utils.dll /out:utils.lib /MACHINE:X86
mkdir x86
copy /y *.a x86\
del /q *.a
copy /y *.dll x86\
del /q *.dll
copy /y *.lib x86\
del /q *.lib
copy /y *.h x86\
del /q *.h
copy /y *.exp x86\
del /q *.exp
