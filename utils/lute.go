package main

import (
	"C"

	"github.com/88250/lute"
)

//export Lute
func Lute(md string, style string, lineNumbers bool) *C.char {
	luteEngine := lute.New()
	luteEngine.ParseOptions.Footnotes = true
	luteEngine.ParseOptions.GFMAutoLink = true
	luteEngine.ParseOptions.GFMStrikethrough = true
	luteEngine.ParseOptions.GFMTable = true
	luteEngine.ParseOptions.GFMTaskListItem = true
	luteEngine.RenderOptions.CodeSyntaxHighlight = true
	luteEngine.RenderOptions.CodeSyntaxHighlightInlineStyle = true
	luteEngine.RenderOptions.CodeSyntaxHighlightLineNum = lineNumbers
	luteEngine.RenderOptions.CodeSyntaxHighlightStyleName = style
	html := luteEngine.MarkdownStr("KarenMeu", md)
	if html == "" {
		r := C.CString(md)
		return r
	}

	re := C.CString(html)
	return re
}
