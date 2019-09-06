package main

import (
	"C"
	"log"

	"github.com/b3log/lute"
)

//export Lute
func Lute(md string, style string, lineNumbers bool) *C.char {
	luteEngine := lute.New(lute.CodeSyntaxHighlight(true, true, lineNumbers, style))
	html, err := luteEngine.MarkdownStr("KarenMeu", md)
	if nil != err {
		log.Println(err)
		r := C.CString(md)
		return r
	}

	re := C.CString(html)
	return re
}
