#include "Dashboard.hpp"
#include "Car.hpp"

Car car;
Dashboard dashboard("Wifi Name");

void setup() {
  car.init();
  dashboard.init();

  dashboard.addButton("Forward")
    .onPress([]() {
      car.driveForward();
    })
    .onRelease([]() {
      car.stop();
    });

  dashboard.addButton("Reverse")
    .onPress([]() {
      car.driveBackward();
    })
    .onRelease([]() {
      car.stop();
    });

  dashboard.addButton("Turn Left")
    .onPress([]() {
      car.turnLeft();
    })
    .onRelease([]() {
      car.stop();
    });

  dashboard.addButton("Turn Right")
    .onPress([]() {
      car.turnRight();
    })
    .onRelease([]() {
      car.stop();
    });
}

void loop() {}