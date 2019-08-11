/*
 * One Button Keyboard by TFF
 * https://www.one-button-key.com/
 */
#include <Keyboard.h>

#define PIN_KEYSW (9)

#undef _LONG_HOLD_LOOP_   // 長押し時に連続実行する場合は#defineする

const int KEY_DOWN = LOW;
const int KEY_UP = HIGH;

int prevKeyState;
int currKeyState;

unsigned long timePressed;
unsigned long timeReleased;

const unsigned long LONG_HOLD_PERIOD_MS = 1000; // キー長押しと判定する時間
const unsigned long INPUT_END_PERIOD_MS =  500; // キー入力の完了と判定する時間 (> CONT_CLICK_PERIOD_MSにする)
const unsigned long CONT_CLICK_PERIOD_MS = 333; // 連続クリックと判定する時間

int countLongHold;
int countClick;


typedef enum {
  stateInit = 0,
  stateDown,
  stateUp,
  stateInputComplete,
  stateLongHold,
  xstateNum,
} KeyState;

KeyState state;


void setup() {
  pinMode(PIN_KEYSW, INPUT_PULLUP);
  prevKeyState = KEY_UP;
  currKeyState = KEY_UP;

  Keyboard.begin();
}

void loop() {
  currKeyState = digitalRead(PIN_KEYSW);


  // 押した or 離した時刻を取得
  if (isKeyPressed()) {
    timePressed = millis();
  } else if (isKeyReleased()) {
    timeReleased = millis();
  }

  // キーの状態遷移
  switch (state) {
    case stateInit:
      if (isKeyPressed()) {
        countLongHold = 0;
        countClick = 0;
        state = stateDown;
      }
      break;

    case stateDown:
      if (isKeyReleased()) {
        state = stateUp;
      } else if ((countClick == 0) && (millis() - timePressed > LONG_HOLD_PERIOD_MS)) { // 連続クリックの途中では長押し判定しない
        state = stateLongHold;
      }
      break;

    case stateUp:
      if (millis() - timeReleased < CONT_CLICK_PERIOD_MS) { // 連続クリックが有効な期間はキーが押されるか待つ
        if (isKeyPressed()) {
          countClick++;
          state = stateDown;
        }
      } else if (millis() - timeReleased > INPUT_END_PERIOD_MS) {
        state = stateInputComplete;
      }
      break;

    case stateInputComplete:
      if (countClick == 0) {
        actionSingleClick();
      } else if (countClick == 1) {
        actionDoubleClick();
      } else {
        /* error */
      }
      state = stateInit;
      break;
      
    case stateLongHold:
      actionLongHold();
      countLongHold++;
      if (isKeyReleased()) {
        state = stateInit;
      }
      break;

    default:
      break;
  }

  prevKeyState = currKeyState;
  delay(10);
}


bool isKeyPressed() {
  return (prevKeyState == KEY_UP) && (currKeyState == KEY_DOWN);
}

bool isKeyReleased() {
  return (prevKeyState == KEY_DOWN) && (currKeyState == KEY_UP);
}

void waitMs(unsigned long ms) {
  unsigned long t = millis();
  while (millis() - t < ms);
}


void actionSingleClick() {
  Keyboard.print("single");
  Keyboard.press(KEY_RETURN);
  delay(10);
  Keyboard.releaseAll();
}

void actionDoubleClick() {
  Keyboard.print("double");
  Keyboard.press(KEY_RETURN);
  delay(10);
  Keyboard.releaseAll();
}

void actionLongHold() {
#ifndef _LONG_HOLD_LOOP_
  if (countLongHold == 0) {
#endif
  Keyboard.print("long hold");
  Keyboard.press(KEY_RETURN);
  delay(10);
  Keyboard.releaseAll();
#ifndef _LONG_HOLD_LOOP_
  }
#endif
}
