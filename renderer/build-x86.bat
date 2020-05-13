gcc -v
go version
go env
set GOARCH=386
set CGO_ENABLED=1
go build -buildmode=c-archive -o renderer.a 
gcc renderer.def renderer.a -shared -lwinmm -lWs2_32 -o renderer.dll -Wl,--out-implib,renderer.dll.a
lib /def:renderer.def /name:renderer.dll /out:renderer.lib /MACHINE:X86
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