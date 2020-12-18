#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));
	createRankList();

	while(!exit){
		
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_RECO: recommendedPlay(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	for(i=0;i<VISIBLE_BLOCKS;i++){
		nextBlock[i] = rand()%7;
	}
	
//	nextBlock[0]=rand()%7;
//	nextBlock[1]=rand()%7;
//	nextBlock[2]=rand()%7;

	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	//1
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	//2
	DrawBox(10,WIDTH+10,4,8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(16,WIDTH+10);
	printw("SCORE");
	DrawBox(17,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(18,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	//1
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
	//2
	for( i = 0; i < 4; i++ ){
		move(11+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[2]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}

}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i, j;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			if(block[currentBlock][blockRotate][i][j]==1){
				if(f[i+blockY][j+blockX]==1){
					return 0;
				}
				if(j+blockX+1 > WIDTH){
					return 0;
				}
				if(i+blockY+1 > HEIGHT){
					return 0;
				}
				if(j+blockX < 0){
					return 0;
				}
				
			}	
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i, j, where;
	where=blockY;
	switch(command){
		case KEY_UP:
			for(i=0;i<4;i++){
				for(j=0;j<4;j++){
					if(block[currentBlock][(blockRotate+3)%4][i][j]==1){
						move(i+blockY+1, j+blockX+1);
						if(i+blockY+1!=0){
							printw(".");
						}

						while(true){
							if(CheckToMove(field, nextBlock[0], (blockRotate+3)%4, where++, blockX)==0){
								where--;
								break;
							}
						}

						move(i+where, j+blockX+1);
						
						printw(".");
						
					}
				}
			}

			DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
			break;
		case KEY_DOWN:
			for(i=0;i<4;i++){
				for(j=0;j<4;j++){
					if(block[currentBlock][blockRotate][i][j]==1){
						move(i+blockY, j+blockX+1);
						printw(".");
						
					}
				}
			}
			DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
			break;
		case KEY_RIGHT:
			for(i=0;i<4;i++){
				for(j=0;j<4;j++){
					if(block[currentBlock][blockRotate][i][j]==1){
						move(i+blockY+1,j+blockX);
						printw(".");
						while(true){
							if(CheckToMove(field,nextBlock[0],blockRotate,where++,blockX-1)==0){
								where--;
								break;
							}
						}
						move(i+where, j+blockX);
						printw(".");
					}
				}
			}
			DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
			break;
		case KEY_LEFT:
			for(i=0;i<4;i++){
				for(j=0;j<4;j++){
					if(block[currentBlock][blockRotate][i][j]==1){
						move(i+blockY+1, j+blockX+2);
						printw(".");
						while(true){
							if(CheckToMove(field,nextBlock[0],blockRotate,where++,blockX+1)==0){
								where--;
								break;
							}
						}
						move(i+where, j+blockX+2);
						printw(".");
					}
				}	
			}
			DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
			break;
		default:
			break;

	}

	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	//3. 새로운 블록 정보를 그린다. 
}

void BlockDown(int sig){
	// user code
	int sc, i;
	RecNode* root=NULL;
	if(CheckToMove(field, nextBlock[0],blockRotate,blockY+1, blockX)){
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	}else{
		if(blockY==-1){
			gameOver=1;
		}
		sc = AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
		score = score + sc;
		PrintScore(score);
		blockRotate=0;
		blockY=-1;
		blockX=WIDTH/2-2;
		sc = DeleteLine(field);
		score = score + sc;
		PrintScore(score);
		DrawField();

		for(i=0;i<VISIBLE_BLOCKS-1;i++){
			nextBlock[i] = nextBlock[i+1];
		}
		nextBlock[VISIBLE_BLOCKS-1] = rand()%7;
		DrawNextBlock(nextBlock);
		recommend(root);
		free(root);
	}
	alarm(1);
	

	//강의자료 p26-27의 플로우차트를 참고한다.
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i, j, count;
	count=0;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			if(block[currentBlock][blockRotate][i][j]==1){
				f[i+blockY][j+blockX]=1;
				if(f[(i+blockY+1)][j+blockX]==1||(i+blockY+1)==HEIGHT ){
				count++;
				}

			}
		}
	}
	//score = (score+count*10);
	//PrintScore(score);	
	return count*10;
	//Block이 추가된 영역의 필드값을 바꾼다.
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code
	int i,j,k,l;
	int lines=0;
	for(i=0;i<HEIGHT;i++){
		int flag=1;
		for(j=0;j<WIDTH;j++){
			if(f[i][j]==0){
				flag=0;
				break;		
			}
		}
		if(flag==1){
			lines++;
			for(k=i;k>0;k--){
				for(l=0;l<WIDTH;l++){
					f[k][l]=f[k-1][l];
				}
			}
		}
	}
	//DrawField(f);
	//score = score + (lines*lines)*100;
	//PrintScore(score);
	return (lines*lines)*100;

	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
	int i, j, where;
	where = y;
	while(true){
		where++;
		if(CheckToMove(field, nextBlock[0], blockRotate, where, x)==0){
			where--;
			DrawBlock(where, x, blockID, blockRotate, '/');
			break;
		}
	}
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
	
	int i, j;
	DrawBlock(y, x, blockID, blockRotate, ' ');
	DrawShadow(y, x, blockID, blockRotate);
}

void createRankList(){
	// user code
	FILE *fp;
	int num;
	int s, i, j;
	char nm[NAMELEN];
	Node* now;
	Node* tmp;
	Node* pre;
	//free List
	if(list != NULL){
		now = list->first;
		for(i=0;i<list->length;i++){
			pre = now;
			now = now->next;
			free(pre);
		}
		free(list);
	}
	list = (LinkedList*)malloc(sizeof(LinkedList)*1);
	list->length=0;
	fp = fopen("rank.txt","r");
	if(fp == NULL){
		fp=fopen("rank.txt","w");
		fprintf(fp, "0");
		fclose(fp);
	}

	fscanf(fp, "%d", &num);

	if(num==0){
		return;
	}

	fscanf(fp, "%s", nm);
	fscanf(fp, "%d", &s);
	tmp = (Node*)malloc(sizeof(Node)*1);
	tmp->score = s;
	strcpy(tmp->name, nm);
	list->first = tmp;
	list->length++;

	for(i=0;i<num-1;i++){
		fscanf(fp,"%s", nm);
		fscanf(fp,"%d", &s);
		tmp = (Node*)malloc(sizeof(Node)*1);
		tmp->score=s;
		strcpy(tmp->name, nm);
		now = list->first;
		for(j=0;j<list->length;j++){
			if(tmp->score >= now->score){
				if(j==0){
					tmp->next = now;
					list->first = tmp;
				}else{
					tmp->next = now;
					pre->next = tmp;
				}
				
				list->length++;
				break;
				
			}else{
				if(j==list->length-1){
					now->next = tmp;
					list->length++;
					break;
				}else{
					pre = now;
					now = now->next;
				}
			}
			
		}


	}
	fclose(fp);




}

void rank(){
	// user code
	char m, x, y, d;
	char nm[NAMELEN];
	int i, j, xi, yi, count, di;
	Node* now;
	Node* tmp;

	clear();

	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");
	
	m = wgetch(stdscr);
	if(m=='1'){
		
		echo();
		printw("X: ");
		x = wgetch(stdscr);
		printw("\n");
		if(x==NULL){
			xi = 1;
		}else{
			xi = x-'0';
		}
		printw("Y: ");
		
		y = wgetch(stdscr);

		printw("\n");

		if(y>list->length){
			yi = list->length;
		}else{
			yi = y - '0';
		}

		printw("       name       |   score   \n");	
		printw("------------------------------\n");

		if(xi>list->length || yi<xi){
			printw("search failure: no rank in the list\n");
			wgetch(stdscr);
			return;
		}
		noecho();

		now = list->first;
		for(i=1;i<xi;i++){
			now=now->next;
		}

		for(i=0;i<yi-xi+1;i++){
			printw("%-18s|%-11d\n", now->name, now->score);
			now = now->next;
		}
		
	}
	if(m=='2'){
		printw("input the name: ");
		echo();
		wgetstr(stdscr, nm);
		noecho();

		printw("       name       |   score   \n");	
		printw("------------------------------\n");
	
		count=0;
		now = list->first;
		for(i=0;i<list->length;i++){
			if(strcmp(now->name, nm)==0){
				printw("%-18s|%-11d\n", now->name, now->score);
				count++;
			}
			now = now->next;
		}
		if(count==0){
			printw("search failure: no name in the list\n");
		}
	}
	if(m=='3'){
		printw("input the rank: ");
		echo();
		nocbreak();
		d = wgetch(stdscr);
		noecho();
		printw("\n");
		di = d - '0';

		if(di > list->length){
			printw("search failure: the rank not in the list\n");
			wgetch(stdscr);
		}else{
			now = list->first;
			if(di==1){
				tmp = now;
				list->first = now->next;
				free(tmp);
				list->length--;

			}else{
				for(i=0;i<di-1;i++){
					tmp = now;
					now = now->next;
				}
				tmp->next = now->next;
				free(now);
				list->length--;

				printw("result: the rank deleted\n");
				wgetch(stdscr);
			}

			writeRankFile();

		}
		

	}

	wgetch(stdscr);
	cbreak();
	clear();

	

}

void writeRankFile(){
	// user code
	FILE* fp;
	Node* now;
	int i;

	fp = fopen("rank.txt","w");
	fprintf(fp, "%d\n", list->length);
	now = list->first;
	for(i=0;i<list->length;i++){
		fprintf(fp,"%s %d\n",now->name, now->score );
		now = now->next;
	}
	fclose(fp);
	return;



}

void newRank(int score){
	// user code
	char nm[NAMELEN];
	Node* tmp, *now, *pre;
	int i;
	
	clear();
	printw("your name: ");
	echo();
	//nocbreak();
	wgetstr(stdscr,nm);
	noecho();

	tmp = (Node*)malloc(sizeof(Node)*1);
	tmp->score = score;
	strcpy(tmp->name, nm);
	now = list->first;

	if(list->length==0){
		list->first=tmp;
		list->length++;
		writeRankFile();
		clear();
		return;
	}
	
	for(i=0;i<list->length;i++){
		if(tmp->score >= now->score){
			if(i==0){
				tmp->next = now;
				list->first = tmp;
			}else{
				tmp->next = now;
				pre->next = tmp;
			}
			list->length++;
			break;
		}else{
			if(i==list->length-1){
				now->next = tmp;
				list->length++;
				break;
			}else{
				pre = now;
				now = now->next;
			}
		}
	}


	clear();
	writeRankFile();
	return;
	

}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
	DrawBlock(y,x,blockID,blockRotate,'R');
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int x,y,rotate;
	int i;
	// user code
	RecNode* recommend;
	RecNode* p;
	root = (RecNode*)malloc(sizeof(RecNode)*1);
	root->score=0;
	root->lv=0;
	recommend = makeTree(root, field, max);
	p = recommend;
	for(i=0;i<VISIBLE_BLOCKS-1;i++){
		p = p->parent;
	}	

	x = p->recBX;
	y = p->recBY;
	rotate = p->recBR;
	max = recommend->score;
//	deleteTree(root);
	DrawField();
	DrawRecommend(y,x,nextBlock[0],rotate);
//	deleteTree(root);
//	free(root);

	return max;
}

void recommendedPlay(){
	int i, command, ret;
	RecNode* toDraw;
	clear();
	act.sa_handler = sigRecommend;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}
		command = GetCommand();
		if(command==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();
			return;
		}
		
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver");
	refresh();
	getch();

}

void sigRecommend(){
	int x, y, rotate, max=0,s1,s2;
	int i;
	RecNode* root;
	RecNode* recommended;
	RecNode* p;
//	root = (RecNode*)malloc(sizeof(RecNode)*1);
	root = (RecNode*)calloc(1,sizeof(RecNode));
	root->score=0;
	root->lv=0;
//	recRoot = (RecNode*)malloc(sizeof(RecNode)*1);
//	recRoot->score=0;
//	recRoot->lv=0;

	recommended = makeTree(root, field, max);
	p = recommended;
	for(i=0;i<VISIBLE_BLOCKS-1;i++){
		p = p->parent;
	}	
	x = p->recBX;
	y = p->recBY;
	rotate = p->recBR;
	max = recommended->score;
	deleteTree(root);
	free(root);
	DrawField();

	DrawRecommend(y,x,nextBlock[0],rotate);
//	sleep(1);
	s1 = AddBlockToField(field, nextBlock[0],rotate,y,x);
	s2 = DeleteLine(field);
	score = score+s1+s2;
	PrintScore(score);
	DrawField();
	for(i=0;i<VISIBLE_BLOCKS-1;i++){
		nextBlock[i] = nextBlock[i+1];
	}
	nextBlock[VISIBLE_BLOCKS-1] = rand()%7;
	DrawNextBlock(nextBlock);
//	deleteTree(recRoot);
//	free(recRoot);
	if(y<=0){
		gameOver = 1;
	}
	alarm(1);

}

void deleteTree(RecNode* node){
	RecNode* next;
	int i;
	for(i=0;i<CHILDREN_MAX;i++){
		next = (node->c)[i];
		if(next==NULL){
			return;
		}
		deleteTree(next);
		free(next);
		
	}	
	return;
}

RecNode* makeTree(RecNode *node, char f[HEIGHT][WIDTH], int max){
	int i, j, k, blockID, s1, s2, y;
	RecNode* next;
	RecNode* temp;
	RecNode* maxNode=node;
	

	if(node->lv >= VISIBLE_BLOCKS){
		//max와 비교..
	//	if((node->score) > max){
	//		maxNode = node;
	//	} 

		return maxNode;
	}


	blockID = nextBlock[node->lv];
	if(blockID == 0){
	//	node->c = (RecNode*)malloc(sizeof(RecNode)*17);
		for(i=0;i<7;i++){
			for(j=0;j<HEIGHT;j++){
				for(k=0;k<WIDTH;k++){
					node->recField[j][k] = f[j][k];
				}
			}
			y = 0;
			while(CheckToMove(node->recField,blockID,0,y,i)==1){
				y++;
			}
			y--;
			s1 = AddBlockToField(node->recField,blockID,0,y,i);
			s2 = DeleteLine(node->recField);
		//	(node->c)[i] = (RecNode*)malloc(sizeof(RecNode)*1);
			(node->c)[i] = (RecNode*)calloc(1,sizeof(RecNode));

			next = (node->c)[i];
			next->lv = node->lv + 1;
			next->score = node->score + s1 + s2;
			next->recBX = i;
			next->recBY = y;
			next->recBR = 0;
			next->parent = node;
			temp = makeTree(next,node->recField,max);
			if(maxNode->score < temp->score){
				maxNode = temp;
			}
		}
		for(i=7;i<17;i++){
			for(j=0;j<HEIGHT;j++){
				for(k=0;k<WIDTH;k++){
					node->recField[j][k] = f[j][k];
				}
			}
			y = 0;
			while(CheckToMove(node->recField,blockID,1,y,i-8)==1){
				y++;
			}
			y--;
			s1 = AddBlockToField(node->recField,blockID,1,y,i-8);
			s2 = DeleteLine(node->recField);
		//	(node->c)[i] = (RecNode*)malloc(sizeof(RecNode)*1);
			(node->c)[i] = (RecNode*)calloc(1,sizeof(RecNode));
			next = (node->c)[i];
			next->lv = node->lv + 1;
			next->score = node->score + s1 + s2;
			next->recBX = i-8;
			next->recBY = y;
			next->recBR = 1;
			next->parent = node;
			temp = makeTree(next,node->recField,max);
			if(maxNode->score < temp->score){
				maxNode = temp;
			}
		}
	}
	if(blockID==1||blockID == 2){
	//	node->c = (RecNode*)malloc(sizeof(RecNode)*34);
		for(i=0;i<8;i++){
			for(j=0;j<HEIGHT;j++){
				for(k=0;k<WIDTH;k++){
					node->recField[j][k] = f[j][k];
				}
			}
			y = 0;
			while(CheckToMove(node->recField,blockID,0,y,i-1)==1){
				y++;
			}
			y--;
			s1 = AddBlockToField(node->recField,blockID,0,y,i-1);
			s2 = DeleteLine(node->recField);
		//	(node->c)[i] = (RecNode*)malloc(sizeof(RecNode)*1);
			(node->c)[i] = (RecNode*)calloc(1,sizeof(RecNode));
			next = (node->c)[i];
			next->lv = node->lv + 1;
			next->score = node->score + s1 + s2;
			next->recBX = i-1;
			next->recBY = y;
			next->recBR = 0;
			next->parent = node;
			temp = makeTree(next,node->recField,max);
			if(maxNode->score < temp->score){
				maxNode = temp;
			}
		}
		for(i=8;i<17;i++){
			for(j=0;j<HEIGHT;j++){
				for(k=0;k<WIDTH;k++){
					node->recField[j][k] = f[j][k];
				}
			}
			y = 0;
			while(CheckToMove(node->recField,blockID,1,y,i-10)==1){
				y++;
			}
			y--;
			s1 = AddBlockToField(node->recField,blockID,1,y,i-10);
			s2 = DeleteLine(node->recField);
		//	(node->c)[i] = (RecNode*)malloc(sizeof(RecNode)*1);
			(node->c)[i] = (RecNode*)calloc(1,sizeof(RecNode));

			next = (node->c)[i];
			next->lv = node->lv + 1;
			next->score = node->score + s1 + s2;
			next->recBX = i-10;
			next->recBY = y;
			next->recBR = 1;
			next->parent = node;
			temp = makeTree(next,node->recField,max);
			if(maxNode->score < temp->score){
				maxNode = temp;
			}
		}
		for(i=17;i<25;i++){
			for(j=0;j<HEIGHT;j++){
				for(k=0;k<WIDTH;k++){
					node->recField[j][k] = f[j][k];
				}
			}
			y = 0;
			while(CheckToMove(node->recField,blockID,2,y,i-18)==1){
				y++;
			}
			y--;
			s1 = AddBlockToField(node->recField,blockID,2,y,i-18);
			s2 = DeleteLine(node->recField);
		//	(node->c)[i] = (RecNode*)malloc(sizeof(RecNode)*1);
			(node->c)[i] = (RecNode*)calloc(1,sizeof(RecNode));

			next = (node->c)[i];
			next->lv = node->lv + 1;
			next->score = node->score + s1 + s2;
			next->recBX = i-18;
			next->recBY = y;
			next->recBR = 2;
			next->parent = node;
			temp = makeTree(next,node->recField,max);
			if(maxNode->score < temp->score){
				maxNode = temp;
			}
		}
		for(i=25;i<34;i++){
			for(j=0;j<HEIGHT;j++){
				for(k=0;k<WIDTH;k++){
					node->recField[j][k] = f[j][k];
				}
			}
			y = 0;
			while(CheckToMove(node->recField,blockID,3,y,i-26)==1){
				y++;
			}
			y--;
			s1 = AddBlockToField(node->recField,blockID,3,y,i-26);
			s2 = DeleteLine(node->recField);
		//	(node->c)[i] = (RecNode*)malloc(sizeof(RecNode)*1);
			(node->c)[i] = (RecNode*)calloc(1,sizeof(RecNode));

			next = (node->c)[i];
			next->lv = node->lv + 1;
			next->score = node->score + s1 + s2;
			next->recBX = i-26;
			next->recBY = y;
			next->recBR = 3;
			next->parent = node;
			temp = makeTree(next,node->recField,max);
			if(maxNode->score < temp->score){
				maxNode = temp;
			}
		}
	}
	if(blockID == 3){
	//	node->c = (RecNode*)malloc(sizeof(RecNode)*34);
		for(i=0;i<8;i++){
			for(j=0;j<HEIGHT;j++){
				for(k=0;k<WIDTH;k++){
					node->recField[j][k] = f[j][k];
				}
			}
			y = 0;
			while(CheckToMove(node->recField,blockID,0,y,i)==1){
				y++;
			}
			y--;
			s1 = AddBlockToField(node->recField,blockID,0,y,i);
			s2 = DeleteLine(node->recField);
		//	(node->c)[i] = (RecNode*)malloc(sizeof(RecNode)*1);
			(node->c)[i] = (RecNode*)calloc(1,sizeof(RecNode));

			next = (node->c)[i];
			next->lv = node->lv + 1;
			next->score = node->score + s1 + s2;
			next->recBX = i;
			next->recBY = y;
			next->recBR = 0;
			next->parent = node;
			temp = makeTree(next,node->recField,max);
			if(maxNode->score < temp->score){
				maxNode = temp;
			}
		}
		for(i=8;i<17;i++){
			for(j=0;j<HEIGHT;j++){
				for(k=0;k<WIDTH;k++){
					node->recField[j][k] = f[j][k];
				}
			}
			y = 0;
			while(CheckToMove(node->recField,blockID,1,y,i-8)==1){
				y++;
			}
			y--;
			s1 = AddBlockToField(node->recField,blockID,1,y,i-8);
			s2 = DeleteLine(node->recField);
		//	(node->c)[i] = (RecNode*)malloc(sizeof(RecNode)*1);
			(node->c)[i] = (RecNode*)calloc(1,sizeof(RecNode));

			next = (node->c)[i];
			next->lv = node->lv + 1;
			next->score = node->score + s1 + s2;
			next->recBX = i-8;
			next->recBY = y;
			next->recBR = 1;
			next->parent = node;
			temp = makeTree(next,node->recField,max);
			if(maxNode->score < temp->score){
				maxNode = temp;
			}
		}
		for(i=17;i<25;i++){
			for(j=0;j<HEIGHT;j++){
				for(k=0;k<WIDTH;k++){
					node->recField[j][k] = f[j][k];
				}
			}
			y = 0;
			while(CheckToMove(node->recField,blockID,2,y,i-17)==1){
				y++;
			}
			y--;
			s1 = AddBlockToField(node->recField,blockID,2,y,i-17);
			s2 = DeleteLine(node->recField);
		//	(node->c)[i] = (RecNode*)malloc(sizeof(RecNode)*1);
			(node->c)[i] = (RecNode*)calloc(1,sizeof(RecNode));

			next = (node->c)[i];
			next->lv = node->lv + 1;
			next->score = node->score + s1 + s2;
			next->recBX = i-17;
			next->recBY = y;
			next->recBR = 2;
			next->parent = node;
			temp = makeTree(next,node->recField,max);
			if(maxNode->score < temp->score){
				maxNode = temp;
			}
		}
		for(i=25;i<34;i++){
			for(j=0;j<HEIGHT;j++){
				for(k=0;k<WIDTH;k++){
					node->recField[j][k] = f[j][k];
				}
			}
			y = 0;
			while(CheckToMove(node->recField,blockID,3,y,i-26)==1){
				y++;
			}
			y--;
			s1 = AddBlockToField(node->recField,blockID,3,y,i-26);
			s2 = DeleteLine(node->recField);
		//	(node->c)[i] = (RecNode*)malloc(sizeof(RecNode)*1);
			(node->c)[i] = (RecNode*)calloc(1,sizeof(RecNode));

			next = (node->c)[i];
			next->lv = node->lv + 1;
			next->score = node->score + s1 + s2;
			next->recBX = i-26;
			next->recBY = y;
			next->recBR = 3;
			next->parent = node;
			temp = makeTree(next,node->recField,max);
			if(maxNode->score < temp->score){
				maxNode = temp;
			}
		}
	}
	if(blockID == 4){
	//	node->c = (RecNode*)malloc(sizeof(RecNode)*9);
		for(i=0;i<9;i++){
			for(j=0;j<HEIGHT;j++){
				for(k=0;k<WIDTH;k++){
					node->recField[j][k] = f[j][k];
				}
			}
			y = 0;
			while(CheckToMove(node->recField,blockID,0,y,i-1)==1){
				y++;
			}
			y--;
			s1 = AddBlockToField(node->recField,blockID,0,y,i-1);
			s2 = DeleteLine(node->recField);
		//	(node->c)[i] = (RecNode*)malloc(sizeof(RecNode)*1);
			(node->c)[i] = (RecNode*)calloc(1,sizeof(RecNode));

			next = (node->c)[i];
			next->lv = node->lv + 1;
			next->score = node->score + s1 + s2;
			next->recBX = i-1;
			next->recBY = y;
			next->recBR = 0;
			next->parent = node;
			temp = makeTree(next,node->recField,max);
			if(maxNode->score < temp->score){
				maxNode = temp;
			}
		}
	}
	if(blockID == 5 || blockID == 6){
	//	node->c = (RecNode*)malloc(sizeof(RecNode)*17);
		for(i=0;i<8;i++){
			for(j=0;j<HEIGHT;j++){
				for(k=0;k<WIDTH;k++){
					node->recField[j][k] = f[j][k];
				}
			}
			y = 0;
			while(CheckToMove(node->recField,blockID,0,y,i-1)==1){
				y++;
			}
			y--;
			s1 = AddBlockToField(node->recField,blockID,0,y,i-1);
			s2 = DeleteLine(node->recField);
		//	(node->c)[i] = (RecNode*)malloc(sizeof(RecNode)*1);
			(node->c)[i] = (RecNode*)calloc(1,sizeof(RecNode));

			next = (node->c)[i];
			next->lv = node->lv + 1;
			next->score = node->score + s1 + s2;
			next->recBX = i-1;
			next->recBY = y;
			next->recBR = 0;
			next->parent = node;
			temp = makeTree(next,node->recField,max);
			if(maxNode->score < temp->score){
				maxNode = temp;
			}
		}
		for(i=8;i<17;i++){
			for(j=0;j<HEIGHT;j++){
				for(k=0;k<WIDTH;k++){
					node->recField[j][k] = f[j][k];
				}
			}
			y = 0;
			while(CheckToMove(node->recField,blockID,1,y,i-9)==1){
				y++;
			}
			y--;
			s1 = AddBlockToField(node->recField,blockID,1,y,i-9);
			s2 = DeleteLine(node->recField);
		//	(node->c)[i] = (RecNode*)malloc(sizeof(RecNode)*1);
			(node->c)[i] = (RecNode*)calloc(1,sizeof(RecNode));

			next = (node->c)[i];
			next->lv = node->lv + 1;
			next->score = node->score + s1 + s2;
			next->recBX = i-9;
			next->recBY = y;
			next->recBR = 1;
			next->parent = node;
			temp = makeTree(next,node->recField,max);
			if(maxNode->score < temp->score){
				maxNode = temp;
			}
		}
	}

	return maxNode;
	
}


