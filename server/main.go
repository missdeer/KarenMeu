// basic auth
// add/rename/remove directory
// add/rename/modify/remove file
package main

import (
	"fmt"
	"log"

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

	// Group using gin.BasicAuth() middleware
	// gin.Accounts is a shortcut for map[string]string
	authorized := r.Group("/", gin.BasicAuth(gin.Accounts{
		"foo":    "bar",
		"austin": "1234",
		"lena":   "hello2",
		"manu":   "4321",
	}))

	authorized.GET("/folders", controllers.ListFolders)
	authorized.GET("/folder/:id", controllers.GetFolderContent)
	authorized.POST("/folder/name/:id", controllers.RenameFolder)
	authorized.DELETE("/folder/:id", controllers.RemoveFolder)
	authorized.PUT("/folder", controllers.AddFolder)

	authorized.GET("/file/:id", controllers.GetFileContent)
	authorized.POST("/file/content/:id", controllers.ModifyFileContent)
	authorized.POST("/file/name/:id", controllers.RenameFile)
	authorized.DELETE("/file/:id", controllers.RemoveFile)
	authorized.PUT("/file", controllers.AddFile)
	authorized.PATCH("/file/:id", controllers.MoveFile)

	r.Run(":4567")
}
