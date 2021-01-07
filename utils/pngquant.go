package main

import "C"

import (
	"image/png"

	"github.com/ultimate-guitar/go-imagequant"
)

//export Crush
func Crush(input []byte) []byte {
	output, err := imagequant.Crush(input, 3, png.BestCompression)
	if err != nil {
		return nil
	}
	return output
}
