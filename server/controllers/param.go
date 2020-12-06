package controllers

import (
	"github.com/jinzhu/gorm"
)

var (
	db *gorm.DB
)

func SetDatabase(d *gorm.DB) {
	db = d
}
