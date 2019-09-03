package main

import (
	"C"
	"bytes"
	"html/template"
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

var (
	codeBlockStyle = "xcode"
	lineNumbers    = true
	previewTheme   string
	htmlTemplate   = `<!DOCTYPE html>
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
	t        *template.Template

	codeBlockStyles = map[string]highlighting.Option{
		"xcode":             highlighting.WithStyle("xcode"),
		"vs":                highlighting.WithStyle("vs"),
		"vim":               highlighting.WithStyle("vim"),
		"emacs":             highlighting.WithStyle("emacs"),
		"abap":              highlighting.WithStyle("abap"),
		"algol":             highlighting.WithStyle("algol"),
		"algol_nu":          highlighting.WithStyle("algol_nu"),
		"api":               highlighting.WithStyle("api"),
		"arduino":           highlighting.WithStyle("arduino"),
		"autumn":            highlighting.WithStyle("autumn"),
		"borland":           highlighting.WithStyle("borland"),
		"bw":                highlighting.WithStyle("bw"),
		"colorful":          highlighting.WithStyle("colorful"),
		"dracula":           highlighting.WithStyle("dracula"),
		"friendly":          highlighting.WithStyle("friendly"),
		"fruity":            highlighting.WithStyle("fruity"),
		"github":            highlighting.WithStyle("github"),
		"igor":              highlighting.WithStyle("igor"),
		"lovelace":          highlighting.WithStyle("lovelace"),
		"manni":             highlighting.WithStyle("manni"),
		"monokai":           highlighting.WithStyle("monokai"),
		"monokailight":      highlighting.WithStyle("monokailight"),
		"murphy":            highlighting.WithStyle("murphy"),
		"native":            highlighting.WithStyle("native"),
		"paraiso-dark":      highlighting.WithStyle("paraiso-dark"),
		"paraiso-light":     highlighting.WithStyle("paraiso-light"),
		"pastie":            highlighting.WithStyle("pastie"),
		"perldoc":           highlighting.WithStyle("perldoc"),
		"pygments":          highlighting.WithStyle("pygments"),
		"rainbow_dash":      highlighting.WithStyle("rainbow_dash"),
		"rrt":               highlighting.WithStyle("rrt"),
		"solarized-dark":    highlighting.WithStyle("solarized-dark"),
		"solarized-dark256": highlighting.WithStyle("solarized-dark256"),
		"solarized-light":   highlighting.WithStyle("solarized-light"),
		"swapoff":           highlighting.WithStyle("swapoff"),
		"tango":             highlighting.WithStyle("tango"),
		"trac":              highlighting.WithStyle("trac"),
	}
)

func createGoldmarkMarkdown() {
	options := []html.Option{}
	if lineNumbers {
		options = []html.Option{html.WithLineNumbers()}
	}
	log.Println("re-create markdown object", codeBlockStyle, len(codeBlockStyle), lineNumbers)

	markdown = goldmark.New(
		goldmark.WithExtensions(
			highlighting.NewHighlighting(
				codeBlockStyles[codeBlockStyle],
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
}

//export SetPreviewTheme
func SetPreviewTheme(theme string) {
	log.Println("set preview theme", theme)
	previewTheme = theme
}

//export SetCodeBlockStyle
func SetCodeBlockStyle(style string) {
	codeBlockStyle = style
	createGoldmarkMarkdown()
}

//export SetHTMLTempalte
func SetHTMLTempalte(tpl string) {
	htmlTemplate = tpl
	var err error
	t, err = template.New("").Parse(htmlTemplate)
	if err != nil {
		log.Println(err)
	}
}

//export EnableLineNumbers
func EnableLineNumbers(enable bool) {
	lineNumbers = enable
	createGoldmarkMarkdown()
}

//export ConvertToHTML
func ConvertToHTML(md string) *C.char {
	var buf bytes.Buffer
	err := markdown.Convert([]byte(md), &buf, parser.WithWorkers(8))
	if err != nil {
		log.Println(err)
		return C.CString(md)
	}
	log.Println("preview theme: ", previewTheme)
	data := struct {
		PreviewTheme template.HTML
		FinalContent template.HTML
	}{
		PreviewTheme: template.HTML(previewTheme),
		FinalContent: template.HTML(buf.String()),
	}
	var res bytes.Buffer
	err = t.Execute(&res, data)
	if err != nil {
		log.Println(err)
		return C.CString(md)
	}
	return C.CString(res.String())
}
