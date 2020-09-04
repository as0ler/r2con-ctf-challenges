package main

import (
	"fmt"
	"strings"
	"crypto/aes"
	"crypto/cipher"
	"crypto/rand"
	"io"
	"encoding/base64"
	"bytes"
	"crypto/sha256"
)

func rotateCharset(c rune, i int) rune {
	if c < 'a' || c > 'z' {
		return c
	}

	if cmin := c + rune(i%26); cmin > 'z' {
		return cmin - 26
	} else {
		return cmin
	}
}

func Pad(src []byte) []byte {
    padding := aes.BlockSize - len(src)%aes.BlockSize
    padtext := bytes.Repeat([]byte{byte(padding)}, padding)
    return append(src, padtext...)
}

func removeBase64Padding(value string) string {
    return strings.Replace(value, "=", "", -1)
}


func main() {
	flag :=  "r2con{_y0U_G0T_d4_Tr34sur3=FooKud0s!}"
	moves := "kkkkkkklllliiiiiiijj"
	//moves := "kkkkkkkllllkkkkllllliiiiiiiiiijjkkkiiillkkkkkkkkkkllliiiiiiiiiiikkkkkkkkkkkkkkkjjjjjjii"
	var solution string
	var count int = 0

	for _, c := range moves {
		r := rotateCharset(c, count)
		solution = solution + string(r)
		count ++
	}

	fmt.Printf("Moves: %s\n", solution)

	hash := sha256.Sum256([]byte(solution))
	var key []byte = hash[:]

	fmt.Printf("[*] Encrypting the flag: %s\n", flag)
	block, err := aes.NewCipher(key)
    if err != nil {
        panic(err)
    }
	msg := Pad([]byte(flag))
	ciphertext := make([]byte, aes.BlockSize+len(msg))
    iv := ciphertext[:aes.BlockSize]
	if _, err := io.ReadFull(rand.Reader, iv); err != nil {
		panic(err)
	}
	cfb := cipher.NewCFBEncrypter(block, iv)
	cfb.XORKeyStream(ciphertext[aes.BlockSize:], []byte(msg))
	result := removeBase64Padding(base64.URLEncoding.EncodeToString(ciphertext))

	fmt.Printf("[*] Encrypted flag: %s\n", result)
}
