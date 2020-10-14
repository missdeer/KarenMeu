# Makefile for building renderer module
GITCOMMIT:=$(shell git describe --dirty --always)
BINARY:=librenderer.a
SYSTEM:=
CHECKS:=lute.go main.go inliner.go goldmark.go go.mod
BUILDOPTS:=-v -buildmode=c-archive 
CGO_ENABLED:=1

.PHONY: all
all: $(BINARY)

$(BINARY): $(CHECKS)
	GO111MODULE=on CGO_ENABLED=$(CGO_ENABLED) $(SYSTEM) go build $(BUILDOPTS) -ldflags="-s -w" -o $(BINARY)
	mv librenderer.h renderer.h
	go mod tidy

.PHONY: clean
clean:
	GO111MODULE=on go clean
	rm -f $(BINARY) *.h
