package main

import (
	"C"
	"bytes"
	"log"

	"github.com/alecthomas/chroma/formatters/html"
	mathjax "github.com/litao91/goldmark-mathjax"
	"github.com/yuin/goldmark"
	emoji "github.com/yuin/goldmark-emoji"
	highlighting "github.com/yuin/goldmark-highlighting"
	meta "github.com/yuin/goldmark-meta"
	"github.com/yuin/goldmark/extension"
	"github.com/yuin/goldmark/renderer"
	"github.com/yuin/goldmark/util"
)

//export Goldmark
func Goldmark(md string, style string, lineNumbers bool) *C.char {
	options := []html.Option{html.WithLineNumbers(lineNumbers)}
	markdown := goldmark.New(
		goldmark.WithExtensions(mathjax.MathJax),
		goldmark.WithExtensions(
			highlighting.NewHighlighting(
				highlighting.WithStyle(style),
				highlighting.WithFormatOptions(options...),
			),
			meta.New(meta.WithTable()),
		),
		goldmark.WithRendererOptions(
			renderer.WithNodeRenderers(
				util.Prioritized(extension.NewTableHTMLRenderer(), 500),
			),
		),
		goldmark.WithExtensions(
			emoji.Emoji,
		),
	)

	var buf bytes.Buffer
	err := markdown.Convert([]byte(md), &buf)
	if err != nil {
		log.Println(err)
		r := C.CString(md)
		return r
	}

	re := C.CString(buf.String())
	return re
}
