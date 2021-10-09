package main

import (
	"log"
	"os"
	"strconv"
	"time"
)

func main() {

	level := int(time.Now().UTC().UnixNano())
	if len(os.Args) > 1 {
		value, err := strconv.Atoi(os.Args[1])
		if err != nil {
			log.Fatalf("%+v", err)
		}
		level = value
		if level <= 0 {
			level = 1
		}
	}
	maze := NewGame(level)
	maze.Run()
}
