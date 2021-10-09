package main

import (
	"bufio"
	"fmt"
	"io/ioutil"
	"log"
	"math/rand"
	"os"
	"strings"
	"time"

	"github.com/gdamore/tcell/v2"
)

type Game struct {
	start         time.Time
	screen        tcell.Screen
	style         tcell.Style
	width, height int
	level         int
	x, y          int
	rows, cols    int
}

type Positions struct {
	left, top, right, bottom int
	player_x, player_y       int
	door_x, door_y           int
}

func NewGame(level int) *Game {
	screen, err := tcell.NewScreen()
	if err != nil {
		log.Fatalf("%+v", err)
	}
	if err := screen.Init(); err != nil {
		log.Fatalf("%+v", err)
	}

	style := tcell.StyleDefault.Background(tcell.ColorReset).Foreground(tcell.ColorReset)
	screen.SetStyle(style)

	screen.Clear()

	width, height := screen.Size()

	return &Game{
		start:  time.Now(),
		screen: screen, style: style,
		width: width, height: height,
		level: level, x: 0, y: 0,
		rows: 8, cols: 8}
}

func (this *Game) DrawText(x1, y1, x2, y2 int, text string) {
	row := y1
	col := x1
	for _, r := range []rune(text) {
		this.screen.SetContent(col, row, r, nil, this.style)
		col++
		if col >= x2 {
			row++
			col = x1
		}
		if row > y2 {
			break
		}
	}
}

func (this *Game) DrawTitle(text string) {
	text = fmt.Sprintf("= %s =", text)
	length := len(text)
	start := int((this.width - length) / 2)
	this.DrawText(start, 0, start+length, 0, text)
}

func (this *Game) DrawRoster(text string) {
	text = fmt.Sprintf("= %s =", text)
	length := len(text)
	start := int((this.width - length) / 2)
	this.DrawText(start, this.height-1, start+length, this.height-1, text)
}

func (this *Game) DrawBox(x1, y1, x2, y2 int) {
	if y2 < y1 {
		y1, y2 = y2, y1
	}
	if x2 < x1 {
		x1, x2 = x2, x1
	}
	for row := y1; row <= y2; row++ {
		for col := x1; col <= x2; col++ {
			this.screen.SetContent(col, row, ' ', nil, this.style)
		}
	}
	for col := x1; col <= x2; col++ {
		this.screen.SetContent(col, y1, tcell.RuneHLine, nil, this.style)
		this.screen.SetContent(col, y2, tcell.RuneHLine, nil, this.style)
	}
	for row := y1 + 1; row < y2; row++ {
		this.screen.SetContent(x1, row, tcell.RuneVLine, nil, this.style)
		this.screen.SetContent(x2, row, tcell.RuneVLine, nil, this.style)
	}
	if y1 != y2 && x1 != x2 {
		this.screen.SetContent(x1, y1, tcell.RuneULCorner, nil, this.style)
		this.screen.SetContent(x2, y1, tcell.RuneURCorner, nil, this.style)
		this.screen.SetContent(x1, y2, tcell.RuneLLCorner, nil, this.style)
		this.screen.SetContent(x2, y2, tcell.RuneLRCorner, nil, this.style)
	}
}

func (this *Game) GetPositions() *Positions {

	maze_width := this.rows * 4
	maze_height := this.cols * 2

	left := int((this.width - maze_width) / 2)
	if left < 4 {
		left = 4
	}

	top := int((this.height - maze_height) / 2)
	if top < 2 {
		top = 2
	}

	right := left + maze_width
	bottom := top + maze_height

	player_x := left + 2 + (this.x * 4)
	player_y := top + 1 + (this.y * 2)

	door_x := right - 2
	door_y := bottom - 1

	return &Positions{
		left: left, top: top,
		right: right, bottom: bottom,
		player_x: player_x, player_y: player_y,
		door_x: door_x, door_y: door_y}
}

func (this *Game) DrawMaze() bool {

	pos := this.GetPositions()
	w := pos.right - pos.left
	h := pos.bottom - pos.top

	maze := NewMaze(this.rows, this.cols)
	rand.Seed(int64(this.level))
	maze.generator()

	lineno := 0
	scanner := bufio.NewScanner(strings.NewReader(maze.String()))
	for scanner.Scan() {
		line := scanner.Text()
		for rowno, char := range line {
			if rowno == 0 && lineno == 0 {
				char = tcell.RuneULCorner
			} else if rowno == w && lineno == 0 {
				char = tcell.RuneURCorner
			} else if rowno == 0 && lineno == h {
				char = tcell.RuneLLCorner
			} else if rowno == w && lineno == h {
				char = tcell.RuneLRCorner
			} else {
				if char == '|' {
					char = tcell.RuneVLine
				} else if char == '-' {
					char = tcell.RuneHLine
				} else if char == '+' {
					char = tcell.RuneDiamond
				}
			}
			this.screen.SetContent(pos.left+rowno, pos.top+lineno, char, nil, this.style)
		}
		lineno += 1
	}

	if pos.player_x == pos.door_x && pos.player_y == pos.door_y {
		if this.level == 42 {
			this.screen.SetContent(pos.door_x, pos.door_y, '💥', nil, this.style)
		} else {
			this.screen.SetContent(pos.door_x, pos.door_y, '🚪', nil, this.style)
		}
		return true
	}

	this.screen.SetContent(pos.player_x, pos.player_y, '🥷', nil, this.style)
	if this.level == 42 {
		this.screen.SetContent(pos.door_x, pos.door_y, '👿', nil, this.style)
	} else {
		this.screen.SetContent(pos.door_x, pos.door_y, '🔒', nil, this.style)
	}
	return false
}

func (this *Game) DrawScreen() {

	this.screen.Clear()
	this.DrawBox(0, 0, this.width-1, this.height-1)

	var title string
	if this.level > 42 {
		title = "YOU WIN"
	} else if this.level < 42 {
		title = fmt.Sprintf("DEPTH %d", this.level)
	} else {
		title = "FINAL BOSS"
	}
	this.DrawTitle(title)

	//end := this.start.Add(time.Minute)
	//this.DrawRoster(fmt.Sprintf("%d seconds left", int(end.Sub(time.Now()).Seconds())))

	win := false
	if this.level <= 42 {
		win = this.DrawMaze()
	} else {
		pos := this.GetPositions()
		this.DrawBox(pos.left, pos.top, pos.right, pos.bottom)
		flag := this.GetFlag()
		flag_x := pos.left + int((pos.right-pos.left-len(flag))/2)
		flag_y := pos.top + int((pos.bottom-pos.top)/2)
		this.DrawText(flag_x, flag_y, flag_x+len(flag), flag_y+1, flag)
	}

	this.screen.Sync()

	if win {
		time.Sleep(time.Second)
		this.NextLevel()
		this.DrawScreen()
	}
}

func (this *Game) GetFlag() string {
	flag := " YOU HAVE WON THE GAME"
	record := this.start.Add(time.Minute)
	if time.Now().Before(record) {
		content, err := ioutil.ReadFile("flag.txt")
		if err == nil {
			flag = string(content)
			flag = strings.TrimRight(flag, "\r\n\t ")
			flag = strings.Split(flag, "\n")[0]
		}
	}
	return flag
}

func (this *Game) MoveLeft() {

	new_x := this.x - 1
	if new_x < 0 {
		return
	}

	pos := this.GetPositions()
	char, _, _, _ := this.screen.GetContent(pos.player_x-2, pos.player_y)
	if char == tcell.RuneVLine {
		return
	}

	this.x = new_x
	this.DrawScreen()
}

func (this *Game) MoveRight() {

	new_x := this.x + 1
	if new_x > 7 {
		return
	}

	pos := this.GetPositions()
	char, _, _, _ := this.screen.GetContent(pos.player_x+2, pos.player_y)
	if char == tcell.RuneVLine {
		return
	}

	this.x = new_x
	this.DrawScreen()
}

func (this *Game) MoveUp() {

	new_y := this.y - 1
	if new_y < 0 {
		return
	}

	pos := this.GetPositions()
	char, _, _, _ := this.screen.GetContent(pos.player_x, pos.player_y-1)
	if char == tcell.RuneHLine {
		return
	}

	this.y = new_y
	this.DrawScreen()
}

func (this *Game) MoveDown() {
	new_y := this.y + 1
	if new_y > 7 {
		return
	}

	pos := this.GetPositions()
	char, _, _, _ := this.screen.GetContent(pos.player_x, pos.player_y+1)
	if char == tcell.RuneHLine {
		return
	}

	this.y = new_y
	this.DrawScreen()
}

func (this *Game) NextLevel() {
	this.x = 0
	this.y = 0
	this.level += 1
	this.DrawScreen()
}

func (this *Game) Run() {
	for {
		this.screen.Show()
		ev := this.screen.PollEvent()
		switch ev := ev.(type) {
		case *tcell.EventResize:
			this.width, this.height = ev.Size()
			this.DrawScreen()
		case *tcell.EventKey:
			if ev.Key() == tcell.KeyEscape || ev.Key() == tcell.KeyCtrlC {
				this.screen.Fini()
				os.Exit(0)
			} else if ev.Key() == tcell.KeyCtrlL {
				this.screen.Sync()
			} else if ev.Key() == tcell.KeyLeft || ev.Rune() == 'A' || ev.Rune() == 'a' {
				this.MoveLeft()
			} else if ev.Key() == tcell.KeyRight || ev.Rune() == 'D' || ev.Rune() == 'd' {
				this.MoveRight()
			} else if ev.Key() == tcell.KeyUp || ev.Rune() == 'W' || ev.Rune() == 'w' {
				this.MoveUp()
			} else if ev.Key() == tcell.KeyDown || ev.Rune() == 'S' || ev.Rune() == 's' {
				this.MoveDown()
			}
		}
	}
}
