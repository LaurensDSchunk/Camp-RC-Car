#ifndef DASHBOARD_HPP_HEADER_GUARD
#define DASHBOARD_HPP_HEADER_GUARD

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <functional>
#include <WebServer.h>
#include <uri/UriRegex.h>
#include <vector>

#include "html/HTMLAssets.h"

class Dashboard;

class Button {
private:
  friend class Dashboard;

  std::function<void()> pressCallback;
  std::function<void()> releaseCallback;

  std::string keybind = "";
public:
  Button(): pressCallback([](){}), releaseCallback([](){}) {
  }

  Button& onPress(std::function<void()> callback) {
    this->pressCallback = callback;
    return *this;
  }

  Button& onRelease(std::function<void()> callback) {
    this->releaseCallback = callback;
    return *this;
  }

  Button& withKeybind(std::string keyCode) {
    this->keybind = keyCode;
    return *this;
  }
};

class Dashboard {
private:
  std::string name;

  TaskHandle_t periodicTaskHandle;
  WebServer server;

  std::vector<std::pair<std::string, Button>> buttons;

  static void periodicTask(void* voidObj) {
    Dashboard& obj = *static_cast<Dashboard*>(voidObj);

    while (true) {
      delay(10);
      obj.server.handleClient();
    }
  }

  void handleRootRequest() {
    std::string result = INDEX_HTML;

    std::string keyDownListener = "window.addEventListener('keydown', (e) => {\ne.preventDefault()\n";
    std::string keyUpListener = "window.addEventListener('keyup', (e) => {\ne.preventDefault()\n";

    for (const auto& kv : buttons) {
      std::string newButton = "<button onmousedown=\"fetch(encodeURI('button/" + kv.first + "/press'))\" onmouseup=\"fetch(encodeURI('button/" + kv.first + "/release'))\">" + kv.first + "</button>";
      result += newButton;

      keyDownListener += "if (e.code == '" + kv.second.keybind + "') {\nfetch(encodeURI('button/" + kv.first + "/press'))\n}\n";
      keyUpListener += "if (e.code == '" + kv.second.keybind + "') {\nfetch(encodeURI('button/" + kv.first + "/release'))\n}\n";
    }
    
    result += "<script>\n";
    result += keyDownListener + "\n})\n";
    result += keyUpListener + "\n})\n";
    result += "</script>";

    server.send(200, "text/html", result.c_str());
  };

  void handleWildcardRequest() {
    std::string path = server.uri().c_str();
    if (path.find("/button/") == 0) {
      int nextSlash = path.find("/", 8);
      std::string key = server.urlDecode(path.substr(8, nextSlash - 8).c_str()).c_str();
      std::string action = server.urlDecode(path.substr(nextSlash + 1).c_str()).c_str();
      
      Button* buttonPtr = nullptr; 
  
      for (auto& kv : buttons) { 
        if (kv.first == key) {
          buttonPtr = &kv.second; 
          break;
        }
      }

      if (buttonPtr == nullptr) {
        server.send(404, "text/plain", "button not found"); 
        return;
      }

      if (action == "press") {
        buttonPtr->pressCallback();
        server.send(200, "text/plain", "pressed");
      } else if (action == "release") {
        buttonPtr->releaseCallback();
        server.send(200, "text/plain", "released");
      }
    }
  }

public:
  Dashboard(std::string name): name(name) {}

  void init() {
    // Set up the access point and web server
    WiFi.softAP(name.c_str());
    IPAddress ip = WiFi.softAPIP();
    server.begin(80);

    server.on("/", [this](){ handleRootRequest(); });
    server.on(UriRegex("^/button/(.*)"), [this]() { handleWildcardRequest(); });

    xTaskCreate(periodicTask, 
                "RCController Periodic Task", 
                10000, 
                this, 
                2, 
                &periodicTaskHandle);
  }

  Button& addButton(std::string name) {
    buttons.push_back({name, Button()});
    return buttons[buttons.size() - 1].second;
  }
};

#endif