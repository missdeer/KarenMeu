package main

import (
	"C"
	"bytes"
	"log"

	"github.com/alecthomas/chroma/formatters/html"
	"github.com/yuin/goldmark"
	highlighting "github.com/yuin/goldmark-highlighting"
	"github.com/yuin/goldmark/parser"
)

//export ConvertToHTML
func ConvertToHTML(md string, codeBlockStyle string) *C.char {
	markdown := goldmark.New(
		goldmark.WithExtensions(
			highlighting.NewHighlighting(
				highlighting.WithStyle(codeBlockStyle),
				highlighting.WithFormatOptions(
					html.WithLineNumbers(),
				),
			),
		),
		goldmark.WithParserOptions(
			parser.WithAutoHeadingID(),
		),
	)

	var buf bytes.Buffer
	if err := markdown.Convert([]byte(md), &buf, parser.WithWorkers(8)); err != nil {
		log.Println(err)
	}
	return C.CString(buf.String())
}
