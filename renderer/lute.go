package main

import (
	"C"
	"log"

	"github.com/88250/lute"
)

//export Lute
func Lute(md string, style string, lineNumbers bool) *C.char {
	luteEngine := lute.New()
	luteEngine.CodeSyntaxHighlight = true
	luteEngine.CodeSyntaxHighlightInlineStyle = true
	luteEngine.CodeSyntaxHighlightLineNum = lineNumbers
	luteEngine.CodeSyntaxHighlightStyleName = style
	html, err := luteEngine.MarkdownStr("KarenMeu", md)
	if nil != err {
		log.Println(err)
		r := C.CString(md)
		return r
	}

	re := C.CString(html)
	return re
}
