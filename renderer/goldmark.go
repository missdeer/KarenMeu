package main

import (
	"C"
	"bytes"
	"log"

	"github.com/alecthomas/chroma/formatters/html"
	"github.com/yuin/goldmark"
	highlighting "github.com/yuin/goldmark-highlighting"
	"github.com/yuin/goldmark-meta"
	"github.com/yuin/goldmark/extension"
	"github.com/yuin/goldmark/parser"
	"github.com/yuin/goldmark/renderer"
	"github.com/yuin/goldmark/util"
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
			meta.New(meta.WithTable()),
		),
		goldmark.WithParserOptions(
			parser.WithAutoHeadingID(),
		),
		goldmark.WithRendererOptions(
			renderer.WithNodeRenderers(
				util.Prioritized(extension.NewTableHTMLRenderer(), 500),
			),
		),
	)

	var buf bytes.Buffer
	if err := markdown.Convert([]byte(md), &buf, parser.WithWorkers(8)); err != nil {
		log.Println(err)
	}
	return C.CString(buf.String())
}
