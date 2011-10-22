#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define COL_DIFF 4
#define ROW_DIFF 6

typedef uint8_t boolean;

int crow = 0, ccol = 0;
int *scores = NULL;
WINDOW **boxes = NULL;
WINDOW *choice = NULL;
char **playerNames = NULL;

WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);
    box(local_win, 0 , 0); 
    return local_win;
}

void drawNumbers(int n, int lcd[]){
    WINDOW *c = boxes[n];
    if (lcd[0]){
        mvwprintw(c,1,6,"##########");
    }
    int j;
    if (lcd[1]){
        for (j = 1; j < 5; j++){
            mvwprintw(c, 1 + j, 5, "#" );
        }
    }
    if (lcd[2]){
        for (j = 1; j < 5; j++){
            mvwprintw(c, 1+j, 16, "#");
        }
    }
    if (lcd[3]){
        mvwprintw(c, 6, 6, "#########");
    }
    if (lcd[4]){
        for (j = 1; j < 4; j++ ){
            mvwprintw(c, 6 + j, 5, "#");
        }
    }
    if (lcd[5]){
        for( j = 1; j < 4; j++){
            mvwprintw(c, 6 + j, 16, "#");
        }
    }
    if (lcd[6]){
        mvwprintw(c, 10, 6 , "##########");
    }
    wrefresh(c);
    refresh();
}

void printNum(){
    int i, n;
    int lcd[7];
    for (i = 0; i < 4; i++){
        n = scores[i];
        lcd[0] = (n != 1 && n != 4) ? 1 : 0;
        lcd[1] = (n != 1 && n != 2 && n != 3 && n != 7) ? 1 : 0;
        lcd[2] = (n != 5 && n != 6) ? 1 : 0;
        lcd[3] = (n != 0 && n != 1 && n != 7) ? 1 : 0;
        lcd[4] = (n == 0 || n == 2 || n == 6 || n == 8) ? 1 : 0;
        lcd[5] = (n != 2) ? 1 : 0;
        lcd[6] = (n != 1 && n != 4 && n != 7) ? 1 : 0;
        drawNumbers(i, lcd);
    }
}



void printBox(int row, int col){
        
    boxes[0] = create_newwin(12,22,row, col);
    if (row == crow && col == ccol){
        wattron(boxes[0], A_STANDOUT);
    }
    wborder (boxes[0], '|','|','-','-','+','+','+','+');
    wattroff(boxes[0],A_STANDOUT);
    wrefresh(boxes[0]);

    row *= ROW_DIFF;
    boxes[2] = create_newwin(12,22,row, col);
    if (row == crow && col == ccol){
        wattron(boxes[2], A_STANDOUT);
    }
    wborder (boxes[2], '|','|','-','-','+','+','+','+');
    wrefresh(boxes[2]);
    wattroff(boxes[2],A_STANDOUT);

    col *= COL_DIFF;
    boxes[3] = create_newwin(12,22, row, col);
    if (row == crow && col == ccol){
        wattron(boxes[3], A_STANDOUT);
    }
    wborder (boxes[3], '|','|','-','-','+','+','+','+');
    wrefresh(boxes[3]);
    wattroff(boxes[3],A_STANDOUT);

    row /= ROW_DIFF;
    boxes[1] = create_newwin(12,22, row, col);
    if (row == crow && col == ccol){
        wattron(boxes[1], A_STANDOUT);
    }
    wborder (boxes[1], '|','|','-','-','+','+','+','+');
    wrefresh(boxes[1]);
    wattroff(boxes[1],A_STANDOUT);
    printNum();
    refresh();
}

void createChoiceBox(int row, int col){
    choice = create_newwin(11, 40, row *(ROW_DIFF - 3), col* (COL_DIFF + 3) );
    wborder(choice, ' ', ' ', '-', '-', '+','+', '+', '+');
    wrefresh(choice);
}

void deleteBoard(){
    int i;
    for ( i = 0; i < 4; i++){
        wborder(boxes[i], ' ', ' ', ' ',' ',' ',' ',' ',' ');
        wrefresh(boxes[i]);
        delwin(boxes[i]);
    }
}

void nullCheck(void *m){
    if ( m == NULL){
        perror("Memory Error!");
        exit(0);
    }
}

void deleteWindow(WINDOW *w){
    wborder(w, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wrefresh(w);
    delwin(w);

}

void nextPlayersTurn(int currentPl, WINDOW *p1w, WINDOW *p2w){
    WINDOW *c , *p;
    c = (currentPl == 0) ? p1w : p2w;
    p = (currentPl == 0) ? p2w : p1w;
    wborder(p, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    mvwprintw(p, 1, 0, "                          ");
    wrefresh(p);
    wborder(c , '|', '|' , '-', '-', '+', '+', '+', '+');
    mvwprintw(c, 1, 1, "%s's turn", playerNames[currentPl] );
    wrefresh(c);
}

bool isGameEnd(){
    return (scores[0] || scores[1] || scores[2] || scores[3]) ? 0 : 1;
}


void mainLoop(int row, int col){
    
    int currentPl = 0;
    WINDOW *p1box = create_newwin(3, 25, row, col * 7);
    WINDOW *p2box = create_newwin(3, 25, row * 8, col * 7);
    nextPlayersTurn(currentPl, p1box, p2box);

    int left = 0, up = 0, current = 0;
 
    int c;
    while ( c != KEY_F(4) && !isGameEnd()){ 
        c = getch();
        switch (c){
            case KEY_LEFT:
                if (left == 1 ){
                    ccol /= COL_DIFF;
                    left = 0;
                    current -= 1;
                }
                break;
            case KEY_RIGHT:
                if (left == 0 ){
                    ccol *= COL_DIFF;
                    left = 1;
                    current += 1;
                }
                break;
            case KEY_UP:
                if (up == 1 ){
                    crow /= ROW_DIFF;
                    up = 0;
                    current -= 2;
                }
                break;
            case KEY_DOWN:
                if (up == 0 ){
                    crow *= ROW_DIFF;
                    up = 1;
                    current += 2;
                }
                break;
            case '\n':
                if (choice != NULL){
                    
                }
                createChoiceBox(row, col);
                if (scores[current] == 0){
                    mvwprintw(choice,1,1,"Please select another heap");
                    wrefresh(choice);
                    break;
                }
                mvwprintw(choice,1,0, "Enter no. of plates to remove\n");
                wrefresh(choice);
                int v, temp; 
                wscanw(choice,"%d",&v);
                wrefresh(choice);
                if (v < 1 || v > 9){
                    v = 9;
                }
                temp = scores[current];
                scores[current] -= v;
                if (scores[current] < 0){
                    scores[current] = 0;
                }
                wprintw(choice, "You removed %d\n", temp-scores[current]);
                currentPl = 1 - currentPl;
                nextPlayersTurn(currentPl, p1box, p2box);
                wrefresh(choice);
                refresh();
        }
        deleteBoard();
        printBox(row, col);
    }
    int winner = 1 - currentPl;

    if (c != KEY_F(1)){
        mvwprintw(choice, 1, 0, "The winner is %s\n\n",playerNames[winner]);
        wprintw(choice, "Do you want to play again (y/n):");
        wrefresh(choice);
        char c;
        wscanw(choice, "%c", &c);

    }
    endwin();
    free(scores);
    free(boxes);
    if (c == 'y' || c == 'Y'){
       // main(0, NULL);
    }
}

void programinfo(){
    printf("Nim v1\n");
    printf("Copyright (C) 2011, Shailesh Ghimire <shailesh.ghimire10@imperial.ac.uk>\n");
    printf("This is free software, covered by the GNU General Public License.\n");
    printf("There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
}

int main(int argc, char **argv){
    programinfo();
    printf("\nEnter P1 name: ");
    char *p1 = ( char *) malloc (20);
    nullCheck(p1);
    fgets(p1, 20, stdin);
    p1[strlen(p1) - 1] = 0;
    if (strlen(p1) == 0){
        p1 = "Player 1";
    }

    printf("Enter P2 name: ");
    char *p2 = (char *) malloc (20);
    nullCheck(p2);
    fgets(p2, 20, stdin);
    p2[strlen(p2) - 1] = 0;
    if (strlen(p2) == 0){
        p2 = "Player 2";
    }

    playerNames = (char **) malloc (2 * sizeof(p1));
    nullCheck(playerNames);
    playerNames[0] = p1;
    playerNames[1] = p2;

    printf("\n%s VS %s\n", p1, p2);
    printf("\nPress any key to continue...");
    getchar();
    initscr();
    keypad(stdscr, TRUE);
    curs_set(0);
    refresh();

    scores = (int *) malloc (4 * sizeof(int));
    nullCheck(scores);

    boxes = (WINDOW **) malloc (4 * sizeof(WINDOW *));
    nullCheck(boxes);
    
    srand(time(NULL));

    do{
        scores[0] = rand() % 10;
        scores[1] = rand() % 10;
        scores[2] = rand() % 10;
        scores[3] = rand() % 10;
    }while(isGameEnd());
    
    int row, col;
    getmaxyx(stdscr, row, col);
    row = row / 10;
    col = col / 10;
    crow = row;
    ccol = col;

    WINDOW *title = create_newwin(3,10, 0, 0);
    mvwprintw(title, 1, 1, "Nim V1");
    wrefresh(title);

    WINDOW *footer = create_newwin(3,14, (row*10), (col*10) - 7);
    mvwprintw(footer, 1, 1, "By Shailesh");
    wrefresh(footer);
    
    printBox(row, col); 
    mainLoop(row,col);
         
    return 0;
}
