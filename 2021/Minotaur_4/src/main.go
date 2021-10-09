package main

import (
	"crypto/rand"
	"encoding/binary"
	"fmt"
	"io"
	"log"
	"os"
	"strconv"
)

func main() {

	buf := make([]byte, 8)
	_, err := io.ReadFull(rand.Reader, buf)
	if err != nil {
		panic(fmt.Sprintf("crypto/rand is unavailable: Read() failed with %#v", err))
	}

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

	maze := NewGame(level, int64(binary.LittleEndian.Uint64(buf)))
	maze.Run()
}
