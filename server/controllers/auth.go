package controllers

import (
	jwt "github.com/appleboy/gin-jwt/v2"
	"github.com/gin-gonic/gin"
	"github.com/missdeer/KarenMeu/server/models"
)

type login struct {
	Username string `form:"username" json:"username" binding:"required"`
	Password string `form:"password" json:"password" binding:"required"`
}

// AuthIdentityKey auth identity key
var AuthIdentityKey = "name"

// PayloadFunc auth pay load function
func PayloadFunc(data interface{}) jwt.MapClaims {
	if v, ok := data.(*models.User); ok {
		return jwt.MapClaims{
			AuthIdentityKey: v.Name,
		}
	}
	return jwt.MapClaims{}
}

// IdentityHandler identity handler
func IdentityHandler(c *gin.Context) interface{} {
	claims := jwt.ExtractClaims(c)
	return &models.User{
		Name: claims[AuthIdentityKey].(string),
	}
}

// Authenticator authenticator
func Authenticator(c *gin.Context) (interface{}, error) {
	var loginVals login
	if err := c.ShouldBind(&loginVals); err != nil {
		return "", jwt.ErrMissingLoginValues
	}

	u := &models.User{
		DBConn:   db,
		Name:     loginVals.Username,
		Password: loginVals.Password,
	}
	if u.Exists() {
		err := u.TakeByName()
		return u, err
	}

	return nil, jwt.ErrFailedAuthentication
}

// Authorizator authorization
func Authorizator(data interface{}, c *gin.Context) bool {
	if v, ok := data.(*models.User); ok && v.Name == "admin" {
		return true
	}

	return false
}

// Unauthorized unauthorized handler
func Unauthorized(c *gin.Context, code int, message string) {
	c.JSON(code, gin.H{
		"code":    code,
		"message": message,
	})
}
