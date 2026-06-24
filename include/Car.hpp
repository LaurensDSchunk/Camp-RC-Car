#ifndef CAR_HPP_HEADER_GUARD
#define CAR_HPP_HEADER_GUARD

#include "Arduino.h"
#include "mutex"

enum class Throttle {
  FORWARD,
  NONE,
  BACKWARD
};
  
enum class Turn {
  LEFT,
  NONE,
  RIGHT
};

class Car {
private:
  int leftEnable, rightEnable;
  int leftInput1, leftInput2, rightInput1, rightInput2;

  TaskHandle_t periodicTaskHandle;
  std::mutex mutex;

  Throttle throttle = Throttle::NONE;
  Turn turn = Turn::NONE;

  static void periodicTask(void* voidObj) {
    Car& car = *static_cast<Car*>(voidObj);

    while (true) {
      delay(10);
      
      int left = 0;
      int right = 0;
      {
        std::lock_guard<std::mutex> guard(car.mutex);
        switch (car.throttle) {
          case Throttle::FORWARD:
            left = 255;
            right = 255;
            break;
          case Throttle::BACKWARD:
            left = -255;
            right = -255;
            break;
          case Throttle::NONE:
            left = 0;
            right = 0;
            break;
        }

        switch (car.turn) {
          case Turn::LEFT:
            left += 255;
            right -= 255;
            break;
          case Turn::RIGHT:
            left -= 255;
            right += 255;
            break;
          case Turn::NONE:
            break;
        }
      }

      // Normalize
      float scaleFactor = 1;
      if (abs(left) > 255) {
        scaleFactor = min(scaleFactor, (255.0f / left));
      }
      if (abs(right) > 255) {
        scaleFactor = min(scaleFactor, (255.0f / right));
      }
      left = (int)(left * scaleFactor);
      right = (int)(right * scaleFactor);

      // Output
      analogWrite(car.leftEnable, abs(left));
      analogWrite(car.rightEnable, abs(right));

      if (left > 0) {
        digitalWrite(car.leftInput1, HIGH);
        digitalWrite(car.leftInput2, LOW);
      } else {
        digitalWrite(car.leftInput1, LOW);
        digitalWrite(car.leftInput2, HIGH);
      }

      if (right > 0) {
        digitalWrite(car.rightInput1, HIGH);
        digitalWrite(car.rightInput2, LOW);
      } else {
        digitalWrite(car.rightInput1, LOW);
        digitalWrite(car.rightInput2, HIGH);
      }
    }
  }

public:
  Car(int leftEnable, 
        int rightEnable, 
        int leftInput1, 
        int leftInput2, 
        int rightInput1, 
        int rightInput2): 
        leftEnable(leftEnable),
        rightEnable(rightEnable),
        leftInput1(leftInput1),
        leftInput2(leftInput2),
        rightInput1(rightInput1),
        rightInput2(rightInput2) {};

  Car(): Car(4, 5, 6, 7, 11, 12) {};

  void init() {
    pinMode(leftEnable, OUTPUT);
    pinMode(rightEnable, OUTPUT);
    pinMode(leftInput1, OUTPUT);
    pinMode(leftInput2, OUTPUT);
    pinMode(rightInput1, OUTPUT);
    pinMode(rightInput2, OUTPUT);

    xTaskCreate(periodicTask, 
                "Car Periodic Task", 
                10000, 
                this, 
                1, 
                &periodicTaskHandle);
  }

  void setThrottle(Throttle throttle) {
    std::lock_guard<std::mutex> guard(mutex);
    this->throttle = throttle;
  }

  void setTurn(Turn turn) {
    std::lock_guard<std::mutex> guard(mutex);
    this->turn = turn;
  }
};

#endif