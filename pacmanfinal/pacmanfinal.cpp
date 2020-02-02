#include <iostream>
#include <stdio.h>	//using C library to print map  on screen
#include <windows.h> //to take input to control character in game
#include <string> // to use strings in C++
#include <string.h> // to use strings in C
#include <vector> // to move the villains around
#include <fstream> // to save highscore into text file
#undef max // used to ignore certain input after enter was pressed

using namespace std;


// variables 
char map[30][70];	//used to upload text file into map
char tmp_map[30][70]; //temporary copy of map used for villains to chase pacman in add array function
int x; //used to save direction of pacman
int y; // used also for direction of pacman
string pacman, lamey, cortex, bob, crusher; // to give each characters look in game
int ex, ex2, ex3, ex4; //for enemies location
int ey, ey2, ey3, ey4;
int pts;			//save the score
int target_x;		//directions given to the villains in x plane	
int target_y;		//directions given to villains in y plane
int old_x;			//temporary storage of directions for pacman
int old_y;
int con;			//used to keep pacman continuosly moving in the same direction
int con2;
int i;				//used for counters
int lives;			//pacmans lives
int frame, frame_old;		//counter for when pacman eats the power pellet
int frame1, frame2, frame3, frame4;		//counter which stores when each villain leaves the homebase
int sp2 = 1, sp3 = 2, sp4 = 3, sp5 = 4;	//different speed modes for each villain
int k, l, num_dot;			///used for counters and num_dot is used for counting coins
bool power_pallet;			//checks whether pacman still has the power pellet powers
int choice;					//control arrow used in main menu
int enter;					//used for when i press enter in main menu
bool blinking;				//to make the arrows blink in main menu
int wait, wait2, wait3, wait4;		//wait timer for villain animation in main menu
int chase_x, chase_x2, chase_x3, chase_x4;		//used in villain animation in main menu '
int chase_y, chase_y2, chase_y3, chase_y4;


//to assign default starting positions for when the game starts
void assign_variables(int &x, int &y, string &pacman, int &ex, int &ex2, int &ex3, int &ex4, int &ey, int &ey2, int &ey3, int &ey4, int &pts, int &target_x, int &target_y, int &old_x, int &old_y, int &con, int &con2, int &i, int &lives, int &frame, int &frame_old, int &frame1, int &frame2, int &frame3, int &frame4, int &k, int &l, int &num_dot, bool &power_pallet, int &choice, int &enter, bool &blinking) {
	x = 33;
	y = 14;
	ex = 33, ex2 = 35, ex3 = 33, ex4 = 31;
	ey = 11, ey2 = 12, ey3 = 12, ey4 = 12;
	pts = 0;
	target_x = 0;
	target_y = 0;
	con = 0;
	con2 = 0;
	i = 0;
	lives = 3;
	frame = 0, frame_old = 0;
	frame1 = 0, frame2 = 0, frame3 = 0, frame4 = 0;
	k = 0, l = 0, num_dot = 507;
	power_pallet = false;
	pacman = ":)";
}

//default starting positions for animation in main menu
void assign_variables_for_animation(int &x, int &y, string &pacman, int &ex, int &ex2, int &ex3, int &ex4, int &ey, int &ey2, int &ey3, int &ey4, int &pts, int &target_x, int &target_y, int &old_x, int &old_y, int &con, int &con2, int &i, int &lives, int &frame, int &frame_old, int &frame1, int &frame2, int &frame3, int &frame4, int &k, int &l, int &num_dot, bool &power_pallet, int &choice, int &enter, bool &blinking, int &wait, int &wait2, int &wait3, int &wait4) {
	x = 27;
	y = 9;
	ex = 19, ex2 = 13, ex3 = 7, ex4 = 1;
	ey = 9, ey2 = 9, ey3 = 9, ey4 = 9;
	target_x = 0;
	target_y = 0;
	con = 4;
	con2 = 4;
	i = 0;
	frame = 0, frame_old = 0;
	frame1 = 0, frame2 = 0, frame3 = 0, frame4 = 0;
	k = 0, l = 0;
	power_pallet = false;
	choice = 1;
	enter = 0;
	pacman = ":)";
	blinking = true;
	wait = 0;
	wait2 = 0;
	wait3 = 0;
	wait4 = 0;

}


struct walk {				//used to make villains chase you
	short walk_count;
	short x;
	short y;
	short back;
};


struct target {				//stores directions of monsters
	short x;
	short y;
};
vector<target> walk_queue;
vector<walk> BFSArray;


//function prototypes

void ShowMap();														//to display the map
void won(int pts);													//shows you animation when you win the round
void highscorefunc(int pts);										//checks if you made it to the high score and lets you input your name if you have
void load_the_map();												//loads the map into the array
void load_the_map();												//loads the main menu map
void load_animation_map();									//loads the map in the highscore page
void gotoxy(short x, short y);										//used for format of printing text
void AddArray(int x, int y, int wc, int back);						//used to make the monsters chase you
void FindPath(int sx, int sy, int x, int y);						//used also in chasing
void printFigures();												//to print the score, lives, pacman and monsters in the game
void check_for_inputs(int &con2, int &num_dot, int pts);			//to take your inputs to control the game


																	//classes

class CCreature {

public:
	void Findpath(int frame, int sp, int x, int y, int &ex, int &ey, int fixspeed) {		//used in chasing pacman

		if (frame >= fixspeed && frame % sp == 0) {
			FindPath(ex, ey, x, y);
			if (walk_queue.size() != 0) {
				ex = walk_queue.back().x;
				ey = walk_queue.back().y;
			}
			walk_queue.clear();
		}


	}
	void killing_monster(int &frame, int &pts, int &ex, int &ey, int old_ex, int old_ey) {		//checks if you ate a villain when you have power pellet mode on

		if ((ex == x && ey == y) || (ex == x + 1 && ey == y) || (ex + 1 == x && ey == y) || (ex == x && ey == y + 1) || (ex == x + 1 && ey == y - 1) || (ex + 2 == x && ey == y) || (ex - 2 == x && ey == y)) {
			pts += 100;
			frame = 0;
			gotoxy(ex, ey); cout << "  ";
			ex = old_ex;
			ey = old_ey;
		}

	}
	void killing_monster_main(int &frame, int &pts, int &ex, int &ey, int old_ex, int old_ey, string &monster, string new_monster, string name, int &wait) {			//checks if you ate the villains during main menu animation

		if ((ex == x && ey == y) || (ex == x + 1 && ey == y) || (ex + 1 == x && ey == y) || (ex == x && ey == y + 1) || (ex == x + 1 && ey == y - 1) || (ex + 2 == x && ey == y) || (ex - 2 == x && ey == y)) {
			pts += 100;
			gotoxy(ex, ey); cout << "  ";
			ex = old_ex;
			ey = old_ey;
			wait = 500;
			monster = new_monster;

			if (ex == 58 && ey == 2) {
				gotoxy(ex - 7, ey); cout << name;
			}
			else if (ex == 58 && ey == 8) {
				gotoxy(ex - 4, ey); cout << name;
			}
			else if ((ex == 10 && ey == 8) || (ex == 10 && ey == 2)) {
				gotoxy(ex + 3, ey); cout << name;
			}
		}

	}

	//checks whether you've died that round

	void check_death(int &x, int &y, int &ex, int &ey, int &ex2, int &ey2, int &ex3, int &ey3, int &ex4, int &ey4, int &frame, int &frame1, int &frame2, int &frame3, int &frame4, int &lives) {
		if ((ex == x && ey == y) || (ex == x + 1 && ey == y) || (ex + 1 == x && ey == y) || (ex2 == x && ey2 == y) || (ex2 == x + 1 && ey2 == y) || (ex2 + 1 == x && ey2 == y) || (ex3 == x && ey3 == y) || (ex3 == x + 1 && ey3 == y) || (ex3 + 1 == x && ey3 == y) || (ex4 == x && ey4 == y) || (ex4 == x + 1 && ey4 == y) || (ex4 + 1 == x && ey4 == y)) {
			lives--;
			frame = 0;
			frame1 = 0;
			frame2 = 0;
			frame3 = 0;
			frame4 = 0;
			ex = 33, ex2 = 35, ex3 = 33, ex4 = 31;
			ey = 11, ey2 = 12, ey3 = 12, ey4 = 12;
			x = 33;
			y = 14;

			if (lives == 2) {

				gotoxy(25, 13);  cout << " _______________";
				gotoxy(25, 14);  cout << "|               |";
				gotoxy(25, 15);  cout << "|  You died :(  |";
				gotoxy(25, 16);  cout << "|_______________|";

				Sleep(3000);
				gotoxy(25, 13);  cout << " _______________";
				gotoxy(25, 14);  cout << "|               |";
				gotoxy(25, 15);  cout << "|     Ready?    |";
				gotoxy(25, 16);  cout << "|_______________|";


				Sleep(2000);
				gotoxy(25, 13);  cout << " _______________";
				gotoxy(25, 14);  cout << "|               |";
				gotoxy(25, 15);  cout << "|      GOO!     |";
				gotoxy(25, 16);  cout << "|_______________|";

				Sleep(500);
				system("cls");
				ShowMap();
				gotoxy(32, 25); cout << ":)    ";

			}
			else if (lives == 1) {
				gotoxy(22, 13);  cout << " _____________________";
				gotoxy(22, 14);  cout << "|                     |";
				gotoxy(22, 15);  cout << "|  You died again :(  |";
				gotoxy(22, 16);  cout << "|    and last live    |";
				gotoxy(22, 17);  cout << "|_____________________|";


				Sleep(3000);
				system("cls");
				ShowMap();
				gotoxy(25, 13);  cout << " _______________";
				gotoxy(25, 14);  cout << "|               |";
				gotoxy(25, 15);  cout << "|     Ready?    |";
				gotoxy(25, 16);  cout << "|_______________|";

				Sleep(1500);
				gotoxy(25, 13);  cout << " _______________";
				gotoxy(25, 14);  cout << "|               |";
				gotoxy(25, 15);  cout << "|      GOO!     |";
				gotoxy(25, 16);  cout << "|_______________|";
				Sleep(500);
				system("cls");
				ShowMap();
				gotoxy(32, 25); cout << "      ";

			}
			else if (lives == 0) {
				gotoxy(22, 13);  cout << " _____________________";
				gotoxy(22, 14);  cout << "|                     |";
				gotoxy(22, 15);  cout << "|  YOU LOST NO MORE   |";
				gotoxy(22, 16);  cout << "|        LIVES        |";
				gotoxy(22, 17);  cout << "|_____________________|";


				Sleep(3000);
				Sleep(500);
				system("cls");
				ShowMap();
				gotoxy(32, 25); cout << "       ";

			}



		}


	}
	void chasing_in_a_loop(int &x, int &y, int ex, int ey) {			//used in main menu animation
		if (ex == 58 && ey == 2) {
			x = 58;
			y = 8;
		}
		else if (ex == 58 && ey == 8) {
			x = 10;
			y = 8;
		}
		else if (ex == 10 && ey == 8) {
			x = 10;
			y = 2;
		}
		else if (ex == 10 && ey == 2) {
			x = 58;
			y = 2;
		}

	}
};
class CDungeon {
private:
	int k, l;

public:
	void check_gate_and_powerpallet(int &x, int &y, bool &power_pallet, int frame, int &old_frame) {			//used in teleporting in between gates and whether pacman has ate the power pallet
		if (map[y][x - 1] == '<') {
			for (k = 0; k < 30; k++) {
				for (l = 0; l < 70; l++) {
					if (map[k][l] == '>') {
						x = l - 3;
						y = k;
					}
				}
			}
		}
		else if (map[y][x + 2] == '>') {
			for (k = 0; k < 30; k++) {
				for (l = 0; l < 70; l++) {
					if (map[k][l] == '<') {
						x = l + 2;
						y = k;
					}
				}
			}
		}

		if (map[y][x] == '0' || map[y][x + 1] == '0') {
			power_pallet = true;
			old_frame = frame;
		}
	}
	void print(int x, int y, int ex, int ey, int ex2, int ey2, int ex3, int ey3, int ex4, int ey4, int old_x, int old_y) {			//used in deleting the printed characters left behind when moving villains in game 
		gotoxy(x, y); cout << pacman;


		gotoxy(ex, ey); cout << map[ey][ex];
		gotoxy(ex + 1, ey); cout << map[ey][ex + 1];

		gotoxy(ex2, ey2); cout << map[ey2][ex2];
		gotoxy(ex2 + 1, ey2); cout << map[ey2][ex2 + 1];

		gotoxy(ex3, ey3); cout << map[ey3][ex3];
		gotoxy(ex3 + 1, ey3); cout << map[ey3][ex3 + 1];

		gotoxy(ex4, ey4); cout << map[ey4][ex4];
		gotoxy(ex4 + 1, ey4); cout << map[ey4][ex4 + 1];

		gotoxy(ex4, ey4); cout << map[ey4][ex4];
		gotoxy(ex4 + 1, ey4); cout << map[ey4][ex4 + 1];

		map[old_y][old_x] = ' ';
		map[old_y][old_x + 1] = ' ';

	}
	void print_without_pacman(int ex, int ey, int ex2, int ey2, int ex3, int ey3, int ex4, int ey4, int old_x, int old_y) {		//used in deleting the printed characters left behind when moving villains in animation

		gotoxy(ex, ey); cout << map[ey][ex];
		gotoxy(ex + 1, ey); cout << map[ey][ex + 1];

		gotoxy(ex2, ey2); cout << map[ey2][ex2];
		gotoxy(ex2 + 1, ey2); cout << map[ey2][ex2 + 1];

		gotoxy(ex3, ey3); cout << map[ey3][ex3];
		gotoxy(ex3 + 1, ey3); cout << map[ey3][ex3 + 1];

		gotoxy(ex4, ey4); cout << map[ey4][ex4];
		gotoxy(ex4 + 1, ey4); cout << map[ey4][ex4 + 1];

		gotoxy(ex4, ey4); cout << map[ey4][ex4];
		gotoxy(ex4 + 1, ey4); cout << map[ey4][ex4 + 1];

		map[old_y][old_x] = ' ';
		map[old_y][old_x + 1] = ' ';
	}

	void manage_movement(int &x, int &y, int &con, int &pts, string &pacman) {					//used for collision detection

		if ((map[y - 1][x] == ' ' || map[y - 1][x] == '.' || map[y - 1][x] == '0') && (map[y - 1][x + 1] == ' ' || map[y - 1][x + 1] == '.' || map[y - 1][x + 1] == '0') && con == 1) {
			if (map[y - 1][x] == '.' || map[y - 1][x] == '0') { pts += 2; }
			y--;
			con = 1;
		}
		if ((map[y + 1][x] == ' ' || map[y + 1][x] == '.' || map[y + 1][x] == '0') && (map[y + 1][x + 1] == ' ' || map[y + 1][x + 1] == '.' || map[y + 1][x + 1] == '0') && con == 2) {
			if (map[y + 1][x] == '.' || map[y + 1][x] == '0') { pts += 2; }
			y++;
			con = 2;
		}

		if ((map[y][x - 2] == ' ' || map[y][x - 2] == '.' || map[y][x - 2] == '0') && (map[y][x - 1] != '#' && map[y][x - 1] != '|') && con == 3) {
			if (map[y][x - 1] == '.' || map[y][x - 2] == '.') pts += 2;
			pacman = ":)";
			x -= 2; con = 3;
		}
		else if ((map[y][x - 1] == ' ' || map[y][x - 1] == '.' || map[y][x - 1] == '0') && con == 3) {
			if (map[y][x - 1] == '.' || map[y][x - 1] == '0') pts += 2;
			pacman = ":)";
			x -= 1; con = 3;
		}


		if ((map[y][x + 3] == ' ' || map[y][x + 3] == '.' || map[y][x + 3] == '0') && (map[y][x + 2] != '#' && map[y][x + 2] != '|') && con == 4) {
			if (map[y][x + 1] == '.' || map[y][x + 2] == '.' || map[y][x + 1] == '0' || map[y][x + 2] == '0') pts += 2;
			pacman = "(:";
			x += 2; con = 4;
		}
		else if ((map[y][x + 2] == ' ' || map[y][x + 2] == '.' || map[y][x + 2] == '0') && con == 4) {
			if (map[y][x + 2] == '.' || map[y][x + 2] == '0') pts += 2;
			pacman = "(:";
			x += 1; con = 4;
		}

	}

	void continue_moving(int &x, int &y, int &con, int &con2) {				//used in making pacman continuosly in one direction

		if (((map[y - 1][x] == ' ' || map[y - 1][x] == '.' || map[y - 1][x] == '0') && (map[y - 1][x + 1] == ' ' || map[y - 1][x + 1] == '.' || map[y - 1][x + 1] == '0')) && con2 == 1) { con = 1; }
		if (((map[y + 1][x] == ' ' || map[y + 1][x] == '.' || map[y + 1][x] == '0') && (map[y + 1][x + 1] == ' ' || map[y + 1][x + 1] == '.' || map[y + 1][x + 1] == '0')) && con2 == 2) { con = 2; }
		if (((map[y][x - 2] == ' ' || map[y][x - 2] == '.' || map[y][x - 2] == '0') && (map[y][x - 1] != '#' && map[y][x - 1] != '|') || (map[y][x - 1] == ' ' || map[y][x - 1] == '.' || map[y][x - 1] == '0')) && con2 == 3) { con = 3; }
		if (((map[y][x + 3] == ' ' || map[y][x + 3] == '.' || map[y][x + 3] == '0') && (map[y][x + 2] != '#' && map[y][x + 2] != '|') || (map[y][x + 2] == ' ' || map[y][x + 2] == '.' || map[y][x + 2] == '0')) && con2 == 4) { con = 4; }

	}



};
class CRolePlayingGame {

public:
	void For_mainmenue_animation(int frame) {						//for animation in main menu
		CCreature Pacman, monster, high_score;
		CDungeon Map;

		gotoxy(x, y); cout << "  ";

		old_x = x;
		old_y = y;

		if ((x == 0) && (y == 1)) {



		}
		else {
			Map.continue_moving(x, y, con, con2);
			Map.manage_movement(x, y, con, pts, pacman);
			Map.check_gate_and_powerpallet(x, y, power_pallet, frame, frame_old);
		}

		if (power_pallet) {
			con = 3;
			con2 = 3;
		}
		else {
			con = 4;
			con2 = 4;
		}
		if ((x == 0) && (y == 1)) {
			Map.print_without_pacman(ex, ey, ex2, ey2, ex3, ey3, ex4, ey4, old_x, old_y);

		}
		else {
			Map.print(x, y, ex, ey, ex2, ey2, ex3, ey3, ex4, ey4, old_x, old_y);
		}
		if (power_pallet == false) {
			target_x = x;
			target_y = y;
			frame_old = frame;
			sp2 = 1;
			lamey = cortex = bob = crusher = "$$";

			if ((x == 0) && (y == 1)) {

				high_score.chasing_in_a_loop(chase_x, chase_y, ex, ey);
				monster.Findpath(frame, sp2, chase_x, chase_y, ex, ey, 0);

				high_score.chasing_in_a_loop(chase_x2, chase_y2, ex2, ey2);
				monster.Findpath(frame, sp2, chase_x2, chase_y2, ex2, ey2, 0);

				high_score.chasing_in_a_loop(chase_x3, chase_y3, ex3, ey3);
				monster.Findpath(frame, sp2, chase_x3, chase_y3, ex3, ey3, 0);

				high_score.chasing_in_a_loop(chase_x4, chase_y4, ex4, ey4);
				monster.Findpath(frame, sp2, chase_x4, chase_y4, ex4, ey4, 0);


			}
			else {
				monster.Findpath(frame, sp2, target_x, target_y, ex, ey, 0);
				monster.Findpath(frame, sp2, target_x, target_y, ex2, ey2, 0);
				monster.Findpath(frame, sp2, target_x, target_y, ex3, ey3, 0);
				monster.Findpath(frame, sp2, target_x, target_y, ex4, ey4, 0);
			}

			gotoxy(ex, ey);   cout << "D:";
			gotoxy(ex2, ey2); cout << "):";
			gotoxy(ex3, ey3); cout << ">:";
			gotoxy(ex4, ey4); cout << "}:";

		}
		else if (power_pallet == true) {


			sp2 = 3;


			monster.Findpath(frame, sp2, 0, 9, ex, ey, wait);
			monster.Findpath(frame, sp2, 0, 9, ex2, ey2, wait2);
			monster.Findpath(frame, sp2, 0, 9, ex3, ey3, wait3);
			monster.Findpath(frame, sp2, 0, 9, ex4, ey4, wait4);


			monster.killing_monster_main(frame, pts, ex, ey, 10, 2, lamey, "D:", "lamey", wait);
			monster.killing_monster_main(frame, pts, ex2, ey2, 58, 2, cortex, ":c", "cortex", wait2);
			monster.killing_monster_main(frame, pts, ex3, ey3, 58, 8, bob, ":<", "bob", wait3);
			monster.killing_monster_main(frame, pts, ex4, ey4, 10, 8, crusher, "}:", "crusher", wait4);

			gotoxy(ex, ey); cout << lamey;
			gotoxy(ex2, ey2); cout << cortex;
			gotoxy(ex3, ey3); cout << bob;
			gotoxy(ex4, ey4); cout << crusher;
		}


		if (frame - frame_old >= 100) {
			power_pallet = false;
		}
	}


	void start_the_game() {						//to start the game
		while (lives > 0) {

			CCreature Pacman, monster;
			CDungeon Map;

			gotoxy(x, y); cout << "  ";

			old_x = x;
			old_y = y;

			check_for_inputs(con2, num_dot, pts);


			Map.continue_moving(x, y, con, con2);
			Map.manage_movement(x, y, con, pts, pacman);
			Map.check_gate_and_powerpallet(x, y, power_pallet, frame, frame_old);
			Map.print(x, y, ex, ey, ex2, ey2, ex3, ey3, ex4, ey4, old_x, old_y);


			if (power_pallet == false) {
				target_x = x;
				target_y = y;
				frame_old = frame;
				sp2 = 1;
				sp3 = 2;
				sp4 = 2;
				sp5 = 3;

				monster.Findpath(frame1, sp5, target_x, target_y, ex, ey, 50);
				monster.Findpath(frame2, sp4, target_x, target_y, ex2, ey2, 100);
				monster.Findpath(frame3, sp3, target_x, target_y, ex3, ey3, 150);
				monster.Findpath(frame4, sp2, target_x, target_y, ex4, ey4, 200);

				gotoxy(ex, ey);   cout << "D:";
				gotoxy(ex2, ey2); cout << ":c";
				gotoxy(ex3, ey3); cout << ">:";
				gotoxy(ex4, ey4); cout << ":{";

				Pacman.check_death(x, y, ex, ey, ex2, ey2, ex3, ey3, ex4, ey4, frame, frame1, frame2, frame3, frame4, lives);
			}
			else if (power_pallet == true) {

				target_x = 65 - x;
				target_y = 24 - y;
				sp2 = sp3 = sp4 = sp5 = 3;

				monster.Findpath(frame1, sp5, target_x, target_y, ex, ey, 50);
				monster.Findpath(frame2, sp4, target_x, target_y, ex2, ey2, 100);
				monster.Findpath(frame3, sp3, target_x, target_y, ex3, ey3, 150);
				monster.Findpath(frame4, sp2, target_x, target_y, ex4, ey4, 200);

				gotoxy(ex, ey); cout << "$$";
				gotoxy(ex2, ey2); cout << "$$";
				gotoxy(ex3, ey3); cout << "$$";
				gotoxy(ex4, ey4); cout << "$$";

				monster.killing_monster(frame1, pts, ex, ey, 33, 11);
				monster.killing_monster(frame2, pts, ex2, ey2, 35, 12);
				monster.killing_monster(frame3, pts, ex3, ey3, 33, 12);
				monster.killing_monster(frame4, pts, ex4, ey4, 31, 12);
			}


			if (frame - frame_old >= 100) {
				power_pallet = false;
			}


			gotoxy(10, 25); cout << pts;
			Sleep(125);
			frame++;
			frame1++;
			frame2++;
			frame3++;
			frame4++;

			if (num_dot == 0) {				//checks if you won the game
				won(pts);
			}
			num_dot = 0;
			for (k = 0; k < 30; k++) {
				for (l = 0; l < 70; l++) {
					if (map[k][l] == '.') {
						num_dot++;

					}
				}
			}



		}

	}

};


void main()
{
	CRolePlayingGame start;
	assign_variables_for_animation(x, y, pacman, ex, ex2, ex3, ex4, ey, ey2, ey3, ey4, pts, target_x, target_y, old_x, old_y, con, con2, i, lives, frame, frame_old, frame1, frame2, frame3, frame4, k, l, num_dot, power_pallet, choice, enter, blinking, wait, wait2, wait3, wait4);
	load_animation_map();		//loads the main menu map from txt file into array
	system("cls");				//clears the screen
	ShowMap();					//prints the map onto the screen

	gotoxy(21, 10);  cout << " ________________________";					//for message sign
	gotoxy(21, 11);  cout << "|                        |";
	gotoxy(21, 12);  cout << "|                        |";
	gotoxy(21, 13);  cout << "|       Play Game        |";
	gotoxy(21, 14);  cout << "|       Highscore        |";
	gotoxy(21, 15);  cout << "|       EXIT GAME        |";
	gotoxy(21, 16);  cout << "|                        |";
	gotoxy(21, 17);  cout << "|________________________|";

	while (1) {																		//used in programming the arrow in the main menu
		if (GetAsyncKeyState(VK_UP) && choice != 1) { choice--; enter = 0; }
		if (GetAsyncKeyState(VK_DOWN) && choice != 3) { choice++; enter = 0; }
		if (GetAsyncKeyState(VK_RETURN) && i > 3) {
			enter = 1;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		if (i % 4 == 0) blinking = !blinking;
		frame++;
		start.For_mainmenue_animation(frame);

		if (choice == 1) {

			if (blinking) {
				gotoxy(39, 13); cout << "<--";
			}
			else {
				gotoxy(39, 13); cout << "   ";
			}
			gotoxy(39, 14); cout << "   ";

		}
		else if (choice == 2) {
			gotoxy(39, 13); cout << "   ";
			gotoxy(39, 15); cout << "   ";

			if (blinking) {
				gotoxy(39, 14); cout << "<--";
			}
			else {
				gotoxy(39, 14); cout << "   ";
			}
		}
		else if (choice == 3) {
			gotoxy(39, 14); cout << "   ";

			if (blinking) {
				gotoxy(39, 15); cout << "<--";
			}
			else {
				gotoxy(39, 15); cout << "   ";
			}
		}


		if (choice == 1 && enter == 1) {
			break;
		}
		else if (choice == 2 && enter == 1) {
			enter = 0;
			highscorefunc(0);
		}
		else if (choice == 3 && enter == 1) {
			cout << endl << endl;
			exit(0);
		}
		Sleep(125);
		i++;


	}
	assign_variables(x, y, pacman, ex, ex2, ex3, ex4, ey, ey2, ey3, ey4, pts, target_x, target_y, old_x, old_y, con, con2, i, lives, frame, frame_old, frame1, frame2, frame3, frame4, k, l, num_dot, power_pallet, choice, enter, blinking);
	system("cls");
	load_the_map();					//loads the game map from txt file with the starting animation before the game starts
	printFigures();					//prints the score and lives on the map 

	gotoxy(21, 10);  cout << " ________________________";
	gotoxy(21, 11);  cout << "|                        |";
	gotoxy(21, 12);  cout << "|                        |";
	gotoxy(21, 13);  cout << "|                        |";
	gotoxy(21, 14);  cout << "|       GET READY!       |";
	gotoxy(21, 15);  cout << "|                        |";
	gotoxy(21, 16);  cout << "|                        |";
	gotoxy(21, 17);  cout << "|________________________|";

	Sleep(3000);
	gotoxy(21, 10);  cout << " ________________________";
	gotoxy(21, 11);  cout << "|                        |";
	gotoxy(21, 12);  cout << "|                        |";
	gotoxy(21, 13);  cout << "|                        |";
	gotoxy(21, 14);  cout << "|           GO!          |";
	gotoxy(21, 15);  cout << "|                        |";
	gotoxy(21, 16);  cout << "|                        |";
	gotoxy(21, 17);  cout << "|________________________|";
	Sleep(500);


	load_the_map();			//loads the map for when the game starts
	printFigures();

	start.start_the_game();		//starts  the game
	system("cls");
	main();			//takes you back to main menu when esc button is clicked

}
void check_for_inputs(int &con2, int &num_dot, int pts) {				//used in programming the movement in game
	if (GetAsyncKeyState(VK_UP)) { con2 = 1; }
	if (GetAsyncKeyState(VK_DOWN)) { con2 = 2; }
	if (GetAsyncKeyState(VK_LEFT)) { con2 = 3; }
	if (GetAsyncKeyState(VK_RIGHT)) { con2 = 4; }
	if (GetAsyncKeyState(0x43)) {			//cheat button when 'c' is pressed in game
		num_dot = 0;
	}

	if (GetAsyncKeyState(VK_ESCAPE))
	{
		system("cls");
		main();
	}
}


void won(int pts) {														//displays message when you win the round
	string High_Score_Data[100], High_Score_Data_temp[100];
	int j, i, c, l, highscorearr[100];
	gotoxy(20, 12);  cout << " __________________________ ";
	gotoxy(20, 13);  cout << "|                          |";
	gotoxy(20, 14);  cout << "| Congratulations You won! |";
	gotoxy(20, 15);  cout << "|__________________________|";

	Sleep(3000);
	system("cls");
	ifstream myfile("Highscore.txt");				//used in comparing your highscore with the previous ones
	if (myfile.is_open())
	{
		for (i = 0; myfile.good(); i++)
		{
			getline(myfile, High_Score_Data[i]);
		}
		myfile.close();
	}

	for (j = 0; j < i; j++)
	{
		High_Score_Data_temp[j] = High_Score_Data[j];
	}

	for (l = 0; l < i; l++) {
		for (c = 0; c < High_Score_Data[l].length(); c++) { if (isdigit(High_Score_Data[l][c])) break; }
		High_Score_Data[l] = High_Score_Data[l].substr(c, High_Score_Data[l].length() - c);
		highscorearr[l] = atoi(High_Score_Data[l].c_str());
	}
	l = 0;
	for (c = 0; c < i; c++) {
		if (pts > highscorearr[l]) {
			l++;
		}
	}
	if (l == i) {
		highscorefunc(pts);
	}
	else {
		main();
	}
}

void highscorefunc(int pts) {						//takes you to the highscore screen
	int choice = 1;
	int enter = 0;
	int i = 0;
	int l = 0;
	int ord[100];
	int highscore[100];
	int j;
	int temp;
	int temp2;
	vector<int> numbers;
	string High_Score_Data[100];
	string High_Score_Data_temp[100];
	string name;
	system("cls");
	CRolePlayingGame start;


	if (pts != 0) {					//displays message used in saving your highscore
		assign_variables_for_animation(x, y, pacman, ex, ex2, ex3, ex4, ey, ey2, ey3, ey4, pts, target_x, target_y, old_x, old_y, con, con2, i, lives, frame, frame_old, frame1, frame2, frame3, frame4, k, l, num_dot, power_pallet, choice, enter, blinking, wait, wait2, wait3, wait4);
		load_animation_map();
		system("cls");
		ShowMap();
		gotoxy(21, 10);  cout << " ________________________";
		gotoxy(21, 11);  cout << "|    ENTER YOUR NAME,    |";
		gotoxy(21, 12);  cout << "|      HIGH SCORER!      |";
		gotoxy(21, 13);  cout << "|                        |";
		gotoxy(21, 14);  cout << "|     Do you want to     |";
		gotoxy(21, 15);  cout << "|    insert your name    |";
		gotoxy(21, 16);  cout << "|                        |";
		gotoxy(21, 17);  cout << "|                        |";
		gotoxy(21, 18);  cout << "|          YES           |";
		gotoxy(21, 19);  cout << "|                        |";
		gotoxy(21, 20);  cout << "|        GO BACK         |";
		gotoxy(21, 21);  cout << "|________________________|";
		Sleep(100);
		while (1) {
			if (GetAsyncKeyState(VK_UP)) { choice = 1; enter = 0; }
			if (GetAsyncKeyState(VK_DOWN)) { choice = 2; enter = 0; }
			if (GetAsyncKeyState(VK_RETURN) && i > 3) {
				enter = 1;
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}
			if (i % 5 == 0) blinking = !blinking;
			start.For_mainmenue_animation(i);

			if (choice == 1) {

				if (blinking) {
					gotoxy(31, 18); 	cout << "{";
					gotoxy(35, 18); 	cout << "}";

				}
				else {
					gotoxy(31, 18); 	cout << " ";
					gotoxy(35, 18); 	cout << " ";
				}
				gotoxy(29, 20); 	cout << " ";
				gotoxy(37, 20); cout << " ";

			}

			else if (choice == 2) {
				gotoxy(31, 18); 	cout << " ";
				gotoxy(35, 18); 	cout << " ";

				if (blinking) {
					gotoxy(29, 20); 	cout << "{";
					gotoxy(37, 20); cout << "}";
				}
				else {
					gotoxy(29, 20); 	cout << " ";
					gotoxy(37, 20); cout << " ";
				}
			}
			if (choice == 1 && enter == 1) {

				gotoxy(19, 10);  cout << " ____________________________";
				gotoxy(19, 11);  cout << "|      ENTER YOUR NAME,      |";
				gotoxy(19, 12);  cout << "|        HIGH SCORER!        |";
				gotoxy(19, 13);  cout << "|                            |";
				gotoxy(19, 14);  cout << "|                            |";
				gotoxy(19, 15);  cout << "|                            |";
				gotoxy(19, 16);  cout << "| ------------------ {ENTER} |";
				gotoxy(19, 17);  cout << "|____________________________|";
				gotoxy(19, 18);  cout << "                              ";
				gotoxy(19, 19);  cout << "                              ";
				gotoxy(19, 20);  cout << "                              ";
				gotoxy(19, 21);  cout << "                              ";

				Sleep(125);
				start.For_mainmenue_animation(i);

				gotoxy(21, 15);
				getline(cin, name);

				fstream hs;
				hs.open("Highscore.txt", fstream::in | fstream::out | fstream::app);
				hs.width(20);
				hs.fill('.');
				hs << left << name;
				hs.width(4);
				hs.fill('.');
				hs << right << pts << endl;
				hs.close();

				ifstream myfile("Highscore.txt");
				if (myfile.is_open())
				{
					for (i = 0; myfile.good(); i++)
					{
						getline(myfile, High_Score_Data[i]);
					}
					myfile.close();
				}
				int c;
				for (int l = 0; l < i; l++) {
					for (c = 0; c < High_Score_Data[l].length(); c++) { if (isdigit(High_Score_Data[l][c])) break; }
					High_Score_Data[l] = High_Score_Data[l].substr(c, High_Score_Data[l].length() - c);
					highscore[l] = atoi(High_Score_Data[l].c_str());
				}

				system("cls");
				pts = 0;
				break;
			}
			else if (choice == 2 && enter == 1) {
				Sleep(100);
				system("cls");
				main();
			}
			Sleep(125);
			i++;


		}

	}
	if (pts == 0) {
		assign_variables_for_animation(x, y, pacman, ex, ex2, ex3, ex4, ey, ey2, ey3, ey4, pts, target_x, target_y, old_x, old_y, con, con2, i, lives, frame, frame_old, frame1, frame2, frame3, frame4, k, l, num_dot, power_pallet, choice, enter, blinking, wait, wait2, wait3, wait4);
		load_animation_map();
		system("cls");
		ShowMap();
		gotoxy(18, 10);  cout << " ______________________________";
		gotoxy(18, 11);  cout << "| The pacman's high score list |";
		gotoxy(18, 12);  cout << "|     NAME               score |";
		gotoxy(18, 13);  cout << "|                              |";
		gotoxy(18, 14);



		ifstream myfile("Highscore.txt");
		if (myfile.is_open())
		{
			for (i = 0; myfile.good(); i++)
			{
				getline(myfile, High_Score_Data[i]);
			}
			myfile.close();
		}

		for (j = 0; j < i; j++)
		{
			High_Score_Data_temp[j] = High_Score_Data[j];
		}

		int c;
		for (l = 0; l < i; l++) {
			for (c = 0; c < High_Score_Data[l].length(); c++) { if (isdigit(High_Score_Data[l][c])) break; }
			High_Score_Data[l] = High_Score_Data[l].substr(c, High_Score_Data[l].length() - c);
			highscore[l] = atoi(High_Score_Data[l].c_str());
		}
		l = 0;
		for (c = 0; c < i; c++) {
			ord[c] = l;
			l++;
		}

		for (l = 0; l < i; l++)
		{
			for (j = l + 1; j < i; j++)
			{
				if (highscore[l] > highscore[j])
				{
					temp = highscore[l];
					highscore[l] = highscore[j];
					highscore[j] = temp;

					temp = ord[l];
					ord[l] = ord[j];
					ord[j] = temp;
				}
			}
		}

		i--;
		j = 1;
		for (i; i > 0; i--)
		{
			gotoxy(18, j + 13);
			cout << "|  " << j << ".";
			gotoxy(24, j + 13);
			cout << High_Score_Data_temp[ord[i]] << " |";
			j++;
		}
		enter = 0;
		choice = 1;
		c = 0;
		j += 14;
		gotoxy(18, j - 1);	 cout << "|                              |";
		gotoxy(18, j);	 cout << "|  GO BACK      CLEAR THE LIST |";
		gotoxy(18, j + 1);	 cout << "|______________________________|";


		while (1) {
			if (GetAsyncKeyState(VK_LEFT)) {
				gotoxy(33, j); 	cout << " ";
				gotoxy(48, j); 	cout << " ";
				choice = 1; enter = 0;
			}
			if (GetAsyncKeyState(VK_RIGHT)) {
				gotoxy(20, j); 	cout << " ";
				gotoxy(28, j); 	cout << " ";
				choice = 2; enter = 0;
			}
			if (GetAsyncKeyState(VK_RETURN) && c > 3) {
				enter = 1;
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}
			start.For_mainmenue_animation(c);
			if (c % 5 == 0) blinking = !blinking;
			if (choice == 1) {
				if (blinking) {
					gotoxy(20, j); 	cout << "{";
					gotoxy(28, j); 	cout << "}";

				}
				else {
					gotoxy(20, j); 	cout << " ";
					gotoxy(28, j); 	cout << " ";
				}
			}
			else if (choice == 2) {
				if (blinking) {
					gotoxy(33, j); 	cout << "{";
					gotoxy(48, j); 	cout << "}";

				}
				else {
					gotoxy(33, j); 	cout << " ";
					gotoxy(48, j); 	cout << " ";
				}
			}

			Sleep(125);
			c++;

			if (enter == 1 && choice == 1) break;
			if (enter == 1 && choice == 2) {
				ofstream ofs;
				ofs.open("Highscore.txt", ofstream::out | ofstream::trunc);
				ofs.close();
				highscorefunc(pts);
			}
		}


	}
	system("cls");
	main();
}
void load_the_map() {					//function used in loading the game map from txt file and saving it into array
	int i = 0;
	string line[30];
	ifstream myfile("map.txt");

	if (myfile.is_open())

	{
		while (myfile.good())

		{
			getline(myfile, line[i]);
			strcpy_s(map[i], line[i].c_str());
			i++;
		}

		myfile.close();

	}
}

void load_animation_map() {			//function used in loading the game map from txt file and saving it into array
	int i = 0;
	string line[30];
	ifstream myfile("animation_map.txt");

	if (myfile.is_open())

	{
		while (myfile.good())

		{
			getline(myfile, line[i]);
			strcpy_s(map[i], line[i].c_str());
			i++;
		}

		myfile.close();

	}
}

vector<int> read_numbers(string file_name)				//used in reading highscore from textfile 	
{
	ifstream infile;
	infile.open(file_name);
	vector<int> numbers;

	if (infile.is_open())
	{
		int num;

		for (int i = 0; i << 6; i++)
		{
			if (infile >> num)
			{
				infile >> num;
				numbers.push_back(num);
			}
		}
	}

	return numbers;
}


void ShowMap()						//prints the map from text file
{
	for (int i = 0; i < 30; i++) {
		printf("%s\n", map[i]);
	}
}

void gotoxy(short x, short y)			//used in moving the cursor across the map to print
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { x, y };

	SetConsoleCursorPosition(hStdout, position);
}

void AddArray(int x, int y, int wc, int back) {						//used in finding the shortest distance for villains
	if (tmp_map[y][x] == ' ' || tmp_map[y][x] == '.' || tmp_map[y][x] == '0' || tmp_map[y][x] == '_') {
		tmp_map[y][x] = '#';
		if (tmp_map[y][x + 1] == ' ' || tmp_map[y][x + 1] == '.') {
			tmp_map[y][x + 1] = '#';
		}
		walk tmp;
		tmp.x = x;
		tmp.y = y;
		tmp.walk_count = wc;
		tmp.back = back;
		BFSArray.push_back(tmp);
	}
}
void FindPath(int sx, int sy, int x, int y) {			//used in calculating shortest distance to pacman


	memcpy(tmp_map, map, sizeof(map));
	BFSArray.clear();
	walk tmp;
	tmp.x = sx;
	tmp.y = sy;
	tmp.walk_count = 0;
	tmp.back = -1;
	BFSArray.push_back(tmp);

	int i = 0;
	while (i < BFSArray.size()) {
		if ((BFSArray[i].x == x && BFSArray[i].y == y) || (BFSArray[i].x == x + 1 && BFSArray[i].y == y) || (BFSArray[i].x + 1 == x && BFSArray[i].y == y)) {
			walk_queue.clear();
			target tmp2;
			while (BFSArray[i].walk_count != 0) {
				tmp2.x = BFSArray[i].x;
				tmp2.y = BFSArray[i].y;
				walk_queue.push_back(tmp2);

				i = BFSArray[i].back;
			}

			break;
		}
		if ((map[BFSArray[i].y][BFSArray[i].x + 2] == ' ' || map[BFSArray[i].y][BFSArray[i].x + 2] == '.') && (map[BFSArray[i].y][BFSArray[i].x + 3] == ' ' || map[BFSArray[i].y][BFSArray[i].x + 3] == '.')) {
			AddArray(BFSArray[i].x + 2, BFSArray[i].y, BFSArray[i].walk_count + 1, i);
		}
		else 		AddArray(BFSArray[i].x + 1, BFSArray[i].y, BFSArray[i].walk_count + 1, i);
		if ((map[BFSArray[i].y][BFSArray[i].x - 2] == ' ' || map[BFSArray[i].y][BFSArray[i].x - 2] == '.') && (map[BFSArray[i].y][BFSArray[i].x - 1] == ' ' || map[BFSArray[i].y][BFSArray[i].x - 1] == '.')) {
			AddArray(BFSArray[i].x - 2, BFSArray[i].y, BFSArray[i].walk_count + 1, i);
		}
		else 			AddArray(BFSArray[i].x - 1, BFSArray[i].y, BFSArray[i].walk_count + 1, i);

		AddArray(BFSArray[i].x, BFSArray[i].y + 1, BFSArray[i].walk_count + 1, i);
		AddArray(BFSArray[i].x, BFSArray[i].y - 1, BFSArray[i].walk_count + 1, i);

		i++;

	}

	BFSArray.clear();
}
void printFigures() {					//prints pacman and the villains in game
	pacman = ":)";

	system("cls");
	ShowMap();

	gotoxy(ex, ey); cout << "D:";
	gotoxy(ex2, ey2); cout << ":c";
	gotoxy(ex3, ey3); cout << ">:";
	gotoxy(ex4, ey4); cout << ":{";
	gotoxy(x, y); cout << pacman;
	gotoxy(44, 25); cout << "Press 'Esc' to exit";
	gotoxy(10, 25); cout << pts;
	gotoxy(32, 25); cout << ":) :) ";
}




