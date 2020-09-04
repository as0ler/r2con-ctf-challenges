package main

import (
	"crypto/aes"
	"crypto/cipher"
	"log"
	"encoding/binary"
	"fmt"
	"strings"
	"github.com/jroimartin/gocui"
	"sync"
	"crypto/sha256"
	"encoding/base64"
)

const (
	levelSizeX = 16
	levelSizeY = 16
	nLevels    = 2
	nLifes     = 3
)

type dungeonEntity byte

var (
	wg   sync.WaitGroup
)

const (
	entityPath       dungeonEntity = ' '
	entityWall                  = '#'
	entityLadderUp              = 'u'
	entityLadderDown            = 'd'
	entityMonster               = 'm'
	entityFinish                = 'f'
	entitySword					= 's'
	entityMap					= 'M'
)

var boy string = "wA65XuQeKuwrhOlLWEi7NsLjQaibIE7rt51HhFcd8yBRkwpSWX8t9UrhT9ChwhqqlgxdBEXqV3rcduA3HWp6yg"

type status int

const (
	statusAlive status = iota
	statusLevelUp
	statusLevelDown
	statusDead
	statusWin
	statusDeadByMonster
	statusHurtByMonster
	statusKill
	gotSword
	gotMap
)

type player struct {
	x, y  int
	level int
	lifes int
	sword bool
	mapp   bool
}


func newPlayer() *player {
	return &player{x: 0, y: 0, level: 0, lifes: nLifes, sword: false, mapp: false}
}

func (p *player) forward() {
	p.x += 1
	if p.x > levelSizeX-1 {
		p.x = levelSizeX - 1
	}
}

func (p *player) backward() {
	p.x -= 1
	if p.x < 0 {
		p.x = 0
	}
}

func (p *player) left() {
	p.y -= 1
	if p.y < 0 {
		p.y = 0
	}
}

func (p *player) right() {
	p.y += 1
	if p.y > levelSizeY-1 {
		p.y = levelSizeY - 1
	}
}

func (p *player) nextLevel() {
	p.level += 1
	if p.level > nLevels-1 {
		p.level = nLevels - 1
	}
}

func (p *player) prevLevel() {
	p.level -= 1
	if p.level < 0 {
		p.level = 0
	}
}

func (p *player) hurt() (dead bool, kill bool) {
	if (p.sword) {
		kill = true
	} else {
		kill = false
		p.lifes -= 1
	}
	if p.lifes < 0 {
		dead = true
	}
	dead = false
	return dead, kill
}

func rotateCharset(c rune, i int) rune {
	if c < 'a' || c > 'z' {
		return c
	}

	if cmin := c - rune(i%26); cmin < 'a' {
		return cmin + 26
	} else {
		return cmin
	}
}

func (p *player) move(c rune) (m bool){
	switch c {
	case 'k':
		p.forward()
	case 'i':
		p.backward()
	case 'j':
		p.left()
	case 'l':
		p.right()
	default:
		return false
	}
	return true
}

type level [levelSizeX][levelSizeY]dungeonEntity

type dungeon [nLevels]level


func (m *dungeon) status(p *player) (s status) {
	level := m[p.level]
	switch level[p.x][p.y] {
	case entitySword:
		p.sword = true
		s = gotSword
	case entityMap:
		p.mapp = true
		s = gotMap
	case entityPath:
		s = statusAlive
	case entityWall:
		s = statusDead
	case entityLadderUp:
		p.nextLevel()
		s = statusLevelUp
	case entityLadderDown:
		p.prevLevel()
		s = statusLevelDown
	case entityMonster:
		dead, kill := p.hurt()
		if kill {
			s = statusKill
		} else if dead {
			s = statusDeadByMonster
		} else {
			s = statusHurtByMonster
		}
	case entityFinish:
		s = statusWin
	default:
		s = statusDead
	}
	return
}

func addBase64Padding(value string) string {
    m := len(value) % 4
    if m != 0 {
        value += strings.Repeat("=", 4-m)
    }

    return value
}


func Unpad(src []byte) ([]byte, error) {
    length := len(src)
    unpadding := int(src[length-1])

    return src[:(length - unpadding)], nil
}

func win(g *gocui.Gui, sol string) {
	hash := sha256.Sum256([]byte(sol))
	var x []byte = hash[:]

	b, err := aes.NewCipher(x)
	if err != nil {
		panic(err)
	}

	y, err := base64.URLEncoding.DecodeString(addBase64Padding(boy))
	aa := y[:aes.BlockSize]
	zz := y[aes.BlockSize:]
	cfb := cipher.NewCFBDecrypter(b, aa)
	cfb.XORKeyStream(zz, zz)
	m, err := Unpad(zz)
	print (g, string(m))
}

func secret()(n uint64) {
	b := []byte{0x8, 0x0, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8}

	for k := range(b) {
		b[k] = b[k] ^ 0x08
	}
	n = binary.LittleEndian.Uint64(b)

	return n
}

func options(g *gocui.Gui, v *gocui.View) error {
	var ret error
	input, _ := v.Line(0)
	input = strings.TrimRight(input, "\n")
	v.Clear()
	fmt.Fprint(v, shell)
	v.SetCursor(len(shell), 0)
	if len(input) >= len(shell)+4 {
		cmd := input[len(shell):len(shell)+4]
		switch string(cmd) {
			case "help":
				help(g)
			case "move":
				if strings.Contains(input[5:], " ") {
					opts := strings.Split(input[5:]," ")
					moves := opts[1]
					ret = move(g,moves)
				}
			case "flag":
				flag(g)
			case "free":
				egg(g)
		}
	}
	if ret != nil {
		return ret
	} else {
		return nil
	}
}

func show_map(g *gocui.Gui, v *gocui.View) error {
	if (p.mapp) {
		mapview, _ := g.View("mapview")
		mapview.Title = "Show Map"
		if map_shown {
			for l:= 0; l < nLevels; l++ {
				for x:=0; x < levelSizeX; x++ {
					for y:=0; y < levelSizeY; y++ {
						fmt.Fprintf(mapview,"|%v|", m[l][x][y])
					}
				fmt.Fprintf(mapview,"\n")
				}
				fmt.Fprintf(mapview,"\n")
				fmt.Fprintf(mapview,"\n")
			}
			map_shown = false
		} else {
			mapview.Clear()
			map_shown = true
		}
	}
	return nil
}

func scroll_down_map(g *gocui.Gui, v *gocui.View) error {
	mapview, _ := g.View("mapview")
	mapview.SetOrigin(0,18)
	return nil
}

func scroll_up_map(g *gocui.Gui, v *gocui.View) error {
	mapview, _ := g.View("mapview")
	mapview.SetOrigin(0,0)
	return nil
}


func print(g *gocui.Gui, s string) {
	g.Update(func(g *gocui.Gui) error {
		v, err := g.View("output")
		if err != nil {
			log.Panicln(err)
		}
		fmt.Fprintln(v, s)
		return nil
	})
	return
}

func egg(g *gocui.Gui) {
	print (g, "[*] You got a free 8====D")
}

func banner (v *gocui.View) {

	fmt.Fprintln(v, `
************************************
**    The Legend of Zoltan        **
────────────────▄────────────────
──────────────▄▀░▀▄──────────────
────────────▄▀░░░░░▀▄────────────
──────────▄▀░░░░░░░░░▀▄──────────
────────▄█▄▄▄▄▄▄▄▄▄▄▄▄▄█▄────────
───────▄▀▄─────────────▄▀▄───────
─────▄▀░░░▀▄─────────▄▀░░░▀▄─────
───▄▀░░░░░░░▀▄─────▄▀░░░░░░░▀▄───
─▄▀░░░░░░░░░░░▀▄─▄▀░░░░░░░░░░░▀▄─
▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀
***********************************
`	)

}

func help(g *gocui.Gui){
	print(g, "[+] Fairy: Find the flag inside the dungeon but, be aware of the monsters and the fatigue!")
}

func flag(g *gocui.Gui){
	if fk == 1  {
		print(g,"[+] Oh man...! How many fairies you need to kill?")
	} else if  fk > 1 {
		print(g,"[+] Dude...! Trust me...this is not the right way.")
	} else {
		print(g,`[+] Ouch! You have killed the fairy :(
   (\{\
   { { \ ,~,
  { {   \)))  *
   { {  (((  /
    {/{/; ,\/
       (( '
        \ \
        (/  \
        )  \
`		)
	}
	fk +=1
}

func move(g *gocui.Gui, moves string) error{
	for _, move := range moves {
		if count > exhausted {
			return gocui.ErrQuit
		}
		s := rotateCharset(move, count)
		step := p.move(s)
		if (step) {
			print(g,"[+] Move one step")
			count++
			sol = sol + string(move)
		}
		switch m.status(p) {
			case statusLevelUp:
				print(g,"[*] Level Up!")
				continue
			case statusLevelDown:
				print(g,"[*] Level Down!")
				continue
			case statusKill:
				print(g,"[*] You have killed a monster!")
				continue
			case statusDeadByMonster:
				return gocui.ErrQuit
			case statusHurtByMonster:
				print(g,"[*] You have been hurt by a monster!")
				continue
			case statusDead:
				return gocui.ErrQuit
			case statusAlive:
				continue
			case gotSword:
				print(g,"[*] You got the sword!")
				maxX, _ := g.Size()
				if v, err := g.SetView("sword", maxX/2+10, 0, maxX/2+30, 5); err != nil {
					if err != gocui.ErrUnknownView {
						return err
					}
					v.Title = "Sword"
					fmt.Fprintln(v, `

    /
O===[====================-
    \	
`)
				}
				continue
			case gotMap:
				print(g, "[*] You got the map!")
				maxX, _ := g.Size()
				if v, err := g.SetView("map", maxX/2+10, 10, maxX/2+30, 20); err != nil {
					if err != gocui.ErrUnknownView {
						return err
					}
					v.Title = "Map"
					fmt.Fprintln(v,` 
_____________
| |   | |_| |
| ___ | |___|
|___|_|____ |
| |_______|_|
`)
				}
				continue
			case statusWin:
				print(g,"[*] Good!")
				win(g, sol)
				return nil
			default:
				return nil
		}
	}
	return nil
}

func layout(g *gocui.Gui) error {
	maxX, maxY := g.Size()
	if v, err := g.SetView("mainview", 0, 0, maxX/2+5, maxY/2-7); err != nil {
		if err != gocui.ErrUnknownView {
			return err
		}
		banner(v)
	}

	if v, err := g.SetView("output", 0, maxY/2-7, maxX/2+5, maxY/2+14); err != nil {
		if err != gocui.ErrUnknownView {
			return err
		}
		v.Autoscroll = true
	}

	if v, err := g.SetView("input", 0, maxY/2+15, maxX/2+5, maxY/2+17); err != nil {
		if err != gocui.ErrUnknownView {
			return err
		}
		fmt.Fprint(v, shell)
		v.SetCursor(len(shell),0)
		v.Editable = true
	}

	if _, err := g.SetView("mapview",maxX/2+10, 21, maxX-1, maxY/2+17); err != nil {
		if err != gocui.ErrUnknownView {
			return err
		}
	}

	if _, err := g.SetCurrentView("input"); err != nil {
            return err
    }

	return nil
}

func quit(g *gocui.Gui, v *gocui.View) error {
	return gocui.ErrQuit
}

var p *player
var m *dungeon
var fk int
var exhausted int
var shell string
var map_shown bool
var sol string
var count int
func main() {
	p = newPlayer()
	m = generateDungeon()
	fk = 0
	count = 0
	exhausted = 88
	shell = "shell>"
	map_shown = false

	g, err := gocui.NewGui(gocui.OutputNormal)

	if err != nil {
		log.Panicln(err)
    }

	defer func() {
		g.Close()
		fmt.Println("(X__X) You are Dead!!")
	}()

	g.SetManagerFunc(layout)

	if err := g.SetKeybinding("", gocui.KeyCtrlC, gocui.ModNone, quit); err != nil {
		log.Panicln(err)
	}

	if err := g.SetKeybinding("input", gocui.KeyEnter, gocui.ModNone, options); err != nil {
        log.Panicln(err)
    }

	if err := g.SetKeybinding("input", gocui.KeyCtrlM, gocui.ModNone, show_map); err != nil {
        log.Panicln(err)
    }

	if err := g.SetKeybinding("input", gocui.KeyArrowDown, gocui.ModNone, scroll_down_map); err != nil {
        log.Panicln(err)
    }
	if err := g.SetKeybinding("input", gocui.KeyArrowUp, gocui.ModNone, scroll_up_map); err != nil {
        log.Panicln(err)
    }

	if err := g.MainLoop(); err != nil && err != gocui.ErrQuit {
		log.Panicln(err)
	}
}
