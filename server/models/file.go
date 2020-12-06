package models

import "github.com/jinzhu/gorm"

type File struct {
	gorm.Model
	ID     uint64 `gorm:"type:bigint(20) unsigned auto_increment;PRIMARY_KEY"`
	Name   string
	Folder uint
	UserID uint
	DBConn *gorm.DB `gorm:"-"`
}
