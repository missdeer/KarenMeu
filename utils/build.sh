#!/bin/bash
go env
go version
go build -buildmode=c-archive -o libutils.a 
mv libutils.h utils.h
