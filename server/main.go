// basic auth
// add/rename/remove directory
// add/rename/modify/remove file
package main

import (
	"fmt"
	"log"
	"os"
	"time"

	jwt "github.com/appleboy/gin-jwt/v2"
	"github.com/gin-gonic/gin"
	"github.com/jinzhu/gorm"

	_ "github.com/jinzhu/gorm/dialects/mssql"
	_ "github.com/jinzhu/gorm/dialects/mysql"
	_ "github.com/jinzhu/gorm/dialects/postgres"
	_ "github.com/jinzhu/gorm/dialects/sqlite"

	"github.com/missdeer/KarenMeu/server/controllers"
	"github.com/missdeer/KarenMeu/server/models"
)

var (
	db *gorm.DB
)

func main() {
	fmt.Println("KarenMeu cloud")
	var err error
	db, err = gorm.Open("sqlite3", "KarenMeuCloud.db")
	if err != nil {
		log.Fatal("failed to connect database")
	}
	defer db.Close()

	controllers.SetDatabase(db)
	models.SetDatabase(db)
	models.SetupDatabase()

	r := gin.Default()

	// the jwt middleware
	authMiddleware, err := jwt.New(&jwt.GinJWTMiddleware{
		Realm:           "KarenMenu Cloud Service",
		Key:             []byte(os.Getenv("AUTH_KEY")),
		Timeout:         time.Hour,
		MaxRefresh:      time.Hour,
		IdentityKey:     controllers.AuthIdentityKey,
		PayloadFunc:     controllers.PayloadFunc,
		IdentityHandler: controllers.IdentityHandler,
		Authenticator:   controllers.Authenticator,
		Authorizator:    controllers.Authorizator,
		Unauthorized:    controllers.Unauthorized,
		// TokenLookup is a string in the form of "<source>:<name>" that is used
		// to extract token from the request.
		// Optional. Default value "header:Authorization".
		// Possible values:
		// - "header:<name>"
		// - "query:<name>"
		// - "cookie:<name>"
		// - "param:<name>"
		TokenLookup: "header: Authorization, query: token, cookie: jwt",
		// TokenLookup: "query:token",
		// TokenLookup: "cookie:token",

		// TokenHeadName is a string in the header. Default value is "Bearer"
		TokenHeadName: "Bearer",

		// TimeFunc provides the current time. You can override it to use another time value. This is useful for testing or if your server uses a different time zone than your tokens.
		TimeFunc: time.Now,
	})

	if err != nil {
		log.Fatal("JWT Error:" + err.Error())
	}

	// When you use jwt.New(), the function is already automatically called for checking,
	// which means you don't need to call it again.
	errInit := authMiddleware.MiddlewareInit()

	if errInit != nil {
		log.Fatal("authMiddleware.MiddlewareInit() Error:" + errInit.Error())
	}

	r.POST("/login", authMiddleware.LoginHandler)

	r.NoRoute(authMiddleware.MiddlewareFunc(), func(c *gin.Context) {
		claims := jwt.ExtractClaims(c)
		log.Printf("NoRoute claims: %#v\n", claims)
		c.JSON(404, gin.H{"code": "PAGE_NOT_FOUND", "message": "Page not found"})
	})

	auth := r.Group("/auth")
	// Refresh time can be longer than token timeout
	auth.GET("/refresh_token", authMiddleware.RefreshHandler)
	auth.Use(authMiddleware.MiddlewareFunc())
	{
		auth.GET("/folders", controllers.GetFolderList)
		auth.GET("/folder/:id", controllers.GetFileListInFolder)
		auth.POST("/folder/name/:id", controllers.RenameFolder)
		auth.DELETE("/folder/:id", controllers.RemoveFolder)
		auth.PUT("/folder", controllers.AddFolder)

		auth.GET("/file/:id", controllers.GetFileContent)
		auth.POST("/file/content/:id", controllers.ModifyFileContent)
		auth.POST("/file/name/:id", controllers.RenameFile)
		auth.DELETE("/file/:id", controllers.RemoveFile)
		auth.PUT("/file", controllers.AddFile)
		auth.PATCH("/file/:id", controllers.MoveFile)
	}

	bindAddr := os.Getenv("BINDADDR")
	if bindAddr == "" {
		bindAddr = ":4567"
	}
	log.Fatal(r.Run(bindAddr))
}
