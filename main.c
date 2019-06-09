#include <stdio.h>
#include <termio.h>
#include <windows.h>

char name[10] = { 0, };								// 사용자의 이름울 저장하는 배열
char map[5][30][30] = { {0, }, {0, }, {0, } };					// 맵 전체를 저장하는 배열
char cMap[30][30] = { {0, }, {0, } };					      	// 현재 맵 번호(cIndex)에 해당하는 맵만을 저장하는 배열
char uMap[5][30][30] = { {0, }, {0, }, {0, } };;				// undo 사용전 맵을 저장하는 배열
char rName[10] = { 0, };                                     			// 랭킹을 출력할 때 사용하는 이름의 배열

int cIndex = 0;									// 현재 맵 인덱스를 저장하는 변수
int plX, plY;									// 플레이어('@')의 X축, Y축 위치를 저장하는 변수
int cStorage = 0, err = 0;							// 레벨 클리어를 판단하기 위한 변수
int mvCnt = 0;									// 움직인 횟수
int upl[5][2];									// undo를 사용한 후의 '@' 의 위치를 저장하기 위한 배열
int rIndex, rMvCnt;                 						// rangking 시스템에서 출력시 보이게될 변수

_Bool nextStage = 0;								// 스테이지 클리어를 판별하는 변수

void createMapArray();								// map.txt 파일을 읽어 map[5][30][30] 배열에 저장하는 함수
int checkMap();									// 각 맵마다 상자와 창고의 갯수가 같은지를 확인하는 함수
void createCurrentMap();							// 현재 맵 번호(cIndex)에 해당하는 맵을 cMap[30][30]에 저장하는 함수
void printMap();								// 현재 맵(cMap[30][30])을 출력하는 함수
int getch();						    			// enter 키를 입력할 필요 없이 바로 반응하도록 하는 함수
void recordundo();								// undo 의 횟수와 undo용으로 쓰일 맵 그리고 플레이어 위치를 저장하는 함수
void undo();						  			// 플레이어 이동을 한 칸 되돌리는 함수
void moveCommand(char);								// @를 이동시키기 위한 함수
void command();									// 사용자로부터 키를 입력 받아 해당 명령을 실행하는 함수
void resetundo();								// recordundo() 함수로 저장된 값들을 모두 초기화하는 함수
void createRangkingFile();          						// 랭킹파일이 없을시 생성하는 함수
void inputRangking();               						// 랭킹파일에 순위를 지정하여 rangking.txt파일에 저장하는 함수
void loadAllRangking();            	 					// 전체 랭킹을 불러오는 함수
void loadMapRangking(char);         						// 보고싶은 맵의 랭킹 하나만을 불러오는 함수
void manual();									// 'd'를 눌렀을 때 명령 내용 출력을 위한 함수
void saveMap();
void loadMap();

int main(void) {
	system("clear");
	printf("Start....\ninput name : "); 					// 사용자 이름을 입력받음
	scanf("%s", &name);
	createMapArray();
	if (checkMap() == 1) {              					// 1을 리턴받으면 그 즉시 프로그램을 종료함
		return 0;
	};
	createCurrentMap();
	createRangkingFile();
	system("clear");
	printf("Hello %s\n\n", name);       					// 인삿말
	printMap();
	command();
	return 0;
}

void createMapArray() { // (유상진)
	// map.txt 파일을 읽어 map[5][30][30] 배열에 저장하는 함수
	char b[30];
	FILE *fp;
	fp = fopen("map.txt", "r");
	int l = 0, index = 0;
	while (1) {
		fscanf(fp, "%s", &b); 						// map.txt 파일을 한 줄씩 읽어들여 b에 저장함
		if (b[0] == 'e') {    						// b 배열의 처음이 'e'면 map.txt 읽기를 종료함
			break;
		}
		else if (b[0] >= 49 && b[0] <= 53) { 				// b 배열의 처음이 1과 5사이 숫자라면 index를 해당 숫자로 바꾸고 줄 수를 0으로 초기화함
			index = b[0] - 49;
			l = 0;
		}
		else {    							// map.txt 파일로부터 읽은 b 배열을 map[5][30][30] 배열에 넣음
			for (int i = 0; i < 30; i++) {
				map[index][l][i] = b[i];
			}
			l += 1;							// 줄 수 + 1
		}
	}
	fclose(fp);
}

int checkMap() { // (유상진)
	// 각 맵마다 상자와 창고의 갯수가 같은지를 확인하는 함수
	for (int i = 0; i < 5; i++) {
		int b = 0, s = 0; 						// 박스 갯수, 창고 갯수를 세기 위한 변수
		for (int j = 0; j < 30; j++) {
			for (int k = 0; k < 30; k++) {
				if (map[i][j][k] == '$') {
					b++; 					// 박스 갯수 계산
				}
				else if (map[i][j][k] == 'O') {
					s++; 					// 창고 갯수 계산
				}
			}
		}
		if (b != s) { 							// 박스 갯수와 창고 갯수가 다를 경우 1을 리턴함
			printf("박스와 창고의 갯수가 같지 않습니다. 게임을 종료합니다.");
			return 1;
		}
	}
	return 0;
}

void createCurrentMap() { // (유상진)
	// 현재 맵 번호(cIndex)에 해당하는 맵을 cMap[30][30]에 저장하는 함수
	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < 30; j++) {
			if (map[cIndex][i][j] == '@') { 			// 플레이어('@')의 위치를 plX, plY에 저장함
				plX = i, plY = j;
			}
			cMap[i][j] = map[cIndex][i][j]; 			// 현재 맵 번호(cIndex)에 해당하는 맵을 cMap[30][30] 배열에 넣음
		}
	}
}

void printMap() { // (유상진)
	// 현재 맵(cMap[30][30])을 출력하는 함수
	cStorage = 0, err = 0;
	for (int i = 0; i < 30; i++) {
		if (cMap[i][0] == '\0') {        				// 빈 줄은 출력하지 않음
			break;
		}
		for (int j = 0; j < 30; j++) {
			if (cMap[i][j] == '\0') {      				// 빈 칸은 출력하지 않음
				break;
			}
			else if (map[cIndex][i][j] == 'O') { 			// '@'나 '$'가 아니라면 map 배열에서 'O'인 칸은 무조건 'O'로 출력
				if (cMap[i][j] != '$' && cMap[i][j] != '@') {
					cMap[i][j] = 'O';
				}
				if (cMap[i][j] == 'O') {
					cStorage++;
				}
				else if (cMap[i][j] == '@') {
					err++;
				}
			}
			if (cMap[i][j] == '.') {    				// '.'을 ' '로 출력함
				printf(" ");
			}
			else {
				printf("%c", cMap[i][j]); 			// 맵을 출력
			}
		}
		printf("\n");
	}
	printf("\nMove Count : %d회\n", mvCnt);
	if (cStorage == 0 && err == 0) {
		nextStage = 1;
	}
}

int getch() { // (실습 참조)
	// enter 키를 입력할 필요 없이 바로 반응하도록 하는 함수
	int ch;
	struct termios buf, save;
	tcgetattr(0, &save);
	buf = save;
	buf.c_lflag &= ~(ICANON | ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;
	tcsetattr(0, TCSAFLUSH, &buf);
	ch = getchar();
	tcsetattr(0, TCSAFLUSH, &save);
	return ch;
}

void recordundo() { // (김윤태)
	for (int i = 4; i >= 1; i--) { 						// 저장되어있던 undo용 맵 umap배열의 배열을 한 칸씩 뒤로 빼준다 ( 예 : umap[1][0][0]를 umap[2][0][0]로 )
		for (int j = 0; j < 30; j++) {
			for (int k = 0; k < 30; k++) {
				uMap[i][j][k] = uMap[i - 1][j][k];
			}
		}
		upl[i][0] = upl[i - 1][0], upl[i][1] = upl[i - 1][1];
	} 									// undo 후의 '@' 위치를 표시하는 upl 배열을 위의 uMap과 같이 한 칸씩 뒤로 빼준다 ( 예 : upl[1][0]를 upl[2][0]로 )
	for (int i = 0; i < 30; i++) { 						// uMap의 0번째 배열에 현재맵 cMap배열을 넣어준다
		for (int j = 0; j < 30; j++) {
			uMap[0][i][j] = cMap[i][j];
		}
	}
	upl[0][0] = plX, upl[0][1] = plY;
} 										// upl[0] 배열에 현재 '@' 의 좌표를 넣어준다

void moveCommand(char ch) { // (이성현)
	recordundo();
	int dX = 0, dY = 0;
	switch (ch) {
	case 'h': 								// 왼쪽으로 이동하므로 X축이 +0 Y축이 -1
		dX = 0;
		dY = -1;
		break;
	case 'j':								// 아래쪽으로 이동하므로 X축이 +1
		dX = 1;
		dY = 0;
		break;
	case 'k': 								// 위쪽으로 이동하므로 X축이 -1 Y축이 +0
		dX = -1;
		dY = 0;
		break;
	case 'l': 								// 오른쪽으로 이동하므로 X축이 +0 Y축이 +1
		dX = 0;
		dY = 1;
		break;
	}
	int X = plX + dX, Y = plY + dY, aX = X + dX, aY = Y + dY;
	if (cMap[X][Y] == '.' || cMap[X][Y] == 'O') { 				// 이동하려는 칸이 '.'일 경우
		cMap[X][Y] = '@', cMap[plX][plY] = '.';
	}
	else if (cMap[X][Y] == '#') { 						// 이동하려는 칸이 '#'일 경우
		dX = 0, dY = 0;
	}
	else if (cMap[X][Y] == '$') { 						// 이동하려는 칸이 '$'일 경우 '$'의 옆칸을 확인
		if (cMap[aX][aY] == '.' || cMap[aX][aY] == 'O') { 		// '$'의 옆칸이 '.'이나 'O'일 경우
			cMap[aX][aY] = '$', cMap[X][Y] = '@', cMap[plX][plY] = '.';
		}
		else if (cMap[aX][aY] == '#' || cMap[aX][aY] == '$') { 		// '$'의 옆칸이 '#'이나 '$'일 경우
			dX = 0, dY = 0;
		}
	}
	plX += dX, plY += dY, mvCnt++;
	system("clear");
	printMap();
	return;
}

void undo() { // (김윤태)
	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < 30; j++) {
			cMap[i][j] = uMap[0][i][j];
		}
	} 									// 현재 맵배열에 undo용으로 저장된 uMap배열의 0번째 배열을 가져온다
	plX = upl[0][0], plY = upl[0][1]; 					// 현재 플레이어 '@' 의 X , Y 좌표에 undo후 위치용으로 저장된 배열의 0번째 배열을 가져온다
	for (int i = 0; i <= 4; i++) { 						// 저장되 있던 되돌리기용 맵 배열을 한 칸씩 앞으로 당겨옴
		for (int j = 0; j < 30; j++) {
			for (int k = 0; k < 30; k++) {
				uMap[i][j][k] = uMap[i + 1][j][k];
			}
		} 								// undo하기 위해 저장된 맵배열 uMap을 한 칸씩 앞으로 가져온다 ( 예 : uMap[1][0][0]를 uMap[0][0][0]로 )
		upl[i][0] = upl[i + 1][0], upl[i][1] = upl[i + 1][1]; 		// 저장되 있던 '@' 위치 배열을 한 칸씩 앞으로 당겨옴
	} 									// undo후의 '@' 의 위치를 표시하는 upl 배열을 위 uMap과 같이 한 칸씩 앞으로 가져온다( 예 : upl[1][0]를 upl[0][0]로 )
	printMap();
} 										// undo후의 맵을 출력한다.

void resetundo() { // (김윤태)
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 30; j++) {
			for (int k = 0; k < 30; k++) {
				uMap[i][j][k] = '\0';
			} 							// uMap배열의 모든 값들을 null 값으로 초기화
		}
		upl[i][0] = '\0', upl[i][1] = '\0';
	}
} 										// undo후 플레이어 위치인 upl 배열 역시 모두 null 값으로 초기화

void createRangkingFile() { // (이창우)
	FILE *fp;
	fp = fopen("rangking.txt", "r");
	if (fp == 0) {                              				// 랭킹파일이 없는경우
		fclose(fp);
		fp = fopen("rangking.txt", "w");    				// 랭킹파일을 쓰기형식으로 생성하여 연다.
		fprintf(fp, "%d",100);               
	}
	fclose(fp);
}

void inputRangking() { // (이창우)
	int infront = 0, limitFive = 0;             				// 순위조정, 5개 제한을  위한 변수
	FILE *ifp, *ofp;
	ifp = fopen("rangking.txt", "r");           				// rangking.txt파일을 읽어들여 순위를 지정
	while (1) {                                  				// 순위가 제자리를 찾아갈때까지 반복
		fscanf(ifp, "%d %s %d", &rIndex, &rName, &rMvCnt);
		if ((rIndex - 1 < cIndex) || ((rIndex - 1 == cIndex) && (rMvCnt < mvCnt))) {
			infront++;
		}
		else {
			break;
		}
	}
	fclose(ifp);
	ifp = fopen("rangking.txt", "r"), ofp = fopen("newRangking.txt", "w");
	for (int i = 0; i < infront; i++) {        				// rangking.txt에서 받아온 정보를 new rangking.txt에 저장
		fscanf(ifp, "%d %s %d", &rIndex, &rName, &rMvCnt);
		fprintf(ofp, "%d %s %d\n", rIndex, rName, rMvCnt);
		if (rIndex - 1 == cIndex) {
			limitFive++;
		}
	}
	fprintf(ofp, "%d %s %d\n", cIndex + 1, name, mvCnt);
	limitFive++;
	while (1) {
		fscanf(ifp, "%d %s %d", &rIndex, &rName, &rMvCnt);
		if (rIndex == 100) {
			fprintf(ofp, "%d", 100);             			// 100이 보이면 종료         
			break;
		}
		else if (rIndex - 1 == cIndex) {         			// limitFive 즉 5위 이내에 순위 배정이 되어있으면 출력
			if (limitFive < 5) {
				fprintf(ofp, "%d %s %d\n", rIndex, rName, rMvCnt);
				limitFive++;
			}
		}
		else {
			fprintf(ofp, "%d %s %d\n", rIndex, rName, rMvCnt);
		}
	}
	fclose(ifp), fclose(ofp);
	ifp = fopen("rangking.txt", "w"), ofp = fopen("newRangking.txt", "r");
	while (1) {
		fscanf(ofp, "%d %s %d", &rIndex, &rName, &rMvCnt);
		if (rIndex == 100) {
			fprintf(ifp, "%d", 100);
			break;
		}
		else {
			fprintf(ifp, "%d %s %d\n", rIndex, rName, rMvCnt);
		}
	}
	fclose(ifp), fclose(ofp);
	remove("newRangking.txt");
}
		
void loadAllRangking(){ // (이창우) 
	// 전체 랭킹을 출력
  	int previousRIndex = 0, rank = 1;
	printf("\n전체 맵 랭킹을 출력합니다.\n");
   	FILE *fp;
   	fp = fopen("rangking.txt", "r");
   	while(1) {                                       			//rangking.txt의 내용을 입력받고, 전체 랭킹을 출력함
   		  previousRIndex = rIndex;
      	fscanf(fp, "%d %s %d", &rIndex, &rName, &rMvCnt);
       	if(rIndex == 100){
       	    break;
        } 
		else{
       		if(rIndex != previousRIndex){
         	 	printf("\n%d번 맵 랭킹\n\n", rIndex);
           		rank = 1;
			}
        	printf("%d위\t이름: %-10s\t이동횟수: %d\n", rank, rName, rMvCnt);
        	rank++;
     	}
	}
    fclose(fp);
}

void loadMapRangking (char a) { // (이창우) 
	// 해당 번호의 맵 랭킹만 출력
	int rank = 1;
	printf("%c번 맵 랭킹을 출력합니다.\n\n", a);
	FILE *fp;
	fp = fopen("rangking.txt", "r");            
	while(1){                                       			//해당 번호의 rIndex(rangking에서의 맵 번호)를 찾아 그 랭킹만 출력
        fscanf(fp, "%d %s %d", &rIndex, &rName, &rMvCnt);
        if(a - 48 == rIndex) {
            printf("%d위\t이름: %-10s\t이동횟수: %d\n", rank, rName, rMvCnt);
            rank++;
        } 
		else if(a - 49 < rIndex){
            break;
        }	
    }
}

void manual() { // (이성현)
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

void saveMap() { // (유상진)
	// 현재 맵 상태를 sokoban.txt 파일에 저장함
	FILE *fp;
	fp = fopen("sokoban.txt", "w");
	fprintf(fp, "%d\n", cIndex + 1); // 현재 맵 번호 저장
	fprintf(fp, "%d\n", mvCnt); // 이동 횟수 저장
	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < 30; j++) {
			fprintf(fp, "%c", cMap[i][j]);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "e");
	fclose(fp);
}

void loadMap() { // (유상진)
	// sokoban.txt 파일에 저장한 현재 맵을 불러옴
	char buffer[30] = { 0, };
	FILE *fp;
	fp = fopen("sokoban.txt", "r");
	if (fp == 0) {
		printf("There isn't saved map\n\n");
		printMap();
	}
	else {
		int l = 0;
		while (1) {
			fscanf(fp, "%s", &buffer); 				// map.txt 파일을 한 줄씩 읽어들임
			if (buffer[0] == 'e') { 				// 읽어들인 줄이 'e'일 경우
				break;
			}
			else if (buffer[0] >= 49 && buffer[0] <= 53) { 		// 읽어들인 줄이 1 ~ 5일 경우
				cIndex = buffer[0] - 49, l = 0;
				fscanf(fp, "%d", &mvCnt); 			// 이동 횟수 불러옴
			}
			else {
				for (int i = 0; i < 30; i++) { 			// 읽어들인 줄을 한 문자씩 맵 배열에 저장함
					if (buffer[i] == '\0') {
						break;
					}
					else if (buffer[i] == '@') {
						plX = l, plY = i;
					}
					cMap[l][i] = buffer[i];
				}
				l += 1;
			}
		}
		printMap();
		fclose(fp);
		resetundo();
		printf("\nSaved Map has been output\n");
	}
}

void command() {
	// 사용자로부터 키를 입력 받아 해당 명령을 실행하는 함수
	char ch;
	ch = getchar();
	while (1) {
		printf("\n");
		ch = getch();
		if (ch == 't') { 						// 전체 맵 순위만 출력할지 특정한 맵 순위만 출력할건지 다시 입력받아야 하므로 if문을 통해 구분함
            		printf("t ");
	            	char a = getchar();
        	    	system("clear");
            		printMap();
            		if(a == '\n'){
                		loadAllRangking();
			} 
			else {
				if((a >= '1' && a <= '5') && (map[a - 49][0][0] != '\0')) {
					printf("\n");
	        			loadMapRangking(a);
				}
				else{
					printf("\n해당 맵이 존재하지 않습니다.\n");
				}
	                ch = getchar();
        	    	}
            		printf("\n");
		}
		else {
			switch (ch) {
			case 'u':
				if (uMap[0][0][0] == '\0') {
					system("clear");
					printMap();
					printf("\n더 이상 되돌리기가 불가능 합니다.\n");
				}
				else {
					system("clear");
					mvCnt++;
					undo();
				}
				break;
			case 'r':						// 현재 레벨을 새로 시작
				system("clear");
				printf("Restart This Stage.\n\n");
				resetundo();
				createCurrentMap();
				printMap();
				break;
			case 'n':						// 레벨 1부터 새로운 게임 시작
				system("clear");
				printf("Restart Game.\n\n");
				cIndex = 0, mvCnt = 0;
				resetundo();
				createCurrentMap();
				printMap();
				break;
			case 'e': 						// 무한반복을 끝내고 탈출함
				system("clear");
				printf("SEE YOU %s...",name);
				goto EXIT;
			case 's':						// 현재 맵 상태를 저장함
				saveMap();
				system("clear");
				printMap();
				printf("\nSave Current Map\n");
				break;
			case 'f':						// 저장된 맵을 불러옴
				system("clear");
				loadMap();
				break;
			case 'd': 						// 매뉴얼 출력
				if (ch == 'd'){
					printf("d");
					char b= getchar();
					if (b == '\n') {
						system("clear");
						printMap();
						manual();
					}
				}
				break;
			default:
				if (ch == 'h' || ch == 'j' || ch == 'k' || ch == 'l') { // 이동키일경우 moveCommand() 함수 실행
					moveCommand(ch);
				}
				else { 						// 명령어 & 이동키가 아닐 경우 '잘못된 명령입니다.' 출력
					system("clear");
					printMap();
					printf("\nWrong Command\n");
				}
				break;
			}
			if (nextStage) {
				system("clear");
				printf("\nLevel %d Clear!!!\n", cIndex + 1);
				inputRangking();
				mvCnt = 0, nextStage = 0, cIndex++;
				resetundo();
				if (map[cIndex][0][0] == '\0') {
					printf("Game Clear!!! Program closed\n");
					break;
				}
				else {
					createCurrentMap();
				}
				printf("\n");
				printMap();
			}
		}
	}
EXIT:
	return;
}
