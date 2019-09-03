package main

/*
#include <stdlib.h>
*/
import "C"

import (
	"bytes"
	"log"
	"strings"
	"unsafe"

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
	<script>
	function copyToClip(str) {
	  function listener(e) {
		e.clipboardData.setData("text/html", str);
		e.clipboardData.setData("text/plain", str);
		e.preventDefault();
	  }
	  document.addEventListener("copy", listener);
	  document.execCommand("copy");
	  document.removeEventListener("copy", listener);
	};
	</script>            
	<style type="text/css">
	body { background-color: transparent !important; }
	.wx-box {
	  overflow-y: auto;
	  margin: 0 auto;
	  padding: 20px;
	  width: 480px;
	  height: 100%;
	  box-shadow: 0 0 60px rgba(0, 0, 0, 0.1);
	}            
	::-webkit-scrollbar {
	  width: 6px;
	  height: 6px;
	}
	::-webkit-scrollbar-track {
	  border-radius: 3px;
	  background: rgba(0, 0, 0, 0.06);
	  box-shadow: inset 0 0 5px rgba(0, 0, 0, 0.08);
	}
	::-webkit-scrollbar-thumb {
	  border-radius: 3px;
	  background: rgba(0, 0, 0, 0.12);
	  box-shadow: inset 0 0 10px rgba(0, 0, 0, 0.2);
	}       
	pre, pre span{
	  overflow-x: scroll;
	}
	</style>
	<style type="text/css">{{ .PreviewTheme }}</style><body>
	<!--
	<button onclick='copyToClip(document.getElementById("wx-box").innerHTML)'>Copy</button>
	-->
	<div id="wx-box" class="wx-box"><section>{{ .FinalContent }}</section></div></body></html>
	`
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
		defer C.free(unsafe.Pointer(r))
		return r
	}

	s := strings.Replace(htmlTemplate, `{{ .PreviewTheme }}`, theme, -1)
	s = strings.Replace(s, `{{ .FinalContent }}`, buf.String(), -1)
	r, err := premailerEngine(s)
	if err == nil {
		re := C.CString(r)
		defer C.free(unsafe.Pointer(re))
		return re
	}
	r, err = douceurEngine(s)
	if err == nil {
		re := C.CString(r)
		defer C.free(unsafe.Pointer(re))
		return re
	}

	re := C.CString(md)
	defer C.free(unsafe.Pointer(re))
	return re
}
