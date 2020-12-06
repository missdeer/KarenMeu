package models

import (
	"github.com/jinzhu/gorm"
)

var (
	db *gorm.DB
)

func SetDatabase(d *gorm.DB) {
	db = d
}

func SetupDatabase() {
	db.AutoMigrate(&User{})
	db.AutoMigrate(&File{})
	db.AutoMigrate(&Folder{})
}
