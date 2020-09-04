package main

import (
    "fmt"
    "strings"
	"bufio"
	"os"
)

func x(r rune) rune {
    if r >= 'a' && r <= 'z' {
        // Rotate lowercase letters 13 places.
        if r > 'm' {
            return r - 13
        } else {
            return r + 13
        }
    } else if r >= 'A' && r <= 'Z' {
        // Rotate uppercase letters 13 places.
        if r > 'M' {
            return r - 13
        } else {
            return r + 13
        }
    }
    // Do nothing.
    return r
}

func main() {
	flag := "e2pba{.V_4z_e00bb000g=4aq_V_4z_P00Y!}"

	fmt.Println("- Say the magic word: ")
	reader := bufio.NewReader(os.Stdin)
	text, _ := reader.ReadString('\n')
	text = strings.TrimRight(text, "\n")
    mapped := strings.Map(x, text)
	if strings.Compare(flag, mapped) == 0 {
		fmt.Println("[+] This is correct!")
	} else {
		fmt.Println("[+] This is wrong!")
	}
}
