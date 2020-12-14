package models

import (
	"github.com/jinzhu/gorm"
)

type Folder struct {
	gorm.Model
	ID     uint64 `gorm:"type:bigint(20) unsigned auto_increment;PRIMARY_KEY"`
	Name   string `gorm:"not null"`
	UserID uint
	DBConn *gorm.DB `gorm:"-"`
}

func (u *Folder) Add() error {
	return u.DBConn.Create(u).Error
}

func (u *Folder) Delete() error {
	return u.DBConn.Unscoped().Delete(u).Error
}

func (u *Folder) Update() error {
	return u.DBConn.Save(u).Error
}

func (u *Folder) First() error {
	return u.DBConn.First(u).Error
}

func (u *Folder) Last() error {
	return u.DBConn.Last(u).Error
}

func (u *Folder) Take() error {
	return u.DBConn.Take(u).Error
}

