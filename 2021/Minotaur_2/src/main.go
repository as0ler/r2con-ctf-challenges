package main

import (
	"log"
	"os"
	"strconv"
)

func main() {
	//d := NewMaze(8, 8)
	//d.generator()
	//fmt.Print(d)
	//os.Exit(0)

	level := 1
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
