package main

import (
	"C"
	"log"

	"github.com/missdeer/lute"
)

//export Lute
func Lute(md string) *C.char {
	luteEngine := lute.New()
	html, err := luteEngine.MarkdownStr("KarenMeu", md)
	if nil != err {
		log.Println(err)
		r := C.CString(md)
		return r
	}

	re := C.CString(html)
	return re
}
