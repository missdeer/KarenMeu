# Makefile for building utils module
GITCOMMIT:=$(shell git describe --dirty --always)
BINARY:=libutils.a
SYSTEM:=
CHECKS:=lute.go main.go inliner.go goldmark.go pngquant.go ../go.mod
BUILDOPTS:=-v -buildmode=c-archive 
UNAME_S:=$(shell uname -s)
UNAME_M:=$(shell uname -m)
ifeq ($(UNAME_M),arm64)
	GOARCH:=arm64
endif
ifneq ($(UNAME_S),Darwin)
	UNAME_O:=$(shell uname -o)
	ifeq ($(UNAME_O),Msys)
		HANNAH:=hannah.exe
		RP=rp.exe
	endif
endif

.PHONY: all
all: $(BINARY)

$(BINARY): $(CHECKS)
	env CGO_ENABLED=1 $(SYSTEM) go build $(BUILDOPTS) -ldflags="-s -w" -o $(BINARY)
	if [ "$(UNAME_S)" = "Darwin" ]; then env CGO_ENABLED=1 GOARCH=amd64 go build $(BUILDOPTS) -ldflags="-s -w" -o $(BINARY).alter.a; fi
	mv libutils.h utils.h
	go mod tidy
	bash -c ./update-def.sh

.PHONY: clean
clean:
	GO111MODULE=on go clean
	rm -f $(BINARY) $(BINARY).alter.a *.h
