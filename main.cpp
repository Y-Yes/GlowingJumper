#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h> 
#include <termios.h>
#include <vector>
#include <cstdlib>
#include <array>

int currX;
int gameSizex=40;
int gameSizey=8;
int playerPos;
int currY;
int GameSpeed=70;
int GameEasyness=3;
bool MovNeed=false;
bool movL=false;
bool movR=false;
bool movU=false;
bool movD=false;
bool facingLeft=false;
bool facingRight=true;
bool GameEnd=false;
int now;
int emptyspace;
int maxAmmo=9;
int currentAmmo=maxAmmo;
int Score=0;
bool win=false;
using namespace std;

  void PrintColor(string text, string color){
	if(color=="g"){
		color="\033[1;32m";
	}
  else if(color=="r"){
    color="\033[1;31m";
  }
  else if(color=="y"){
    color="\033[1;33m";
  }
  else if(color=="b"){
    color="\033[1;34m";
  }
  else if(color=="m"){
    color="\033[1;35m";
  }
  else if(color=="c"){
    color="\033[1;36m";
  }
  cout<<color<<text<<"\033[97m";
}

void ClearScreen(){
    cout << "\033[2J\033[1;1H";
}

void Draw(){

}


void enableRawMode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ECHO|ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
void disableRawMode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag |= (ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void Movement(){
  enableRawMode(); //https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
  char input;
  while (MovNeed==true) {
        read(STDIN_FILENO, &input, 1); 
        if (input == '\033') { 
            read(STDIN_FILENO, &input, 1);
            read(STDIN_FILENO, &input, 1);
            
          switch (input) {
            case 'A':
                movU=true;
                break;
            case 'B':
                movD=true;
                break;
            case 'C':
                movR=true;
                break;
            case 'D':
                movL=true;
                break;
            }
        }
    }
  disableRawMode();
}

void DrawLine(int *lg){
  int pr=0, pab=0;
  string str, color;
  for(int i=1; i<=gameSizey; i++){
    pr=pab;
    pab+=gameSizex;
    for(int j=pr; j<pab; j++){
      if(lg[j]==0){
        cout<<" ";
      } else if(lg[j]==1&&facingRight==true){
        str="->", color="c";
        PrintColor(str,color);
        j++;
      } else if(lg[j]==1&&facingLeft==true){
        str="<-", color="c";
        PrintColor(str,color);
        j++;
      } else if(lg[j]==2){
        str="+", color="g";
        PrintColor(str,color);
      } else if(lg[j]==3){
        cout<<"=";
      } else if(lg[j]==4){
        str="!", color="r";
        PrintColor(str,color);
      } else if(lg[j]==5){
        str="@", color="y";
        PrintColor(str,color);
      }
    }
    cout<<endl;
  }
}
void ShootFireball(int *lg){
  if(lg[playerPos+2]!=5&&lg[playerPos+1]!=5){//prevent shooting in walls
    lg[playerPos+2]=2;
    facingRight=true;
  }
}
void UpdateFireball(int *lg){
  int fireballX;
  int pr=0, pab=0;
  int currpos=-1;
  for(int i=1; i<=gameSizey; i++){
    pr=pab;
    pab+=gameSizex;
    for(int j=pr; j<pab; j++){
      currpos++;
      if(lg[currpos]==2){
        fireballX=j-pr;
        if(fireballX==0||fireballX==gameSizex-1){
          lg[currpos]=0;
        } else if(lg[currpos+1]==5){
          lg[currpos]=0;
        }else if(lg[currpos+1]==4){
          lg[currpos]=0;
          lg[currpos+1]=0;
        } else{
          lg[currpos]=0;
          lg[currpos+1]=2;
          j++;
          currpos++;
        }
      }
    } 
  } 
}
void GenerateEnemies(int *lg){
  int odds;
  bool g;
  odds = rand() % GameEasyness + 1;
  if(odds>=2){
    if(lg[gameSizex*(gameSizey-1)-2]==5){
      lg[gameSizex*(gameSizey-2)-1]=4;
      g=false;
    }
    if(g){
      lg[gameSizex*(gameSizey-1)-1]=4;
    }
  }
}
void BuildUpdateWall(int *lg){ 
  int odds, odds2;
  emptyspace=0;
  for(int i=gameSizex*gameSizey-1; i>gameSizex*(gameSizey-1); i--){
    if(lg[i]==5){
      break;
    }
    emptyspace++;
  }
  int luck=(10-emptyspace/4)*2;
  odds = rand() % luck + 1;
  odds2 = rand() % 4 + 1;
  int wallX;
  int pr=0, pab=0;
  int currpos=-1;
  for(int i=1; i<=gameSizey; i++){
    pr=pab;
    pab+=gameSizex;
    for(int j=pr; j<pab; j++){
      currpos++;
      if(lg[currpos]==5){
        wallX=j-pr;

        if(wallX==0||wallX==gameSizex){
          lg[currpos]=0;
        } else if(lg[currpos-2]==1){
          lg[currpos]=0;
          lg[currpos-1]=5;
          lg[currpos-2]=0;
          lg[currpos-3]=1;
        } else{
          lg[currpos]=0;
          lg[currpos-1]=5;
        }
      }
    }
  }
  if(lg[gameSizex*gameSizey-2]==5){ //wall generation
    if(lg[gameSizex*gameSizey-3]==0&&odds2<=3){
      lg[gameSizex*gameSizey-1]=5;//##
      GenerateEnemies(lg);
    } if(odds2>2&&lg[gameSizex*(gameSizey-1)-2]!=4){
        lg[gameSizex*(gameSizey-1)-2]=5; //#^
      GenerateEnemies(lg);
    }
  }
  if(odds<=4){
    lg[gameSizex*gameSizey-1]=5; 
  }
}


void UpdateEnemies(int *lg){
  int enemyX;
  int pr=0, pab=0;
  int currpos=-1;
  for(int i=1; i<=gameSizey; i++){
    pr=pab;
    pab+=gameSizex;
    for(int j=pr; j<pab; j++){
      currpos++;
      if(lg[currpos]==4){
        enemyX=j-pr;
        if(enemyX==0){
          lg[currpos]=0;
        } else if(lg[currpos-1]==2){
          lg[currpos]=0;
          lg[currpos-1]=0;
        } else{
          lg[currpos]=0;
          lg[currpos-1]=4;
        }
      }
    }
  }
}

void JumpEnemies(int *lg){ 
  int enemyX, enemyY, enemyHeight=-1, ignoreList[gameSizex];//ignore list first integer tells how many entries are in the array and the others are X coordinates of the enemies. It is a dum way to manage this but Ain't nobody got time for that.
  int odds = rand() % 4 + 1;
  ignoreList[0]=0;
  int pr=0, pab=0;
  int currpos=-1;
  bool breaker=false;
  for(int i=1; i<=gameSizey; i++){
    pr=pab;
    pab+=gameSizex;
    for(int j=pr; j<pab; j++){
      currpos++;
      if(lg[currpos]==4){
        enemyX=j-pr;
        enemyY=gameSizey-i+1;
      if(lg[currpos-gameSizex]==4){
        lg[currpos-gameSizex]=0;
      }
        }
        for(int i=1; i<=6;i++){
          if(lg[currpos+gameSizex*i]==5){
            enemyHeight=i;
            break;
          }
        }
      if(lg[currpos]==4){
        if(enemyHeight==1&&odds==1){
          lg[currpos]=0;
          if(lg[currpos-gameSizex]==1){
            GameEnd=true;
          }
          lg[currpos-gameSizex]=6;
        }
        else if(enemyHeight==2){
          if(odds>2){
            lg[currpos]=0;
            if(lg[currpos-gameSizex]==1){
              GameEnd=true;
            }
            lg[currpos-gameSizex]=6;
          }else{
            if(lg[currpos+gameSizex]==1){
              GameEnd=true;
            }
            lg[currpos]=0;
            lg[currpos+gameSizex]=6;
          }
        }
        else if(enemyHeight==3){
            if(lg[currpos+gameSizex]==1){
            GameEnd=true;
          }
            lg[currpos]=0;
            lg[currpos+gameSizex]=6;
          }
        }
      if(lg[currpos]==4){
     // cout<<enemyHeight<<endl;
    //this_thread::sleep_for(chrono::milliseconds(300));
      }
      odds = rand() % 4 + 1;
      }
    }
    for (int i=0; i<gameSizex*gameSizey; i++){
      if(lg[i]==6){
        lg[i]=4;//removes the don't touch me flag after the moving is done so that the enemies couldn't be moved twice in the same game tick.
      }
    }
}
//01234
//56789
//01234
//56789
//01234
void movePlayer(int *l1, int y){
int x;
  
  for(int i=0; i<gameSizex*gameSizey; i++){
    if(l1[i]==1){
      x=i;
      playerPos=i;
    }
  }
  
  int pr=0, pab=0;
  for(int i=1; i<=gameSizey; i++){
    pr=pab;
    pab+=gameSizex;
    for(int j=pr; j<pab; j++){
      if(l1[j]==1){
        currY=gameSizey-i+1;
        currX=j-pr;
      }
    } 
  }
  if(currY>5){
  GameEnd=true;
  }
  if(currY>y&&l1[x+gameSizex]!=5&&l1[x+gameSizex+1]!=5){
      if(l1[x+gameSizex]==4){
      GameEnd=true;
    }
    
    l1[x]=0;
    l1[x+gameSizex]=1;
    playerPos+=gameSizex;
  }
  else if(currY<y){
      if(l1[x-gameSizex]==4){
      GameEnd=true;
    }
      l1[x]=0;
      l1[x-gameSizex]=1;
      playerPos-=gameSizex;
  }
  for(int i=0; i<gameSizex*gameSizey; i++){
    if(l1[i]==1){
      playerPos=i;
    }
  }
  if(movR==true){
    movR=false;
    if(currX!=gameSizex-3&&l1[playerPos+1]==0&&l1[playerPos+2]==0){
      l1[playerPos]=0;
      l1[playerPos+1]=1;
      facingRight=true;
      facingLeft=false;
    }
  }

  else if(movL==true){
    movL=false;
    if(currX!=0&&l1[playerPos-1]==0){
      l1[playerPos]=0;
      l1[playerPos-1]=1;
      facingLeft=true;
      facingRight=false;
    }
  }
}
void IsPlayerStillWithUsChecker(int *lg){
  bool playerhere=false;
  for(int i=0; i<gameSizex*gameSizey-1; i++){
    if(lg[i]==1){
      playerhere=true;
    }
  }
  if(playerhere==false){
    GameEnd=true;
  }
}
void UpdateGame(int *l1, int &velocity, int &y, bool &canJump, bool &canShoot){
  Score+=10;
  if(Score>=40000+gameSizex*20){
    GameEnd=true;
    win=true;
  }
  else if(Score>=40000){
  
  if(velocity==0&&y!=1){
    y-=1;
  } else if(velocity>1){
    y+=1;
    velocity-=1;
  } else if(velocity==1){
    velocity-=1;
  }
  if(y==1){
    canJump=true;
  } 
  movePlayer(l1, y);
  }
  else{
  if(movU==true&&canJump==true){
    movU=false;
    velocity+=5;
    canJump=false;
  } else{
    movU=false;
  }

//  UpdateEnemies(l1);
//  GenerateEnemies(l1);
  UpdateFireball(l1);
  if(now>=2){
    now=0;
    BuildUpdateWall(l1);
    UpdateEnemies(l1);
    JumpEnemies(l1);
    if(currentAmmo<maxAmmo){
      currentAmmo++;
    }
  } else{
    now++;
  }
  IsPlayerStillWithUsChecker(l1);
 if(movD==true&&currentAmmo>0){
    ShootFireball(l1);
   canShoot=true;
  movD=false;
  currentAmmo=currentAmmo-2;
 }
  
  if(velocity==0&&y!=1){
    y-=1;
  } else if(velocity>1){
    y+=1;
    velocity-=1;
  } else if(velocity==1){
    velocity-=1;
  }
  if(y==1){
    canJump=true;
  } 
  movePlayer(l1, y);
  }
  Score+=10;
}
void DrawGame(){
  int l1[gameSizex*gameSizey-1];
  int velocity=0;
  int y=1;
  bool canJump=true;
  bool canShoot=true;
//  Draw();
  l1[gameSizex*gameSizey-gameSizex]=1;
  this_thread::sleep_for(chrono::milliseconds(3000));
  string color, text;
  while(GameEnd!=true){
    ClearScreen();
    UpdateGame(l1,velocity, y, canJump, canShoot);
    color="b";
    text="########################################";
    //cout<<"1234567890123456789012345678901234567890"<<endl;
    PrintColor(text,color);
    DrawLine(l1);
    PrintColor(text,color);
    cout<<endl;
    cout<<"Score="<<Score<<endl<<endl;;
    //cout<<"y="<<y<<endl;
    //cout<<"canjump="<<canJump<<endl;
    //cout<<"movU="<<movU<<endl;
    //cout<<"second block"<<l1[1]<<endl;
    //cout<<"player pos:"<<playerPos<<endl;
    //cout<<"Curry="<<currY<<endl;
    //cout<<"Currx="<<currX<<endl;
    //cout<<"Emptyspace="<<emptyspace<<endl;
    cout<<endl;
    
    this_thread::sleep_for(chrono::milliseconds(GameSpeed));
  }
ClearScreen();
}
int main() {
  string choice;
  cout<<"Welcome to the game!"<<endl<<"Press Enter to continue"<<endl;
  getline(cin,choice);
  ClearScreen();
  cout<<R"(      __   			->
      \ \  		This is Bob and you are him.
  _____\ \              You can move Bob using the arrow keys.
 |______> >		Fun Fact:
       / /		Pressing down shoots a fireball to the right of Bob.
      /_/               Press Enter to continue.
  )";
  getline(cin,choice);
  ClearScreen();
  cout<<R"(
    ____  
   / __ \                 @
  / / _` |    This is the ground.
 | | (_| |    Fun Fact:
  \ \__,_|    You can stand on it, but your enemies can too.
   \____/ 
  )";
  getline(cin,choice);
  ClearScreen();
  cout<<R"(
  _             !
 | |	 This is Your Enemy.
 | |	 You must shoot and destroy him or he will destroy you.
 | |	 Fun Fact:
 |_|	 They don't really know where to move so they just fly around and land randomly.
 (_)	 
  )";
  getline(cin,choice);
  ClearScreen();
  srand (time(NULL));
  MovNeed=true;
  thread t1 (Movement);  
  thread t2 (DrawGame);

  
  
 // t1.join();
  t2.join();
  MovNeed=false;
	if(win==true){
	cout<<"You win!"<<endl;
  t1.join();
	return 0;
	}
  t1.join();
  cout<<"RIP"<<endl;
  this_thread::sleep_for(chrono::milliseconds(1000));
  ClearScreen();
  return main();
//-> ===========+    !!
//  ##               ##
//  ##              ####
//#############################
  return 0;
}
