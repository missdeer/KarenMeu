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

func (u *File) Add() error {
	return u.DBConn.Create(u).Error
}

func (u *File) Delete() error {
	return u.DBConn.Unscoped().Delete(u).Error
}

func (u *File) Update() error {
	return u.DBConn.Save(u).Error
}

func (u *File) First() error {
	return u.DBConn.First(u).Error
}

func (u *File) Last() error {
	return u.DBConn.Last(u).Error
}

func (u *File) Take() error {
	return u.DBConn.Take(u).Error
}
