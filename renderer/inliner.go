package main

import (
	"C"
	"github.com/vanng822/go-premailer/premailer"
    "github.com/aymerick/douceur/parser"
)

func premailerEngine(input string) (res string, err error) {
	prem, err := premailer.NewPremailerFromString(input, premailer.NewOptions())
	if err != nil {
		return
	}
	
	res, err = prem.Transform()
	return
}

func douceurEngine(input string) (res string, err error) {
    stylesheet, err := parser.Parse(input)
    if err != nil {
        return 
    }
	res = stylesheet.String()
	return 
}

//export PremailerInliner
func PremailerInliner(input string) *C.char {
	res, err := premailerEngine(input)
	if err != nil {
		return C.CString(input)		
	}
	return C.CString(res)
}

//export DouceurInliner
func DouceurInliner(input string) *C.char {
	res, err := douceurEngine(input)
	if err != nil {
		return C.CString(input)		
	}
	return C.CString(res)
}

//export Inliner
func Inliner(input string)  *C.char {
	res, err:= premailerEngine(input)
	if err == nil {
		return C.CString(res)
	}
	res, err = douceurEngine(input)
	if err == nil {
		return C.CString(res)
	}
	return C.CString(input)
}