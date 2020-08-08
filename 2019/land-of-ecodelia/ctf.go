package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

func encode(password, key string) ([]byte, error) {
	output := make([]byte, len(password))
	for pos, char := range password {
		output = append(output, byte(char) ^ key[pos % len(key)])
	}
	return output, nil
}

func check(a, b []byte) bool {
	if len(a) != len(b) {
        return false
    }
    for i, v := range a {
        if v != b[i] {
            return false
        }
    }
    return true
}

func main() {

	reader := bufio.NewReader(os.Stdin)
	fmt.Println("Welcome To The Greatest Adventure Of Your Life")
	fmt.Println("---------------------")

	fmt.Println("In this game, I will ask you questions and you will have to answer. Some valid answers are y or n, but these are not the only ones. You will need to figure out the rest by yourself :)")
	fmt.Println("---------------------")

	var questions [9]string
	questions[0] = "Do you understand the rules?\n"
	questions[1] = "Great. First question: Have you ever cried during sex?\n"
	questions[2] = "Hmmm. Are you red or purple?\n"
	questions[3] = "Are you a giraffe or a seagull?\n"
	questions[4] = "Is the key in the room?\n"
	questions[5] = "You are standing in a dark room and can’t see anything. There is a torch and a match. What do you do?\n"
	questions[6] = "The torch fills the dark room with light. You see a door in front of you. What do you do?\n"
	questions[7] = "How do you open it?\n"
	questions[8] = "The knob doesn't turn. How do you open it?\n"
	
	var keys [9]string
	keys[0] = "n"
	keys[1] = "of course"
	keys[2] = "red"
	keys[3] = "giraffe"
	keys[4] = "yes!"
	keys[5] = "suicide"
	keys[6] = "close"
	keys[7] = "hammer"
	keys[8] = "fuckoff"

	var answers [9][]byte
	answers[0] = []byte{0, 23} // y
	answers[1] = []byte{0 , 0 , 0 , 0 , 0 , 1 , 3 , 86 , 6 , 29} // never
	answers[2] = []byte{0 , 0 , 0 , 0 , 16 , 10 , 16 , 26} // both
	answers[3] = []byte{0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 18 , 26 , 23 , 19 , 15 , 9 , 16 , 20} // userious
	answers[4] = []byte{0 , 0 , 0 , 0 , 14 , 13 , 18 , 85} // what
	answers[5] = []byte{0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 31 , 28 , 14 , 11 , 29 , 16 , 13 , 22 , 1 , 6 , 17 , 10 , 12} // lightthetorch
	answers[6] = []byte{0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 10 , 11 , 26 , 22 , 22 , 16 , 5 , 0 , 3 , 0 , 13 , 5 , 27} // iguessiopenit
	answers[7] = []byte{0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 28 , 20 , 31 , 3 , 17 , 26 , 13 , 10 , 3 , 2 , 7} // turntheknob
	answers[8] = []byte{0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 49 , 60 , 55 , 35 , 46 , 45 , 35 , 63 , 60 , 54 , 56 , 42 , 39 , 45 , 35 , 44} // WITHAKEYIUSEAKEY

	for i := 0; i < 9; i++ {
		fmt.Print(questions[i])
		text, _ := reader.ReadString('\n')
		// convert CRLF to LF
		text = strings.Replace(text, "\n", "", -1)
		
		hash, _ := encode(text, keys[i])
		// fmt.Println(hash)

		if (!check(hash, answers[i])) {
			fmt.Println("Your poor decisions led you to your death. Sorry.")
			os.Exit(1)
		}
	}

	fmt.Println("Thank you.")
	fmt.Println("---------------------")
	fmt.Println("The flag is r2con2019{all_your_answers_in_order_without_spaces}")

}