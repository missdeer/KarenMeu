package controllers

import (
	"log"

	"github.com/gin-gonic/gin"
)

func GetFileContent(c *gin.Context) {
	user := c.MustGet(gin.AuthUserKey).(string)
	log.Println(user)
}

func AddFile(c *gin.Context) {
	user := c.MustGet(gin.AuthUserKey).(string)
	log.Println(user)

}

func RenameFile(c *gin.Context) {
	user := c.MustGet(gin.AuthUserKey).(string)
	log.Println(user)

}

func MoveFile(c *gin.Context) {
	user := c.MustGet(gin.AuthUserKey).(string)
	log.Println(user)

}

func ModifyFileContent(c *gin.Context) {
	user := c.MustGet(gin.AuthUserKey).(string)
	log.Println(user)

}

func RemoveFile(c *gin.Context) {
	user := c.MustGet(gin.AuthUserKey).(string)
	log.Println(user)

}
