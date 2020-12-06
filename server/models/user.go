package models

import (
	"github.com/jinzhu/gorm"
)

type User struct {
	gorm.Model
	Name     string   `gorm:"unique;not null"`
	Password string   `gorm:"not null"`
	DBConn   *gorm.DB `gorm:"-"`
}

func (u *User) Add() error {
	return u.DBConn.Create(u).Error
}

func (u *User) Delete() error {
	return u.DBConn.Unscoped().Delete(u).Error
}

func (u *User) Update() error {
	return u.DBConn.Save(u).Error
}

func (u *User) First() error {
	return u.DBConn.First(u).Error
}

func (u *User) Last() error {
	return u.DBConn.Last(u).Error
}

func (u *User) Take() error {
	return u.DBConn.Take(u).Error
}

func (u *User) TakeByName() error {
	return u.DBConn.Where("name = ?", u.Name).First(u).Error
}

func (u *User) TakeRecord() error {
	return u.DBConn.Where("name = ? AND password = ?", u.Name, u.Password).First(u).Error
}

func (u *User) TakeByHash() error {
	return u.DBConn.Where("password = ?", u.Password).First(u).Error
}

func (u *User) CheckExistsByName() bool {
	user := &User{}
	err := u.DBConn.Where("name = ?", u.Name).First(user).Error
	return !gorm.IsRecordNotFoundError(err)
}

func (u *User) Exists() bool {
	user := &User{}
	err := u.DBConn.Where("name = ? AND password = ?", u.Name, u.Password).First(user).Error
	return !gorm.IsRecordNotFoundError(err)
}

func (u *User) Users() (users []User) {
	u.DBConn.Find(&users)
	return
}
