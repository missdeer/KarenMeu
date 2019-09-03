#!/bin/bash
go env
go version
go build -buildmode=c-archive -o librenderer.a 
mv librenderer.h renderer.h
