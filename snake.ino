
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

int extensionRow = 0;
int extensionColumn = 0;

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

Position currentExtensiontPosition = Position();
LinkedList<Position> allPossiblePosition = LinkedList<Position>();

// TODO: need to find a good way to randomly add the extention on the board

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

void UpdateExtensionPosition() {
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
  /*int firstIndex = 0;
  int firstIndexValue = indexes.get(firstIndex);
  for (int i = 0; i < 64; i++) {
    Position p = allPossiblePosition.get(i);

    if ((p.row * 8) + p.column != firstIndexValue ) {
      remainingPositions.add(p);
    } else {
      firstIndex += 1;
      firstIndexValue = indexes.get(firstIndex);
    }
  }*/

  int extensionPosition = rand() % remainingPositions.size();

  currentExtensiontPosition = remainingPositions.get(extensionPosition);
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

void VerifyExtension() {
  // If extension eaten, add it, otherwise, do nothing
  if (currentExtensiontPosition.row == snakeRows.get(snakeRows.size() - 1) && currentExtensiontPosition.column == snakeColumns.get(snakeColumns.size() - 1)) {
    
    /*// Add to snake
    snakeRows.add(currentExtensiontPosition.row);
    snakeColumns.add(currentExtensiontPosition.column);*/

    // Add to the end of the snake
    // TODO: will probably need some kind of direction flag, so that it is added at the correct place
    // TODO: Not sure the linked list library provided supports adding an element anywhere ...

    /*int lastRow = snakeRows.get(0);
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

    snakeRows.add(newRow);
    snakeColumns.add(newColumn);*/

    UpdateExtensionPosition();
  }
}

void DisplaySnake() {
  for (int i = 0; i < snakeRows.size(); i++) {
    lc.setLed(0, snakeRows.get(i), snakeColumns.get(i), true);
  }
}

void DisplayExtension() {
  lc.setLed(0, currentExtensiontPosition.row, currentExtensiontPosition.column, true);
}

void setup() {
  Serial.begin(SERIAL_8N1);

  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 6);
  /* and clear the display */
  lc.clearDisplay(0);

  /* Init buttons */
  pinMode(LEFT_BUTTON_PIN, INPUT);
  pinMode(RIGHT_BUTTON_PIN, INPUT);
  pinMode(UP_BUTTON_PIN, INPUT);
  pinMode(DOWN_BUTTON_PIN, INPUT);

  /* Init snake */
  snakeRows.add(0);
  snakeRows.add(0);
  snakeRows.add(0);

  snakeColumns.add(0);
  snakeColumns.add(1);
  snakeColumns.add(2);

  InitAvailablePositions();

  UpdateExtensionPosition();
}

void loop() {
  Serial.println("BOB");
  delay(500);

  ReadControllerInput();

  // TODO: verify collision (what ends the game) (if collision with tail (end of snake), you win!)

  VerifyExtension(); // TODO: better name

  // Update display
  lc.clearDisplay(0);
  DisplaySnake();
  DisplayExtension();
}