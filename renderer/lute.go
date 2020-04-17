package main

import (
	"C"

	"github.com/88250/lute"
)

//export Lute
func Lute(md string, style string, lineNumbers bool) *C.char {
	luteEngine := lute.New()
	luteEngine.CodeSyntaxHighlight = true
	luteEngine.CodeSyntaxHighlightInlineStyle = true
	luteEngine.CodeSyntaxHighlightLineNum = lineNumbers
	luteEngine.CodeSyntaxHighlightStyleName = style
	html := luteEngine.MarkdownStr("KarenMeu", md)
	if html == "" {
		r := C.CString(md)
		return r
	}

	re := C.CString(html)
	return re
}
