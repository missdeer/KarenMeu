//go:build !amd64
// +build !amd64

package main

import "C"

//export Crush
func Crush(input []byte, output *[]C.char) bool {
	return false
}
