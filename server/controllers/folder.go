package controllers

import (
	"log"

	"github.com/gin-gonic/gin"
)

func GetFolderList(c *gin.Context) {
	user := c.MustGet(gin.AuthUserKey).(string)
	log.Println(user)

}

func GetFileListInFolder(c *gin.Context) {
	user := c.MustGet(gin.AuthUserKey).(string)
	log.Println(user)

}

func AddFolder(c *gin.Context) {
	user := c.MustGet(gin.AuthUserKey).(string)
	log.Println(user)

}

func RenameFolder(c *gin.Context) {
	user := c.MustGet(gin.AuthUserKey).(string)
	log.Println(user)

}

func RemoveFolder(c *gin.Context) {
	user := c.MustGet(gin.AuthUserKey).(string)
	log.Println(user)

}
