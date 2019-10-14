
#include <LedControl.h>
#include "LinkedList/LinkedList.h"

const int LEFT_BUTTON_PIN = 2;
const int RIGHT_BUTTON_PIN = 3;
const int UP_BUTTON_PIN = 4;
const int DOWN_BUTTON_PIN = 5;

/*
 Now we need a LedControl to work with.
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 Only one MAX72XX.
 */
LedControl lc = LedControl(12, 11, 10, 1);

LinkedList<int> snakeRows = LinkedList<int>();
LinkedList<int> snakeColumns = LinkedList<int>();

class Position {
  public:
    Position() {
      this->row = 0;
      this->column = 0;
    }

    Position(int row, int column) {
      this->row = row;
      this->column = column;  
    }

    int row;
    int column;
};

Position currentFoodPosition = Position();
LinkedList<Position> allPossiblePosition = LinkedList<Position>();

bool hasLost = false;
bool hasWon = false;

void InitAvailablePositions() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      allPossiblePosition.add(Position(i, j));
    }
  }
}

int compare(int *&a, int *&b) {
  return a < b;
}

void UpdateFoodPosition() {
  LinkedList<int> indexes = LinkedList<int>();

  for (int i = 0; i < snakeRows.size(); i++) {
    indexes.add((snakeRows.get(i) * 8) + snakeColumns.get(i)); // row * 8 + column
  }

  indexes.sort(compare);

  Serial.println(indexes.size());

  LinkedList<Position> remainingPositions = LinkedList<Position>();

  int firstIndex = 0;
  int firstIndexValue = indexes.get(firstIndex);

  for (int i = 0; i < 64; i++) {
    if (i != firstIndexValue) {
      Position p = allPossiblePosition.get(i);
      remainingPositions.add(p);
    } else {
      firstIndex += 1;
      firstIndexValue = indexes.get(firstIndex);
    }
  }

  int foodPosition = random(0, 64) % remainingPositions.size();

  currentFoodPosition = remainingPositions.get(foodPosition);
}

void MoveSnake(int rowModification, int columnModification) {
  int newHeadRow = snakeRows.get(snakeRows.size() - 1) + rowModification;;
  int newHeadColumn = snakeColumns.get(snakeColumns.size() - 1) + columnModification;;
  
  // Snake cannot go back on itself
  if(newHeadRow == snakeRows.get(snakeRows.size() - 2) && newHeadColumn == snakeColumns.get(snakeColumns.size() - 2)) {
    return ;
  }
  
  snakeRows.remove(0);
  snakeColumns.remove(0);

  snakeRows.add(newHeadRow);
  snakeColumns.add(newHeadColumn);
}

void ReadControllerInput() {
  if (digitalRead(LEFT_BUTTON_PIN) == HIGH) {
    if (snakeColumns.get(snakeColumns.size() - 1) != 0) {
      MoveSnake(0, -1);
    }
  } else if (digitalRead(RIGHT_BUTTON_PIN) == HIGH) {
    if (snakeColumns.get(snakeColumns.size() - 1) != 7) {
      MoveSnake(0, 1);
    }
  } else if (digitalRead(UP_BUTTON_PIN) == HIGH) {
    if (snakeRows.get(snakeRows.size() - 1) != 0) {
      MoveSnake(-1, 0);
    }
  } else if (digitalRead(DOWN_BUTTON_PIN) == HIGH) {
    if (snakeRows.get(snakeRows.size() - 1) != 7) {
      MoveSnake(1, 0);
    }
  }
}

bool SnakeEatsHimself()
{
  // This is a very trivial way of doing it, it may not be super optimal when the snake is long
  for (int i = 0; i < snakeRows.size(); i++) {
    for (int j = 0; j < snakeRows.size(); j++) {
      if (i != j) {
        if (snakeRows.get(i) == snakeRows.get(j) && snakeColumns.get(i) == snakeColumns.get(j)) {
          return true;
        }
      }
    }
  }
  return false;
}

bool SnakeEatsTail()
{
  // TODO:
  return false;
}

void VerifyFoodEaten() {
  // If food eaten, add it to the end, in the correct direction, otherwise, do nothing
  if (currentFoodPosition.row == snakeRows.get(snakeRows.size() - 1) && currentFoodPosition.column == snakeColumns.get(snakeColumns.size() - 1)) {
    int lastRow = snakeRows.get(0);
    int lastColumn = snakeColumns.get(0);
    int secondRow = snakeRows.get(1);
    int secondColumn = snakeColumns.get(1);

    int newRow = 0;
    int newColumn = 0;
    if (lastRow == secondRow) {
      if (lastColumn > secondColumn) {
        newRow = lastRow;
        newColumn = lastColumn + 1;
      } else {
        newRow = lastRow;
        newColumn = lastColumn - 1;
      }
    } else {
      if (lastRow > secondRow) {
        newRow = lastRow + 1;
        newColumn = lastColumn;
      } else {
        newRow = lastRow - 1;
        newColumn = lastColumn;
      }
    }

    snakeRows.add(0, newRow);
    snakeColumns.add(0, newColumn);

    UpdateFoodPosition();
  }
}

void DisplaySnake() {
  for (int i = 0; i < snakeRows.size(); i++) {
    lc.setLed(0, snakeRows.get(i), snakeColumns.get(i), true);
  }
}

void DisplayFood() {
  lc.setLed(0, currentFoodPosition.row, currentFoodPosition.column, true);
}

void InitSnake() {
  snakeRows.add(0);
  snakeRows.add(0);
  snakeRows.add(0);

  snakeColumns.add(0);
  snakeColumns.add(1);
  snakeColumns.add(2);

  InitAvailablePositions();
  UpdateFoodPosition();
}

void RestartGame() {
  hasLost = false;
  snakeRows.clear();
  snakeColumns.clear();
  allPossiblePosition.clear();
  InitSnake();
}

void DisplayLose() {
  lc.clearDisplay(0);

  lc.setLed(0, 1, 1, true);
  lc.setLed(0, 2, 1, true);
  lc.setLed(0, 3, 1, true);
  lc.setLed(0, 4, 1, true);
  lc.setLed(0, 5, 1, true);
  lc.setLed(0, 6, 1, true);
  
  lc.setLed(0, 1, 2, true);
  lc.setLed(0, 2, 2, true);
  lc.setLed(0, 3, 2, true);
  lc.setLed(0, 4, 2, true);
  lc.setLed(0, 5, 2, true);

  lc.setLed(0, 6, 2, true);
  lc.setLed(0, 6, 3, true);
  lc.setLed(0, 6, 4, true);
  lc.setLed(0, 6, 5, true);
  lc.setLed(0, 6, 6, true);
  
  lc.setLed(0, 5, 3, true);
  lc.setLed(0, 5, 4, true);
  lc.setLed(0, 5, 5, true);
  lc.setLed(0, 5, 6, true);
}

void setup() {
  Serial.begin(SERIAL_8N1);
  
  // Initialize the pseudo-random generator with a non-connected pin (here 0, change if needed)
  // https://www.arduino.cc/reference/en/language/functions/random-numbers/randomseed/
  randomSeed(analogRead(0));
  
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0, false);
  /* Set the brightness to a minimum value */
  lc.setIntensity(0, 1);
  /* and clear the display */
  lc.clearDisplay(0);

  InitSnake();
}

void loop() {

  if (hasLost) {
      DisplayLose();

      if (digitalRead(RIGHT_BUTTON_PIN) == HIGH) {
        RestartGame();
      }
  } else {
    delay(500);

    ReadControllerInput();

    // TODO: add "menu" (i.e. wait for user input to start game)

    // TODO: the snake move forwards every tick
    // TODO: change tick to something more playable (not for debug purposes.)
    // TODO: pacman effect? Or die over the edges?

    if (SnakeEatsHimself())
    {
      hasLost = true;
    }
    if (SnakeEatsTail()) {
      hasWon = true;
      // TODO: add win animation
      // TODO: wait for input and restart game
    }

    VerifyFoodEaten();

    // Update display
    lc.clearDisplay(0);
    DisplaySnake();
    DisplayFood();
  }
}