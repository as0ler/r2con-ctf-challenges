package main

import (
	"crypto/aes"
	"crypto/cipher"
	"crypto/rand"
	"io"
	"os"
	"text/template"
)

func main() {
	block, err := aes.NewCipher([]byte(key))
	if err != nil {
		panic(err)
	}
	ciphertext := make([]byte, aes.BlockSize+len(dungeons))
	iv := ciphertext[:aes.BlockSize]
	if _, err := io.ReadFull(rand.Reader, iv); err != nil {
		panic(err)
	}
	stream := cipher.NewCFBEncrypter(block, iv)
	stream.XORKeyStream(ciphertext[aes.BlockSize:], dungeons)
	dungeonsTemplate.Execute(os.Stdout, ciphertext)
}

const key = "\xca\xfe\xba\xbe\xca\xfe\xba\xbe\xca\xfe\xba\xbe\xca\xfe\xba\x25\xbe" +
	"\xca\xfe\xba\xbe\xca\xfe\xba\xbe\xca\xfe\xba\xbe\xca\xfe\xba"

var dungeonsTemplate = template.Must(template.New("").Parse(dungeonsCode))

const dungeonsCode = `package main

import (
	"crypto/aes"
	"crypto/cipher"
)

func generateDungeon() (m *dungeon) {
	m = new(dungeon)
	block, err := aes.NewCipher([]byte(key))
	if err != nil {
		panic(err)
	}
	iv := dungeons[:aes.BlockSize]
	ciphertext := dungeons[aes.BlockSize:]
	stream := cipher.NewCFBDecrypter(block, iv)
	stream.XORKeyStream(ciphertext, ciphertext)

	for i, ch := range ciphertext {
		l := i / (levelSizeX*levelSizeY)
		x := (i % (levelSizeX*levelSizeY)) / levelSizeY
		y := (i % (levelSizeX*levelSizeY)) % levelSizeY
		m[l][x][y] = dungeonEntity(ch)
	}
	return m
}

var dungeons = []byte{ {{range .}}{{.}},{{end}} }

const key = "\xca\xfe\xba\xbe\xca\xfe\xba\xbe\xca\xfe\xba\xbe\xca\xfe\xba\x25\xbe" +
	"\xca\xfe\xba\xbe\xca\xfe\xba\xbe\xca\xfe\xba\xbe\xca\xfe\xba"

`

var dungeons = []byte{
		// level 0
		' ', '#', 'M', ' ', ' ', '#', '#', '#', '#', '#', '#', '#', 'u', '#', '#', '#',
		' ', '#', '#', '#', ' ', '#', '#', ' ', ' ', ' ', 'm', '#', ' ', '#', '#', '#',
		' ', '#', '#', '#', ' ', '#', '#', ' ', '#', ' ', '#', '#', ' ', '#', '#', '#',
		' ', '#', '#', '#', ' ', '#', '#', ' ', '#', ' ', '#', '#', ' ', 'm', 'm', '#',
		' ', '#', '#', '#', ' ', '#', '#', 's', '#', ' ', '#', '#', ' ', '#', ' ', '#',
		' ', '#', 'm', ' ', ' ', '#', '#', '#', '#', ' ', '#', '#', ' ', '#', ' ', '#',
		' ', '#', '#', '#', ' ', ' ', 'm', '#', '#', ' ', '#', '#', ' ', '#', ' ', '#',
		' ', ' ', ' ', ' ', ' ', '#', '#', '#', '#', ' ', '#', '#', ' ', '#', ' ', '#',
		' ', '#', '#', '#', ' ', '#', '#', '#', '#', ' ', '#', '#', ' ', '#', ' ', '#',
		' ', '#', '#', '#', ' ', '#', '#', '#', '#', ' ', '#', '#', ' ', '#', ' ', '#',
		' ', '#', '#', '#', ' ', '#', '#', '#', '#', ' ', '#', '#', ' ', '#', ' ', '#',
		' ', '#', 'm', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'm', ' ', ' ', ' ', 'm',
		' ', '#', '#', '#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
		' ', '#', '#', '#', ' ', '#', 'f', '#', '#', '#', '#', '#', '#', '#', '#', '#',
		'm', '#', '#', '#', ' ', '#', ' ', '#', '#', '#', '#', '#', '#', '#', '#', '#',
		'#', '#', '#', '#', 'm', '#', 'm', 'm', 'm', ' ', '#', '#', '#', '#', '#', '#',
		// level 1
		'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#', '#', '#',
		'm', '#', '#', 'm', '#', '#', '#', '#', '#', '#', '#', '#', ' ', '#', '#', '#',
		' ', '#', '#', ' ', '#', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#', '#', '#',
		' ', '#', '#', ' ', '#', '#', ' ', '#', '#', '#', '#', '#', ' ', '#', '#', '#',
		' ', '#', '#', ' ', '#', '#', ' ', '#', ' ', 'm', ' ', '#', ' ', '#', '#', '#',
		' ', '#', '#', ' ', '#', '#', ' ', '#', ' ', ' ', 'm', '#', ' ', '#', '#', '#',
		' ', '#', '#', ' ', ' ', ' ', ' ', '#', 'm', ' ', ' ', '#', ' ', '#', '#', '#',
		' ', '#', '#', ' ', '#', '#', '#', '#', ' ', ' ', ' ', '#', ' ', '#', '#', '#',
		' ', ' ', '#', ' ', '#', '#', '#', '#', ' ', ' ', 'm', '#', ' ', '#', '#', '#',
		'#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', '#', '#', '#',
		'#', ' ', '#', '#', '#', '#', ' ', '#', ' ', ' ', ' ', '#', ' ', '#', '#', '#',
		'#', ' ', '#', '#', 'm', '#', ' ', '#', ' ', 'm', ' ', '#', ' ', '#', '#', '#',
		'#', ' ', '#', '#', ' ', '#', ' ', '#', ' ', ' ', 'm', '#', ' ', '#', '#', '#',
		'#', ' ', '#', '#', ' ', '#', ' ', '#', ' ', ' ', ' ', '#', ' ', '#', '#', '#',
		'#', ' ', '#', '#', ' ', '#', '#', '#', '#', ' ', '#', '#', ' ', '#', '#', '#',
		'#', 'm', ' ', ' ', ' ', ' ', ' ', 'm', '#', 'd', ' ', ' ', ' ', '#', '#', '#',
}
