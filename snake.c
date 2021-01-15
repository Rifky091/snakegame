#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys\timeb.h>
#include <Windows.h>
#include <pthread.h>
// Kecepatan gerakan snake dalam m/s (mili second)
int snake_speed = 200;
int snake2_speed= 200;
// Arah kepala saat awal permainan
int dir = VK_RIGHT;
char dir2 = 0x44;
// Panjang console
int console_width = 80;
// Tinggi console
int console_height = 25;
// Posisi makanan
int food_x, food_y;
int food_a, food_b;
//variabel thread
pthread_t player[2];
	// Untuk menyimpan nilai
int score = 0;
int score2= 0;

/*
    Struktur untuk menampung data tiap segment dari snake  
    (array)
*/
struct Segment{
	int x, y;
	int a, b;
};
// Array untuk menampung data ular
struct Segment snake[2000];
struct Segment snake2[2000];
// Variabel untuk menyimpan panjang ular (array snake)
int length = 0;
int length2 = 0;
/* Fungsi-fungsi **********/

//Untuk menampilkan halaman awal game
void Print(){
	printf("\n\tWelcome to Multiplayer Snake game.\n\n");
	system("pause");
	system("cls");
	printf("\tGame instructions:\n");
	printf("\n-> P1 Gunakan WASD untuk menggerakan ular.\n\n-> P2 Gunakan tanda panah untuk menggerakan badan ular.\n");
	printf("\nAturan Bermain : \n");
	printf("\n1. Score akan bertambah sejumlah 100 jika memakan 1 makanan.\n");
	printf("\n2. Jika ular menabrak boarder, maka akan Game Over.\n");
	printf("\n3. Player tidak akan mati jika menabrak sesama player.\n");
	printf("\n4. Player dengan skor tertinggi akan menjadi pemenang.\n\n");
	system("pause");
}

/*
    Push segment ke snake (pada bagian head).
    Untuk menambah panjang ular saat ular maju sesuai dg arahnya
 */
void push(int x, int y){
	for (int i = length; i > 0; i--){
		snake[i] = snake[i - 1];
	}
	snake[0].x = x;
	snake[0].y = y;
	length++;
}
void push2(int a, int b){
	for (int j = length2; j > 0; j--){
		snake2[j] = snake2[j - 1];
	}
	snake2[0].a = a;
	snake2[0].b = b;
	length2++;
}
/* 
    Pop bagian ekor snake. jadi jika ular maju maka ekor ular mengurang 
 */
void pop(){
	length--;
}

void pop2(){
	length2--;
}


/** Konfigurasi permainan **********/
// Panjang segment snake saat awal permainan
int snake_size = 3;
int snake_size2 = 3;
/** 
    Pindahkan posisi kursor di layar 
    Fungsi ini spesifik untuk OS windows. 
*/
void gotoxy(int x, int y)
{
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
void gotoab(int x, int y){
	COORD pos2;
	pos2.X = x;
	pos2.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos2);
}
/* 
    Tampilan di layar 
 */
void Boarder(){
	int i;
	for (i = 0; i < console_width; i++){
		gotoxy(i, 0);
		gotoab(i, 0);
		printf("!");
		gotoxy(i, console_height);
		gotoab(i, console_height);
		printf("!");
	}
	for (i = 0; i < console_height; i++){
		gotoxy(0, i);
		gotoab(0, i);
		printf("!");
		gotoxy(console_width, i);
		gotoab(console_width, i);
		printf("!");
	}
}
void display(){
	int i;
	for (int i = 0; i < length; i++){
        // Cetak di posisi x,y
		gotoxy(snake[i].x, snake[i].y);
		printf("O");
	}
	gotoxy(food_x, food_y);
	printf("F");
}
void display2(){
	int i;
	for (int j = 0; j < length2; j++){
        // Cetak di posisi x,y
		gotoab(snake2[j].a, snake2[j].b);
		printf("X");
	}
	gotoab(food_a, food_b);
	printf("P");
}
/*
    Memeriksa apakah terdapat salah satu segment 
    snake (array) di koordinat x,y. 
    Return 0 artinya tidak bertumpuk, 1 artinya bertumpuk. 
 */
int check_collision(int x, int y){
	for (int i = 0; i < length; i++){
		if (snake[i].x == x && snake[i].y == y){
			return 1;
		}
	}

	return 0;
}

int check_collision2(int a, int b){
	for (int j = 0; j < length2; j++){
		if (snake2[j].a == a && snake2[j].b == b){
			return 1;
		}
	}
	return 0;
}

/*
    Taruh makanan secara acak, namun memastikan  
    makanan tidak bertumpuk dengan salah satu segment  
    snake (array) 
 */
void place_food(){
    // Jika makanan bertumpuk dengan salah satu segment
    // snake, ulangi penempatan makanan secara acak.
	do{
		food_x = rand() % console_width;
		food_y = rand() % console_height;
	}
	while (check_collision(food_x, food_y) == 1);
}
void place_food2(){
    // Jika makanan bertumpuk dengan salah satu segment
    // snake, ulangi penempatan makanan secara acak.
	do{
		food_a = rand() % console_width;
		food_b = rand() % console_height;
	}
	while (check_collision2(food_a, food_b) == 1);
}


/*
    Program utama 
 */
void player1(){
	srand(time(NULL));
    // Untuk menyimpan penanda waktu saat snake bergerak
	struct timeb last_timestamp;
    ftime(&last_timestamp); // Set nilai awal
	// Pertama-tama, push segment (node) ke kanan
    // sebanyak 3 segment (sesuai nilai variable snake_size)

    for (int i = 0; i < snake_size; i++){
    	push(i, 0);
    }
	   // Tempatkan makanan secara acak
    place_food();
	    // Game loop. Bagian di dalam while akan dieksekusi terus menerus
    while (time){
        // Ambil penanda waktu saat ini
    	struct timeb current_timestamp;
    	ftime(&current_timestamp);
    	 // Selisih waktu terakhir dengan waktu sekarang dalam ms
    	int interval = 1000 * (current_timestamp.time - last_timestamp.time) + (current_timestamp.millitm - last_timestamp.millitm);
    	Boarder();

        // Snake bergerak setiap 200 ms (sesuai nilai variable snake_speed)
        // Dihitung dengan membandingkan selisih waktu sekarang dengan waktu
        // terakhir kali snake bergerak.
    	if (interval >= snake_speed){
            // Tentukan posisi x,y ke mana snake akan bergerak.
            // Posisi dilihat dari koordinat segment kepala (head)
            // dan arah (variable dir)

    		int x, y;
    		switch (dir){
    			case VK_LEFT:
    			x = snake[0].x - 1;
    			y = snake[0].y;
    			break;
    			case VK_RIGHT:
    			x = snake[0].x + 1;
    			y = snake[0].y;
    			break;
    			case VK_UP:
    			x = snake[0].x;
    			y = snake[0].y - 1;
    			break;
    			case VK_DOWN:
    			x = snake[0].x;
    			y = snake[0].y + 1;
    			break;
    		}

            // Jika posisi kepala (head) menabrak tembok pembatas,
            // maka permainan berakhir (keluar dari game loop)
    		if (x < 0 || x >= console_width || y < 0 || y >= console_height){
    			break;
    		}

            // Jika tidak terjadi tabrakan (collision), maka snake bergerak maju..
            // Pop ekor, lalu push segment ke depan head sehingga snake tampak bergerak maju.
            // Namun jika posisi x,y ke mana kepala (head) snake akan bergerak berada di posisi makanan, tidak perlu pop sehingga segment bertambah panjang.

    		if (x == food_x && y == food_y){
                // Dalam hal snake memakan makanan, maka nilai bertambah
    			score += 100;

                // Lalu makanan ditempatkan ulang secara acak
    			place_food();
    		}
    		else{
    			pop();
    		}
    		push(x, y);


            // Bersihkan layar
    		system("cls");

            // Cetak (render) snake dan makanan di layar
    		display();

            // Perbarui penanda waktu
    		last_timestamp = current_timestamp;
    	}
		// Ubah arah jika tombol panah ditekan
    	if (GetKeyState(VK_LEFT) < 0){
    		dir = VK_LEFT;
    	}
    	if (GetKeyState(VK_RIGHT) < 0){
    		dir = VK_RIGHT;
    	}
    	if (GetKeyState(VK_UP) < 0){
    		dir = VK_UP;
    	}
    	if (GetKeyState(VK_DOWN) < 0){
    		dir = VK_DOWN;
    	}
		 // Keluar dari program jika menekan tombol ESC
    	if (GetKeyState(VK_ESCAPE) < 0){
    		break;
    	}

        // Tampilkan kondisi permainan saat ini di layar...

        // Bersihkan layar
    	system("cls");

        // Cetak (render) snake di layar
    	display(); 
    }
}
void player2(){
	srand(time(NULL));
    // Untuk menyimpan penanda waktu saat snake bergerak
	struct timeb last_timestamp;
    ftime(&last_timestamp); // Set nilai awal

    // Pertama-tama, push segment (node) ke kanan
    // sebanyak 3 segment (sesuai nilai variable snake_size)

    for (int j = 0; j < snake_size2; j++){
    	push2(j, 0);
    }
    // Tempatkan makanan secara acak
    place_food2();
	    // Game loop. Bagian di dalam while akan dieksekusi terus menerus
    while (time){
        // Ambil penanda waktu saat ini
    	struct timeb current_timestamp;
    	ftime(&current_timestamp);
	 // Selisih waktu terakhir dengan waktu sekarang dalam ms
    	int interval2 = 1000 * (current_timestamp.time - last_timestamp.time) + (current_timestamp.millitm - last_timestamp.millitm);
    	Boarder();

        // Snake bergerak setiap 200 ms (sesuai nilai variable snake_speed)
        // Dihitung dengan membandingkan selisih waktu sekarang dengan waktu
        // terakhir kali snake bergerak.
    	if (interval2 >= snake2_speed){
            // Tentukan posisi x,y ke mana snake akan bergerak.
            // Posisi dilihat dari koordinat segment kepala (head)
            // dan arah (variable dir)

    		int a, b;
    		switch (dir2){
    			case 0x41:
    			a = snake2[0].a - 1;
    			b = snake2[0].b;
    			break;
    			case 0x44:
    			a = snake2[0].a + 1;
    			b = snake2[0].b;
    			break;
    			case 0x57:
    			a = snake2[0].a;
    			b = snake2[0].b - 1;
    			break;
    			case 0x53:
    			a = snake2[0].a;
    			b = snake2[0].b + 1;
    			break;
    		}

            // Jika posisi kepala (head) menabrak tembok pembatas,
            // maka permainan berakhir (keluar dari game loop)

    		if (a < 0 || a >= console_width || b < 0 || b >= console_height){
    			break;
    		}
            // Jika tidak terjadi tabrakan (collision), maka snake bergerak maju..
            // Pop ekor, lalu push segment ke depan head sehingga snake tampak bergerak maju.
            // Namun jika posisi x,y ke mana kepala (head) snake akan bergerak berada di posisi makanan, tidak perlu pop sehingga segment bertambah panjang.
    		if (a == food_a && b == food_b){
                // Dalam hal snake memakan makanan, maka nilai bertambah
    			score2 += 100;

                // Lalu makanan ditempatkan ulang secara acak
    			place_food2();
    		}
    		else{
    			pop2();
    		}
    		push2(a, b);

            // Bersihkan layar
    		system("cls");

            // Cetak (render) snake dan makanan di layar
    		display2();

            // Perbarui penanda waktu
    		last_timestamp = current_timestamp;
    	}


//=============================================
    	if (GetKeyState(0x41) < 0){
    		dir2 = 0x41;
    	}
    	if (GetKeyState(0x44) < 0){
    		dir2 = 0x44;
    	}
    	if (GetKeyState(0x57) < 0){
    		dir2 = 0x57;
    	}
    	if (GetKeyState(0x53) < 0){
    		dir2 = 0x53;
    	}
        // Keluar dari program jika menekan tombol ESC
    	if (GetKeyState(VK_ESCAPE) < 0){
    		break;
    	}

        // Tampilkan kondisi permainan saat ini di layar...

        // Bersihkan layar
    	system("cls");

        // Cetak (render) snake di layar 
    	display2();  
    }
}
int main(){
	Print();
	pthread_t player[2];//declare variabel untuk thread
    // Randomize
    //mmembuat 2 thread untuk kedua player
	pthread_create(&player[0],NULL,player1,NULL);
	pthread_create(&player[1],NULL,player2,NULL);
	
	for (int i = 0; i < 2; ++i)
	{
		//untuk menjalankan thread
		pthread_join(player[i],NULL);
	}
	
    // Setelah keluar dari game loop, berarti permainan berakhir (game over)
    // Tampilkan nilai yang diraih pemain
	system("cls");
	printf("\tGAME OVER\n");
	printf("Player1 score: %d\n\n", score);
	printf("Player2 score: %d\n\n", score2);
	printf("----------------------\n");
    //tampilkan nilai tertinggi sebagai pemenang
    if(score > score2) {
        printf("Player 1 Win!!!\n\n");
    }else if(score2 > score){
        printf("Player 2 Win!!!\n\n");
    }else{
        printf("Draw!!!\n\n");
    }
    printf("Press ENTER to exit...");
    getchar();
	return 0;
}