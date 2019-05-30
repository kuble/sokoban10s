#include <stdio.h>
#include <termio.h>
#include <windows.h>

char name[10]; // 사용자의 이름울 저장하는 배열
char map[5][30][30]; // 맵 전체를 저장하는 배열
char cMap[30][30]; // 현재 맵 번호(cIndex)에 해당하는 맵만을 저장하는 배열

int cIndex = 0; // 현재 맵 인덱스를 저장하는 변수
int plX, plY; // 플레이어('@')의 X축, Y축 위치를 저장하는 변수

void createMapArray(); // map.txt 파일을 읽어 map[5][30][30] 배열에 저장하는 함수
int checkMap(); // 각 맵마다 상자와 창고의 갯수가 같은지를 확인하는 함수
void createCurrentMap(); // 현재 맵 번호(cIndex)에 해당하는 맵을 cMap[30][30]에 저장하는 함수
void printMap(); // 현재 맵(cMap[30][30])을 출력하는 함수
int getch(); // enter 키를 입력할 필요 없이 바로 반응하도록 하는 함수
void command(); // 사용자로부터 키를 입력 받아 해당 명령을 실행하는 함수

int main() {
    system("clear");
    printf("Start....\ninput name : ");
    scanf("%s", &name); // 사용자 이름을 입력받음
    createMapArray(); 
    if (checkMap() == 1) { // 1을 리턴받으면 그 즉시 프로그램을 종료함
        return 0;
    };
    createCurrentMap();
    system("clear");
    printf("Hello %s\n\n", name); // 인삿말
    printMap(); 
    command(); 
    return 0;
}

void createMapArray() { // made by 유상진
    // map.txt 파일을 읽어 map[5][30][30] 배열에 저장하는 함수
    char b[30];
    FILE *fp;
    fp = fopen("map.txt", "r");
    int l = 0, index = 0; 
    while (1) {
        fscanf(fp, "%s", &b); // map.txt 파일을 한 줄씩 읽어들여 b에 저장함
        if (b[0] == 'e') { // b 배열의 처음이 'e'면 map.txt 읽기를 종료함
            break;
        } else if (b[0] >= 49 && b[0] <= 53) { // b 배열의 처음이 1과 5사이 숫자라면 index를 해당 숫자로 바꾸고 줄 수를 0으로 초기화함
            index = b[0] - 49;
            l = 0;
        } else { // map.txt 파일로부터 읽은 b 배열을 map[5][30][30] 배열에 넣음
            for (int i = 0; i < 30; i++) {
                map[index][l][i] = b[i];
            }
            l += 1; // 줄 수 + 1
        }
    }
    fclose(fp);
}

int checkMap() { // made by 유상진
    // 각 맵마다 상자와 창고의 갯수가 같은지를 확인하는 함수
    for (int i = 0; i < 5; i++) {
        int b = 0, s = 0; 
        for (int j = 0; j < 30; j++) {
            for (int k = 0; k < 30; k++) { 
                if (map[i][j][k] == '$') { 
                    b++; // 박스 갯수 계산
                } else if (map[i][j][k] == 'O') {
                    s++; // 창고 갯수 계산
                }
            }
        }
        if (b != s) { // 박스 갯수와 창고 갯수가 다를 경우 1을 리턴함
            return 1; 
        }
    }
}

void createCurrentMap() { // made by 유상진
    // 현재 맵 번호(cIndex)에 해당하는 맵을 cMap[30][30]에 저장하는 함수
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 30; j++) {
            if (map[cIndex][i][j] == '@') { // 플레이어('@')의 위치를 plX, plY에 저장함
                plX = i, plY = j;
            }
            cMap[i][j] = map[cIndex][i][j]; // 현재 맵 번호(cIndex)에 해당하는 맵을 cMap[30][30] 배열에 넣음
        }
    }
}

void printMap() { 
    // 현재 맵(cMap[30][30])을 출력하는 함수
    for (int i = 0; i < 30; i++) {
        if (cMap[i][0] == '\0') { // 빈 줄은 출력하지 않음
            break;
        }
        for (int j = 0; j < 30; j++) {
            if (cMap[i][j] == '\0') { // 빈 칸은 출력하지 않음
                break;
            } 
            if (cMap[i][j] == '.') { // '.'을 ' '로 출력함
                printf(" ");
            } else {
                printf("%c", cMap[i][j]); // 맵을 출력
            }
        }
        printf("\n");
    }
}

int getch() { 
    // enter 키를 입력할 필요 없이 바로 반응하도록 하는 함수
    int ch;
    struct termios buf, save;
    tcgetattr(0,&save);
    buf = save;
    buf.c_lflag &= ~(ICANON|ECHO);
    buf.c_cc[VMIN] = 1;
    buf.c_cc[VTIME] = 0;
    tcsetattr(0, TCSAFLUSH, &buf);
    ch = getchar();
    tcsetattr(0, TCSAFLUSH, &save);
    return ch;
}

void command() { 
    // 사용자로부터 키를 입력 받아 해당 명령을 실행하는 함수
    char ch;
    ch = getchar();
    while (1) {
        ch = getch();
        if (ch == 't') { // 전체 맵 순위만 출력할지 특정한 맵 순위만 출력할건지 다시 입력받아야 하므로 if문을 통해 구분함
            
        } else {
            switch (ch) {
                case 'u':

                    break;
                case 'r': 

                    break;
                case 'n': 

                    break;
                case 'e': 
                    goto EXIT;
                case 's': 

                    break;
                case 'f': 

                    break;
                case 'd': 

                    break;
                default:
                    if (ch == 'h' || ch == 'j' || ch == 'k' || ch == 'l') { 
                        
                    } else { 
                        printf("잘못된 명령입니다.\n");
                    }
                    break;
            }
        }
    }
    EXIT :
    return;
}
