package main

/*
#include <stdlib.h>
*/
import "C"
import "unsafe"

func main() {}

//export Free
func Free(c *C.char) {
	C.free(unsafe.Pointer(c))
}
