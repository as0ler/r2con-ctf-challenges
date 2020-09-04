package main

import (
	"fmt"
	"strings"
	"syscall"

	"golang.org/x/crypto/ssh/terminal"
)

func main() {
	fmt.Printf("Password: ")
	bytePassword, err := terminal.ReadPassword(int(syscall.Stdin))
	fmt.Println("")
	if err != nil {
		fmt.Println("Err... watcha tryna do?")
	}
	password := string(bytePassword)
	defer checkPassword(password)
}

func checkPassword(password string) {
	// s0rry4str1pp1ng1t!:(
	goodPassword := []string{"s", "0", "r", "r", "y", "4", "s", "t", "r", "1", "p", "p", "1", "n", "g", "1", "t", "!", ":", "("}
	if password == strings.Join(goodPassword, "") {
		fmt.Println("yay!")
	} else {
		fmt.Println("nay...")
	}
}
