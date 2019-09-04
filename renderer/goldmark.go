package main

import (
	"C"
	"bytes"
	"log"
	"strings"

	"github.com/alecthomas/chroma/formatters/html"
	"github.com/yuin/goldmark"
	highlighting "github.com/yuin/goldmark-highlighting"
	"github.com/yuin/goldmark-meta"
	"github.com/yuin/goldmark/extension"
	"github.com/yuin/goldmark/parser"
	"github.com/yuin/goldmark/renderer"
	"github.com/yuin/goldmark/util"
)

var (
	htmlTemplate = `<!DOCTYPE html>
<html><head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<style type="text/css">
{{ .PreviewTheme }}
</style>
<body>
<div id="wx-box" class="wx-box">
<section>
{{ .FinalContent }}
</section>
</div>
</body>
</html>`
	markdown goldmark.Markdown
)

//export ConvertToHTML
func ConvertToHTML(md string, theme string, style string, lineNumbers bool) *C.char {
	options := []html.Option{}
	if lineNumbers {
		options = []html.Option{html.WithLineNumbers()}
	}

	markdown := goldmark.New(
		goldmark.WithExtensions(
			highlighting.NewHighlighting(
				highlighting.WithStyle(style),
				highlighting.WithFormatOptions(options...),
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
	err := markdown.Convert([]byte(md), &buf, parser.WithWorkers(8))
	if err != nil {
		log.Println(err)
		r := C.CString(md)
		return r
	}

	s := strings.Replace(htmlTemplate, `{{ .PreviewTheme }}`, theme, -1)
	s = strings.Replace(s, `{{ .FinalContent }}`, buf.String(), -1)

	re := C.CString(s)
	return re
}
