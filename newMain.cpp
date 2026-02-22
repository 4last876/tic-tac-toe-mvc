#include <iostream>
#include <cstring>
#include <memory>

constexpr int WIDTH = 3;
constexpr int HEIGHT = 3;
constexpr char CHAR_EMPTY = '@';
enum class TPlayer{X,Y,NULLOPT};


//MODEL
class Board{
char board [WIDTH][HEIGHT];

void resetBoard(){
    for(int h = 0; h < HEIGHT; ++h){
        for(int w = 0; w < WIDTH; ++w){
          board[h][w] = CHAR_EMPTY;
       }
    }
}

public:
Board(){
resetBoard();
}


TPlayer getCell(int y, int x){
    if(y > HEIGHT || y < 0 || x > WIDTH || x < 0 ) exit(11);
    if (board[y][x] == 'X') return TPlayer::X;
    if (board[y][x] == 'Y') return TPlayer::Y;
    return TPlayer::NULLOPT;
}

void setCell(int y, int x,TPlayer type){
   if (y >= HEIGHT || y < 0 || x >= WIDTH || x < 0)  exit(12);
    board[y][x] = (type == TPlayer::X ? 'X' : 'Y');
}

char (*getBoard())[HEIGHT]{
char (*copy_board)[HEIGHT] = new char[WIDTH][HEIGHT];
memcpy(copy_board,board,sizeof(board));
return copy_board;
}

};

class Player{
int score;
TPlayer tplayer;
public:
Player(TPlayer tplayer_) : tplayer(tplayer_){}

const int& getScore(){
    return score;
}

const TPlayer& getTPlayer(){
    return tplayer;
}

void reset(){
score = 0;
}

void increase(){
++score;
}
};

class WinChecker{
std::weak_ptr<Board> board;

public:
WinChecker(std::weak_ptr<Board> board_) : board(board_){}

bool is_over(std::weak_ptr<Player> player){
bool res{0};

    for(int y = 0; y < WIDTH; ++y){
      player.lock()->reset();
    for(int x = 0; x < WIDTH; ++x){
      if(board.lock()->getCell(x,y) == player.lock()->getTPlayer()) player.lock()->increase();
      if(player.lock()->getScore() == 3) res = 1;
    }
  }

  for(int y = 0; y < WIDTH; ++y){
      player.lock()->reset();
    for(int x = 0; x < WIDTH; ++x){
      if(board.lock()->getCell(y,x) == player.lock()->getTPlayer()) player.lock()->increase();
      if(player.lock()->getScore() == 3) res = 1;
    }
  }


  if(board.lock()->getCell(0,0)== player.lock()->getTPlayer() && board.lock()->getCell(1,1) == player.lock()->getTPlayer() && board.lock()->getCell(2,2)== player.lock()->getTPlayer() ) res = 1;
  if(board.lock()->getCell(0,2)== player.lock()->getTPlayer() && board.lock()->getCell(1,1) == player.lock()->getTPlayer() && board.lock()->getCell(2,0)== player.lock()->getTPlayer() ) res = 1;
  return res;
}
};

//VIEW

class Displayer {

 public:
 virtual ~Displayer(){}
 virtual void print (char (*board)[HEIGHT]) = 0;
};

class Displayer_console : public Displayer {
public:
void print (char (*board)[HEIGHT]) override{
    for(int h = 0; h < HEIGHT; ++h){
        for(int w = 0; w < WIDTH; ++w){
         std::cout << ' ' << board[h][w];
       }
       std::cout << std::endl;
    }
    delete[] board;
}
};
class InputManager {

  public:
  virtual ~InputManager(){}
  virtual std::pair<int,int> waitMove() = 0;
};

class InputManager_console : public InputManager{
  public:
  std::pair<int,int> waitMove() override{
  int y,x;
  std::cout << "cледущий шаг" << std::endl;
  std::cin >> y >> x;
  return std::make_pair(y,x);
}
};



//CONTROLLER

class Game{
std::shared_ptr<Player> Player1;
std::shared_ptr<Player> Player2;
std::shared_ptr<Player> current_player;
std::shared_ptr<Board> board;
std::shared_ptr<Displayer> display;
std::shared_ptr<InputManager> inputManager;
std::shared_ptr<WinChecker> winChecker;

public:
//SETTERS
void setPlayer1(std::shared_ptr<Player> Player1_){
Player1 = std::move(Player1_);
}

void setPlayer2(std::shared_ptr<Player> Player2_){
Player2 = std::move(Player2_);
}

void setCurrentPlayer(std::shared_ptr<Player> CurrentPlayer_){
current_player = std::move(CurrentPlayer_);
}

void setBoard(std::shared_ptr<Board> Board_){
board = std::move(Board_);
}

void setDisplay(std::shared_ptr<Displayer> Displayer_){
display = std::move(Displayer_);
}

void setInputManager(std::shared_ptr<InputManager> InputManager_){
inputManager = std::move(InputManager_);
}

void setWincheker(std::shared_ptr<WinChecker> WinChecker_){
winChecker = std::move(WinChecker_);
}

Game() = default;

Game(Game&& game){
std::cout << "&&" << std::endl;
Player1 = std::move(game.Player1);
Player2 = std::move(game.Player2);
current_player = std::move(game.current_player);
board = std::move(game.board);
display = std::move(game.display);
inputManager = std::move(game.inputManager);
winChecker = std::move(game.winChecker);
}


void switch_player(){
  if(current_player == Player1){
  current_player = Player2;
  }else if(current_player == Player2){
  current_player = Player1;
  }
}


void run(){
    while(1){
      auto[y,x] = inputManager->waitMove();
      board->setCell(y,x,current_player->getTPlayer());
      if( winChecker->is_over(current_player)) exit(0);
      display->print(board->getBoard());
      switch_player();
  }
}
};

class IGameBuilder{

    public:

    virtual void createPlayer1() = 0;

    virtual void createPlaye2() = 0;

    virtual void createCurrentPlayer() = 0;

    virtual void createBoard() = 0;

    virtual void createDisplay() = 0;

    virtual void createInputManager() = 0;

    virtual void createWincheker() = 0;

    virtual Game&& getGame() = 0;
};

class GameBuilder : public IGameBuilder{
Game game;

std::shared_ptr<Player> Player1 = std::make_shared<Player>(TPlayer::X);
std::shared_ptr<Player> Player2 = std::make_shared<Player>(TPlayer::Y);
std::shared_ptr<Board>  board = std::make_shared<Board>();

    public:

     void createPlayer1() override{
        game.setPlayer1(Player1);
     }

     void createPlaye2() override{
        game.setPlayer2(Player2);
     }

     void createCurrentPlayer() override{
        game.setCurrentPlayer(Player2);
     }

     void createBoard() override{
        game.setBoard(board);
     }

     void createDisplay() override{
        game.setDisplay(std::make_shared<Displayer_console>());
     }

     void createInputManager() override{
        game.setInputManager(std::make_shared<InputManager_console>());
     }

     void createWincheker() override{
       game.setWincheker(std::make_shared<WinChecker>(board));
     }

     Game&& getGame() override {
       return std::move(game);
     }

};

int main(){
IGameBuilder* gameBuildder (new GameBuilder);
gameBuildder->createPlayer1();
gameBuildder->createPlaye2();
gameBuildder->createCurrentPlayer();
gameBuildder->createBoard();
gameBuildder->createWincheker();
gameBuildder->createInputManager();
gameBuildder->createDisplay();
Game game(gameBuildder->getGame());
game.run();

}
