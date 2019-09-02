gcc -v
go version
go env
set GOARCH=amd64
set CGO_ENABLED=1
go build -buildmode=c-archive -o renderer.a goldmark.go
gcc renderer.def renderer.a -shared -lwinmm -lWs2_32 -o renderer.dll -Wl,--out-implib,renderer.dll.a
lib /def:renderer.def /name:renderer.dll /out:renderer.lib /MACHINE:X64
mkdir x64
copy /y *.a x64\
del /q *.a
copy /y *.dll x64\
del /q *.dll
copy /y *.lib x64\
del /q *.lib
copy /y *.h x64\
del /q *.h
copy /y *.exp x64\
del /q *.exp