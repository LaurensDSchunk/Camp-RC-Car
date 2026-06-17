#ifndef CAR_HEADER_GUARD
#define CAR_HEADER_GUARD

#include <Arduino.h>

class Car {
private:
  enum Command {
    STOP,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
  };

  int leftEnable, rightEnable;
  int leftInput1, leftInput2, rightInput1, rightInput2;

  TaskHandle_t periodicTaskHandle;
  QueueHandle_t commandQueue;

  void leftWheelForward() {
    digitalWrite(leftInput1, HIGH);
    digitalWrite(leftInput2, LOW);
    analogWrite(leftEnable, 255);
  }

  void leftWheelBackward() {
    digitalWrite(leftInput1, LOW);
    digitalWrite(leftInput2, HIGH);
    analogWrite(leftEnable, 255);
  }

  void rightWheelForward() {
    digitalWrite(rightInput1, HIGH);
    digitalWrite(rightInput2, LOW);
    analogWrite(rightEnable, 255);
  }

  void rightWheelBackward() {
    digitalWrite(rightInput1, LOW);
    digitalWrite(rightInput2, HIGH);
    analogWrite(rightEnable, 255);
  }

  void stopWheels() {
    analogWrite(leftEnable, 0);
    analogWrite(rightEnable, 0);
  }
  
  static void periodicTask(void* voidObj) {
    Car& car = *static_cast<Car*>(voidObj);
    Command recvCommand;

    while (true) {
      delay(10);
      if (xQueueReceive(car.commandQueue, &recvCommand, portMAX_DELAY) == pdPASS) {
        switch (recvCommand) {
          case Command::FORWARD:
            car.leftWheelForward();
            car.rightWheelForward();
            break;
          case Command::BACKWARD:
            car.leftWheelBackward();
            car.rightWheelBackward();
            break;
          case Command::LEFT:
            car.leftWheelForward();
            car.rightWheelBackward();
            break;
          case Command::RIGHT:
            car.leftWheelBackward();
            car.rightWheelForward();
            break;
          case Command::STOP:
            car.stopWheels();
            break;
        }
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

    commandQueue = xQueueCreate(5, sizeof(Command));
    xTaskCreate(periodicTask, 
                "Car Periodic Task", 
                10000, 
                this, 
                1, 
                &periodicTaskHandle);
  }

  void driveForward() {
    Serial.println("Driving Forward");
    Command cmd = Command::FORWARD;
    xQueueSend(commandQueue, &cmd, 0);
  }

  void driveBackward() {
    Serial.println("Driving Backward");
    Command cmd = Command::BACKWARD;
    xQueueSend(commandQueue, &cmd, 0);
  }

  void turnLeft() {
    Serial.println("Turning Left");
    Command cmd = Command::LEFT;
    xQueueSend(commandQueue, &cmd, 0);
  }

  void turnRight() {
    Serial.println("Turning Right");
    Command cmd = Command::RIGHT;
    xQueueSend(commandQueue, &cmd, 0);
  }

  void stop() {
    Serial.println("Stopping");
    Command cmd = Command::STOP;
    xQueueSend(commandQueue, &cmd, 0);
  }
};

#endif