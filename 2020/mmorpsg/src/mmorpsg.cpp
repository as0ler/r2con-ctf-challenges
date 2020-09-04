#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <limits>
#include <cstring>
#include <algorithm>
#include <unistd.h>
#include <random>


class Player {

	public:
		std::string name;
		virtual char play(){}

		virtual void winner(){
			system("/bin/sh");
		}
};

class Opponent: public Player {
	public:
		virtual char play(){
			srand ( time(NULL) ); //initialize the random seed
			const char options[3] = {'r', 'p', 's'};
			int num = rand() % 4;
			return options[num];
		}
};

static Opponent * player;
static char * nickname;

using namespace std;


std::vector<string> nick_names {
"Snowy Prince",
"Wrecking Monkey",
"Wrecking Payload",
"Shoulder Crew",
"Yarn Eagle",
"Rain Eagle",
"Ricky Drill",
"Rain Crew",
"Swamp Samedi",
"Time Owl",
"Melomy Rage",
"Harpy Rage",
"Quark Vikings",
"Anger Whispers",
"Hammer Nugget",
"Maddeningtie",
"Frog Surfer",
"After Matter",
"Hammer Samedi",
"Swamp Club",
"Weeping Piggy",
"Flash Dining",
"Green Owl",
"FINEBOOM",
"Elder Mutt",
"Roid Mayhem",
"Fine Crew",
"Fine Midnight",
"Wrecking Bang",
"Bow Packet",
"Maddening DOOM",
"DRPACKET",
"Maddening Monkey",
"Melomy Angel",
"Gaping Hole of PACKET",
"Quark Turkey",
"The Owl",
"Shoulder Punch",
"Melomy Prince",
"Fight Turkey",
"Time Stomper",
"Ricky Nugget"};


int random_number() { 
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 50.0);
    return dist(mt);
}


void help(){
	cout << endl;
	cout << "Available commands:" << endl;
	cout << endl;
	cout << "| l\t\tList plugins" << endl;
	cout << "| i\t\tInstall plugins" << endl;
	cout << "| u\t\tUninstall plugins" << endl;
	cout << "| q\t\tQuit" << endl;
	cout << endl;
}


void print_menu(){
		cout << endl;

		if (nickname != NULL ){
			cout << "Welcome "<<nickname<<endl;
		}

		cout << "1. Set your battle nick name" << endl;
		cout << "2. Search an online opponent" << endl;
		cout << "3. Play" << endl;
		cout << "4. Disconnect" << endl;

}

int veredict(char a, char b){

		if(a=='r'){
				if (b=='r') return 1;
				if (b=='p') return 2;
				if (b=='s') return 0;

		}else if (a== 'p'){
				if (b=='r') return 0;
				if (b=='p') return 1;
				if (b=='s') return 2;

		} else if (a== 's'){
				if (b=='r') return 2;
				if (b=='p') return 0;
				if (b=='s') return 1;
		}
}

void set_nickname(){

		cout << "Enter name length: ";
		int num;
		cin >> num;
		nickname = new char[num];

		cout << "Enter your new nickname: ";
		cin >> nickname;

		cout << "Your new nickname is "<< nickname << endl;
}


void play_round(){

		if(player == NULL){
			cout << "You should search a player first" << endl;
			return;
		}

		while (true){

		cout << endl;
		cout << "What is your choice? [r]ock, [p]aper or [s]cissor: " << endl;

		char c;
		cin >> c;

		if(c != 'r' && c != 'p' && c!= 's'){
			cout << "du fuk?" << endl;
			continue;
		}

		char o = player->play();

		cout << "\tYou selected [" << c << "] and " << player->name << " selected [" << o << "]" << endl;
		int v = veredict(c, o);
		switch(v){
			case 0:
				cout << "\tWell done! You won" << endl;
				break;
			case 1:
				cout << "\tIt is a draw!" << endl;
				break;
			case 2:
				cout << "\tPuff..loser.." << endl;
				break;
		}

			cout << endl;
			cout << "Do you want to play again with this player? (y/n)";
			char d;
			cin >> d;
			if (d == 'n'){
				delete player;
				break;
			}
		}
}

void search_player(){
	cout << endl;
	cout << "Searching a player..." << endl;
	int pos = random_number() % nick_names.size();

	player = new Opponent();
	player->name = nick_names[pos];

	cout << "Your opponent is "<< player->name << "!!"<< endl;

}




int main(){

	cout << endl;
	cout << "Welcome to the first MMORPSG in the world!" << endl;
	cout << "============================================" << endl;
	cout << endl;
	cout << "(MMORPSG = Massively Multiplayer Online Rock-Paper-Scissors Game)" << endl;
	cout << endl;
	cout << "Select an option: " << endl;


	bool quit = false;


	int opt;

	while(!quit){
		print_menu();

			cin >> opt;
			cin.clear();
			cin.ignore(100, '\n');

			switch(opt){
				case 1:
					set_nickname();
					break;
				case 2:
					search_player();
					break;
				case 3:
					play_round();
					break;
				case 4:
					cout << "Disconnecting...ByeBye C0m3po!" << endl;
					quit = true;
					break;
				default:
					cout << "Incorrect option" << endl;
			}
	}

	return 0;

}
