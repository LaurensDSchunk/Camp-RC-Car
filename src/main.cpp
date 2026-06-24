#include "Dashboard.hpp"
#include "Car.hpp"

Car car;
Dashboard dashboard("Wifi Name");

void setup() {
  car.init();
  dashboard.init();

  dashboard.addButton("Forward")
    .onPress([]() {
      car.setThrottle(Throttle::FORWARD);
    })
    .onRelease([]() {
      car.setThrottle(Throttle::NONE);
    })
    .withKeybind("KeyW");

  dashboard.addButton("Reverse")
    .onPress([]() {
      car.setThrottle(Throttle::BACKWARD);
    })
    .onRelease([]() {
      car.setThrottle(Throttle::NONE);
    })
    .withKeybind("KeyS");;

  dashboard.addButton("Turn Left")
    .onPress([]() {
      car.setTurn(Turn::LEFT);
    })
    .onRelease([]() {
      car.setTurn(Turn::NONE);
    })
    .withKeybind("KeyA");;

  dashboard.addButton("Turn Right")
    .onPress([]() {
      car.setTurn(Turn::RIGHT);
    })
    .onRelease([]() {
      car.setTurn(Turn::NONE);
    })
    .withKeybind("KeyD");;
}

void loop() {}