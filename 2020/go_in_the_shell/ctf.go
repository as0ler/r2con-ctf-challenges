package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"os/exec"
	"strings"
	"time"
)

func runCommand(binary string, arguments []string) []byte {
	out, err := exec.Command(binary, arguments...).Output()
	if err != nil {
		log.Fatal(err)
	}
	return out
}

func main() {

	fmt.Println(`
   ___        _         _   _            __ _          _ _ 
  / _ \___   (_)_ __   | |_| |__   ___  / _\ |__   ___| | |
 / /_\/ _ \  | | '_ \  | __| '_ \ / _ \ \ \| '_ \ / _ \ | |
/ /_\\ (_) | | | | | | | |_| | | |  __/ _\ \ | | |  __/ | |
\____/\___/  |_|_| |_|  \__|_| |_|\___| \__/_| |_|\___|_|_|
															`)
	fmt.Println("---------------------")
	prompt := "$ "
	specialCommand := "cat flag.txt"
	reader := bufio.NewReader(os.Stdin)
forLoop:
	for {
		fmt.Print(prompt)
		text, _ := reader.ReadString('\n')
		switch text = strings.TrimSpace(text); text {
		case "":
		case "help":
			fmt.Println("Available commands: ls, pwd, cat flag.txt, exit")
		case "ls":
			fmt.Printf("%s\n", runCommand("ls", []string{}))
		case "pwd":
			fmt.Printf("%s\n", runCommand("pwd", []string{}))
		case specialCommand:
			if prompt == "$ " {
				fmt.Print("The flag is r2con{")
				for i := 0; i < 3; i++ {
					fmt.Print(".")
					time.Sleep(1 * time.Second)
				}
				fmt.Println("\nlol it isn't going to be that easy")
			} else {
				parts := strings.Split(specialCommand, " ")
				fmt.Printf("%s\n", runCommand(parts[0], parts[1:]))
			}
		case "gh0stInTheG0":
			fmt.Print("what are you doing?")
			for i := 0; i < 3; i++ {
				fmt.Print(".")
				time.Sleep(1 * time.Second)
			}
			fmt.Println("NO STOP PLEA--")
			prompt = "# "
		case "exit":
			break forLoop
		default:
			fmt.Println("Unrecognized command:", text)
		}
	}
	os.Exit(0)
}

