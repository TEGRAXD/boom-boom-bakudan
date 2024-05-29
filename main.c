#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// Ukuran map
#define SIZE 20

// Jika ingin merubah Bomb probability dan Wall probability
// Usahakan jika Bomb prob + Wall prob masih dibawah 100.
// 20 + 30 = 50. Kemungkinan 50% lainnya adalah Tanah biasa.
#define BOMB_PROBABILITY 20
#define WALL_PROBABILITY 30

// Struct bernama Tile
// seperti object class pada Java, dengan 2 attribute:
// terrain yaitu S, E, ?, (.), *, #.
// status yaitu v -> Visible dan i -> Invisible
typedef struct {
    char terrain;
    char status;
} Tile;

// Menampilkan peta
void showMap(Tile map[SIZE][SIZE], bool revealAll) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if ((map[i][j].terrain == '*' || (map[i][j].terrain == '.' && map[i][j].status == 'i')) && !revealAll) {
                printf("? ");
            } else {
                printf("%c ", map[i][j].terrain);
            }
        }
        printf("\n");
    }
}

// Fungsi untuk menghasilkan angka acak antara parameter min dan max
int getRandomNumber(int min, int max) {
    return rand() % (max - min + 1) + min;
}

// Meng-generate bomb dengan menginputkan map
void generateBomKlee(Tile map[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (getRandomNumber(1, 100) <= BOMB_PROBABILITY) {
                map[i][j].terrain = '*';
            }
        }
    }
}

// Meng-generate tembok dengan menginputkan map
void generateTembok(Tile map[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            // Jika terrain hanya tanah biasa
            // Maka ambil angka acak dari 1-100, jika angka tsb <= 30, ganti dengan tembok. jika angka tidak <= 30, biarkan sebagai tanah.
            if (map[i][j].terrain == '.' && getRandomNumber(1, 100) <= WALL_PROBABILITY) {
                map[i][j].terrain = '#';
            }
        }
    }
}

// Meng-generate posisi start dan end
void generateStartEnd(Tile map[SIZE][SIZE], int *startX, int *startY, int *endX, int *endY) {
    *startX = getRandomNumber(0, SIZE - 1);
    *startY = getRandomNumber(0, SIZE - 1);

    do {
        *endX = getRandomNumber(0, SIZE - 1);
        *endY = getRandomNumber(0, SIZE - 1);
    } while (*endX == *startX && *endY == *startY);

    map[*startX][*startY].terrain = 'S';
    map[*endX][*endY].terrain = 'E';
}

// Inisialisasi map
void initMap(Tile map[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            map[i][j].terrain = '.';
            map[i][j].status = 'i';
        }
    }
}

// Player movement mechanic
void movePlayer(Tile map[SIZE][SIZE], int *startX, int *startY, int *playerX, int *playerY, int *playerPrevX, int *playerPrevY, char move) {
    switch (move) {
        case 'w':
        case 'W':
            // Mengatur arah W agar tidak bisa keluar dari map dan menembus tembok
            if (*playerX > 0 && map[*playerX - 1][*playerY].terrain != '#') {
                (*playerX)--;
            }
            break;
        case 'a':
        case 'A':
            // Mengatur arah A agar tidak bisa keluar dari map dan menembus tembok
            if (*playerY > 0 && map[*playerX][*playerY - 1].terrain != '#') {
                (*playerY)--;
            }
            break;
        case 's':
        case 'S':
            // Mengatur arah S agar tidak bisa keluar dari map dan menembus tembok
            if (*playerX < SIZE - 1 && map[*playerX + 1][*playerY].terrain != '#') {
                (*playerX)++;
            }
            break;
        case 'd':
        case 'D':
            // Mengatur arah D agar tidak bisa keluar dari map dan menembus tembok
            if (*playerY < SIZE - 1 && map[*playerX][*playerY + 1].terrain != '#') {
                (*playerY)++;
            }
            break;
    }
}

// Fungsi untuk mengecek apakah player menyentuh End (E)
bool checkEnd(int playerX, int playerY, int endX, int endY) {
    return playerX == endX && playerY == endY;
}

// Fungsi untuk mengecek apakah player menyentuh Bom dari Klee (*)
bool checkBomb(Tile map[SIZE][SIZE], int playerX, int playerY) {
    return map[playerX][playerY].terrain == '*';
}

int main() {
    // Loop yang ini digunakan untuk me-replay permainan
    // Since the game needs you too replaying the game after Klee's bomb blasting your face
    while (1) {
        Tile map[SIZE][SIZE];
        int startX, startY, endX, endY;
        int playerX, playerY, playerPrevX, playerPrevY;
    
        // SRand merupakan Seed Randomizer (Fungsi Pseudorandom dengan seed, bila seednya berbeda maka hasil random berbeda juga)
        // dengan seed "time" maka setiap kali compile akan menghasilkan random yang berbeda. Since time always different wkwk.
        // Jika ingin real mencoba ubah srand(2) -> srand(time(NULL))
        // Gunakan srand(2) jika ingin mencoba-coba karena srand(1) tidak memiliki jalan menuju E
        srand(2);
        //srand(time(NULL));
        
        // Inisialisasi map (awal inisialisasi seluruh map hanya berisi tanah)
        initMap(map);
        
        // Menggenerate bom Klee
        // F.Y.I: "Boom Boom Bakudan" adalah kalimat yang sering diucapkan Klee (karakter) dalam Game Genshin Impact. (Yang bikin soal ini fix wibu akut)
        generateBomKlee(map);
        
        // Menggenerate tembok
        generateTembok(map);
        
        // Menggenerate letak legend 'S' dan 'E'
        generateStartEnd(map, &startX, &startY, &endX, &endY);
        
        // Posisi inisial player berada pada startX dan startY
        playerX = startX;
        playerY = startY;
        
        // Loop yang ini digunakan untuk menginput terus menerus hingga modar/win
        while (1) {
            // Ganti "cls" -> "clear" jika menggunakan Linux
            system("cls"); 
            showMap(map, false);
            
            // Check end dan menang
            if (checkEnd(playerX, playerY, endX, endY)) {
                system("cls");
                showMap(map, true);
                printf("Selamat, Kamu menang!\n\n");
                break;
            }
            
            // Check bomb dan kalah
            if (checkBomb(map, playerX, playerY)) {
                system("cls");
                showMap(map, true);
                printf("Boom-Boom! Bakudan! Kamu kalah!\n\n");
                break;
            }
            
            
            // Input movement
            printf("Move (WASD): ");
            char move;
            scanf(" %c", &move);
            
            // Sebelum player bergerak, menyimpan gerakan sebelumnya.
            playerPrevX = playerX;
            playerPrevY = playerY;
            
            // Menggerakkan player
            movePlayer(map, &startX, &startY, &playerX, &playerY, &playerPrevX, &playerPrevY, move);
            
            
            // Menggerakkan legend 'P'
            if (map[playerX][playerY].terrain == '.') {
                map[playerX][playerY].terrain = 'P';
                map[playerX][playerY].status = 'v';
            }
            
            // Mengganti menjadi tanah biasa (.) apabila telah dilewati oleh player
            if ((playerX != playerPrevX || playerY != playerPrevY) && map[playerPrevX][playerPrevY].status == 'v') {
                map[playerPrevX][playerPrevY].terrain = '.';
                map[playerPrevX][playerPrevY].status = 'v';
            }
        }
        
        char reset;
        printf("Apakah ingin mengulangi game? (Y/N) : ");
        scanf(" %c", &reset);
        
        if (reset == 'Y' || reset == 'y') {
            continue;
        } else {
            break;
        }
    }
    
    // Pause window cmd agar tidak langsung tertutup
    system("pause");

    return 0;
}
