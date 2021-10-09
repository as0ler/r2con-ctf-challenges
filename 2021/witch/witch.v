import tic

struct State {
mut:
	x       int
	y       int
	cur     int
	sel     [3]int
	pending bool
	action  Action
	complete bool
	text    string
	msgleft int
}

enum Action {
	nothing = 0
	right
	left
	up
	down
	prepare
}

enum Sprites {
	potion = 32
	bat = 34
	frog = 36
}

const sprites = [
	Sprites.potion,
	Sprites.bat,
	Sprites.frog,
]

__global state = State{}

[inline]
fn compare() bool {
	// return true
	return state.sel[0] == 2 && state.sel[1] == 0 && state.sel[2] == 1
}

[inline]
fn compute() string {
	la := fn(a int) string {
		   if a == 0 { return 'P' }
		   if a == 1 { return 'B' }
		   if a == 2 { return 'F' }
		   return '?'
	}
	mut f := ''
	f += la(state.sel[0])
	f += la(state.sel[1])
	f += la(state.sel[2])
	return f
}

[export: 'TIC']
fn tick(a int) {
	tic.clear(int(tic.Color.ocean))
	if state.complete {
		tic.printxy('<witch> ${state.text}!', 0, 30)
		return
	}
	tic.printxy('Prepare the potion!', 10, 10)
	mut any := false
	if tic.button(.up) || tic.keypress(.k) {
		state.action = .up
		state.pending = true
		any = true
	}
	if tic.button(.down) || tic.keypress(.j) {
		state.action = .down
		state.pending = true
		any = true
	}
	if tic.button(.left) || tic.keypress(.h) {
		state.action = .left
		state.pending = true
		any = true
	}
	if tic.button(.right) || tic.keypress(.l) {
		state.action = .right
		state.pending = true
		any = true
	}
	if tic.button(.a) || tic.button(.b) || tic.keypress(.enter) || tic.keypress(.space) {
		state.action = .prepare
		state.pending = true
		any = true
	}
	if !any && state.pending {
		match state.action {
			.up {
				state.sel[state.cur]--
			}
			.down {
				state.sel[state.cur]++
			}
			.left {
				state.cur--
			}
			.right {
				state.cur++
			}
			.prepare {
				if compare() {
					// le winrar
					flagstr := compute()
					state.text = 'Congrats! The flag is: $flagstr'
					state.complete = true
				} else {
					// le big failure, the witch is angry
					state.text = 'This is the wrong potion!'
					state.msgleft = 120
				}
			}
			.nothing {}
		}
		if state.sel[state.cur] < 0 {
			state.sel[state.cur] = 2
		}
		if state.sel[state.cur] > 2 {
			state.sel[state.cur] = 0
		}
		if state.cur < 0 {
			state.cur = 2
		}
		if state.cur > 2 {
			state.cur = 0
		}
		state.pending = false
	}

	sprite_size := 40
	for i := 0; i < 3; i++ {
		xpos := 40 + (i * sprite_size)
		sprite := sprites[state.sel[i]]
		tic.sprite_full(int(sprite), xpos, sprite_size, 0, 2, 0, 0, 2, 2)
		if i == state.cur {
			tic.rectangle(xpos, 40 + sprite_size, 32, 4, int(tic.Color.green))
		}
	}
	if state.msgleft > 0 {
		state.msgleft--
		tic.printxy('<witch> ${state.text}!', 0, 30)
	}
}

fn main() {
	state = State{}
	state.sel = [3]int{}
}
