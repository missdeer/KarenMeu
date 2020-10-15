package main

import (
	"crypto/tls"
	"encoding/json"
	"flag"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"strings"
)

const (
	themeURL = "https://api.mdnice.com/themes?currentPage=0&pageSize=100&checked=true&order=ASC"
)

var (
	outputDir = "../../rc/theme"
)

type ThemeResponse struct {
	Code int `json:"code"`
	Data struct {
		PageNum   int `json:"pageNum"`
		ThemeList []struct {
			ThemeID     int    `json:"themeId"`
			Name        string `json:"name"`
			Cover       string `json:"cover"`
			CSS         string `json:"css"`
			Article     string `json:"article"`
			AuthorName  string `json:"authorName"`
			Description string `json:"discription"`
		} `json:"themeList"`
	} `json:"data"`
}

func main() {
	flag.StringVar(&outputDir, "output", outputDir, "output directory")
	flag.Parse()

	req, err := http.NewRequest("GET", themeURL, nil)
	if err != nil {
		log.Fatal(err)
	}

	client := http.Client{
		Transport: &http.Transport{
			TLSClientConfig: &tls.Config{
				InsecureSkipVerify: true,
			},
		},
	}
	resp, err := client.Do(req)
	if err != nil {
		log.Fatal(err)
	}
	defer resp.Body.Close()

	d, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		log.Fatal(err)
	}

	var themes ThemeResponse
	err = json.Unmarshal(d, &themes)
	if err != nil {
		log.Fatal(err)
	}

	if _, err := os.Stat(outputDir); os.IsNotExist(err) {
		os.MkdirAll(outputDir, 0755)
	}
	for _, theme := range themes.Data.ThemeList {
		fn := filepath.Join(outputDir, strings.TrimSpace(theme.Name)+".css")
		fd, err := os.OpenFile(fn, os.O_CREATE|os.O_TRUNC|os.O_WRONLY, 0644)
		if err != nil {
			log.Println(err)
			continue
		}
		fd.WriteString(theme.CSS)
		fd.Close()
		log.Println(fn, "is written")
	}
}
