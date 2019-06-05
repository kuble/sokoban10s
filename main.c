#include <stdio.h>
#include <termio.h>
#include <windows.h>

char name[10]; // 사용자의 이름울 저장하는 배열
char map[5][30][30]; // 맵 전체를 저장하는 배열
char cMap[30][30]; // 현재 맵 번호(cIndex)에 해당하는 맵만을 저장하는 배열
char uMap[5][30][30]; //undo 사용전 맵을 저장하는 배열 (김윤태)

int cIndex = 0; // 현재 맵 인덱스를 저장하는 변수
int plX, plY; // 플레이어('@')의 X축, Y축 위치를 저장하는 변수
int upl[5][2]; //undo를 사용한 후의 '@' 의 위치를 저장하기 위한 배열 (김윤태)
int mvCnt = 0; // 움직인 횟수

void createMapArray(); // map.txt 파일을 읽어 map[5][30][30] 배열에 저장하는 함수
int checkMap(); // 각 맵마다 상자와 창고의 갯수가 같은지를 확인하는 함수
void createCurrentMap(); // 현재 맵 번호(cIndex)에 해당하는 맵을 cMap[30][30]에 저장하는 함수
void printMap(); // 현재 맵(cMap[30][30])을 출력하는 함수
int getch(); // enter 키를 입력할 필요 없이 바로 반응하도록 하는 함수
void command(); // 사용자로부터 키를 입력 받아 해당 명령을 실행하는 함수
void recordundo(); //  undo 의 횟수와 undo용으로 쓰일 맵 그리고 플레이어 위치를 저장하는 함수 (김윤태)
void undo(); //플레이어 이동을 한 칸 되돌리는 함수 (김윤태)
void resetundo(); // recordundo() 함수로 저장된 값들을 모두 초기화하는 함수 (김윤태)
void moveCommand(char); // @를 이동시키기 위한 함수

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
    return 0;
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

void recordundo(){
	for (int i=4;i>=1;i--){ //저장되어있던 undo용 맵 umap배열의 배열을 한 칸씩 뒤로 빼준다 ( 예 : umap[1][0][0]를 umap[2][0][0]로 ) (김윤태) 
		for(int j=0;j<30;j++){
			for(int k=0;k<30;k++)
				uMap[i][j][k]=uMap[i-1][j][k];}
		upl[i][0]=upl[i-1][0], upl[i][1]=upl[i-1][1];} //undo 후의 '@' 위치를 표시하는 upl 배열을 위의 uMap과 같이 한 칸씩 뒤로 빼준다 ( 예 : upl[1][0]를 upl[2][0]로 ) (김윤태)
	for (int i=0;i<30;i++){  // uMap의 0번째 배열에 현재맵 cMap배열을 넣어준다. (김윤태)
		for (int j=0;j<30;j++){
			uMap[0][i][j]=cMap[i][j];}}
	upl[0][0]=plX, upl[0][1]=plY;} //upl[0] 배열에 현재 '@' 의 좌표를 넣어준다. (김윤태)

void undo(){//움직이기 전 위치로 되돌리기. (김윤태)
	for (int i=0;i<30;i++){
		for (int j=0;j<30;j++){
			cMap[i][j]=uMap[0][i][j];}} //현재 맵배열에 undo용으로 저장된 uMap배열의 0번째 배열을 가져온다. (김윤태)
	plX=upl[0][0],plY=upl[0][1]; //현재 플레이어 '@' 의 X , Y 좌표에 undo후 위치용으로 저장된 배열의 0번째 배열을 가져온다. (김윤태) 
	for (int i=0;i<=4;i++){
		for(int j=0;j<30;j++){
			for(int k=0;k<30;k++)
				uMap[i][j][k]=uMap[i+1][j][k];} //undo하기 위해 저장된 맵배열 uMap을 한 칸씩 앞으로 가져온다 ( 예 : uMap[1][0][0]를 uMap[0][0][0]로 ) (김윤태)
		upl[i][0]=upl[i+1][0], upl[i][1]=upl[i+1][1];} //undo후의 '@' 의 위치를 표시하는 upl 배열을 위 uMap과 같이 한 칸씩 앞으로 가져온다( 예 : upl[1][0]를 upl[0][0]로 ) (김윤태)
	printMap();} //undo후의 맵을 출력한다. (김윤태)

void resetundo(){ //undo 횟수 초기화함수. (김윤태)
	for (int i=0;i<5;i++){
		for(int j=0;j<30;j++){
			for (int k=0;k<30;k++)
				uMap[i][j][k]='\0';} //uMap배열의 모든 값들을 null 값으로 초기화. (김윤태)
		upl[i][0]='\0',upl[i][1]='\0';}} //undo후 플레이어 위치인 upl 배열 역시  모두 null 값으로 초기화. (김윤태)

void manual() { // 'd'를 눌렀을 때 명령 내용 출력을 위한 함수
	printf("\nh(왼쪽), j(아래), k(위), l(오른쪽)\n");
	printf("u(undo)\n");
	printf("r(replay)\n");
	printf("n(new)\n");
	printf("e(exit)\n");
	printf("s(save)\n");
	printf("f(file load)\n");
	printf("d(display help)\n");
	printf("t(top)\n");
}

void moveCommand(char ch) { // (이셩현)
	recordundo();
	int dX = 0, dY = 0;
	switch (ch) {
	case 'h': // 왼쪽으로 이동하므로 X축이 +0 Y축이 -1
		dX = 0;
		dY = -1;
		break;
	case 'j': // 아래쪽으로 이동하므로 X축이 +1 Y축이 +0
		dX = 1;
		dY = 0;
		break;
	case 'k': // 위쪽으로 이동하므로 X축이 -1 Y축이 +0
		dX = -1;
		dY = 0;
		break;
	case 'l': // 오른쪽으로 이동하므로 X축이 +0 Y축이 +1
		dX = 0;
		dY = 1;
		break;
	}
	int X = plX + dX, Y = plY + dY, aX = X + dX, aY = Y + dY;
	if (cMap[X][Y] == '.' || cMap[X][Y] == 'O') { // 이동하려는 칸이 '.'일 경우
		cMap[X][Y] = '@', cMap[plX][plY] = '.';
	}
	else if (cMap[X][Y] == '#') { // 이동하려는 칸이 '#'일 경우
		dX = 0, dY = 0;
	}
	else if (cMap[X][Y] == '$') { // 이동하려는 칸이 '$'일 경우 '$'의 옆칸을 확인
		if (cMap[aX][aY] == '.' || cMap[aX][aY] == 'O') { // '$'의 옆칸이 '.'이나 'O'일 경우
			cMap[aX][aY] = '$', cMap[X][Y] = '@', cMap[plX][plY] = '.';
		}
		else if (cMap[aX][aY] == '#' || cMap[aX][aY] == '$') { // '$'의 옆칸이 '#'이나 '$'일 경우
			dX = 0, dY = 0;
		}
	}
	plX += dX, plY += dY, mvCnt++;
	system("clear");
	printMap();
	return;
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
			if (uMap[0][0][0]=='\0'){// uMap배열의 첫번째 배열이 null 값이면 되돌리기가 불가능하다는 메세지 출력. (김윤태)
				printf("남은 되돌리기 횟수가0이 되어 더 이상 되돌리기가 불가능 합니다.");}
			else{// uMap 배열의 첫번째 배열이 null 값이 아니면 이동횟수를 1회 올리고 undo()함수 실행. (김윤태)
				system("clear");
				mvCnt++;
				undo();}
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
					system("clear");
					printMap();
					manual();
					break;
                default:
					if (ch == 'h' || ch == 'j' || ch == 'k' || ch == 'l') { // 이동키일경우 move_command() 함수 실행
						moveCommand(ch);
					}
					else { // 명령어 & 이동키가 아닐 경우 '잘못된 명령입니다.' 출력
						printf("잘못된 명령입니다.\n");
					}
                    break;
            }
        }
    }
    EXIT :
    return;
}
