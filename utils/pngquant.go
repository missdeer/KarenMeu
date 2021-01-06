package main

import "C"

import (
	"image/png"

	"github.com/ultimate-guitar/go-imagequant"
)

//export Crush
func Crush(input []byte, output []byte) bool {
	output, err := imagequant.Crush(input, 3, png.BestCompression)
	if err != nil {
		return false
	}
	return true
}
