package main

import "C"

import (
	"image/png"

	"github.com/ultimate-guitar/go-imagequant"
)

//export Crush
func Crush(input []byte, output *[]C.char) bool {
	out, err := imagequant.Crush(input, 3, png.BestCompression)
	if err != nil {
		return false
	}
	output_ := make([]C.char, len(out))
	for i := 0; i < len(out); i++ {
		output_[i] = C.char(out[i])
	}
	*output = output_
	return true
}
