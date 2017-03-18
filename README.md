# Arduino-JenkinsNotify
RGB Arduino based Jenkins Notifications

![Example gidf](images/example.gif)

# Requirements
## Hardware
[Arduino mega 2560](https://www.amazon.co.uk/dp/B0046AMGW0) or compatible clone
[Ethernet W5100 Network Shield](https://www.amazon.co.uk/dp/B00CQN2FDY/) or compatible clone
IR LED

Supported notification hardware:
* RGB lights driven via a 24 key IR remote
  ![24 button remote](images/arduino_fritzing.png)
* [Pac-Man Plastic Ghost Lamp](https://www.amazon.co.uk/dp/B00IFC2YCC/)

## Jenkins
Install jenkins plugin https://plugins.jenkins.io/notification

## Arduino
Install library dependencies
* ArduinoJson
* IRremote
* [WebServer](https://github.com/sirleech/Webduino) - requires manual install

# Setup
## Hardware
![Arduino fritzing](images/arduino_fritzing.png)

## Jenkins
Configure job with the following settings
* Format:JSON
* PROTOCOL:HTTP
* EVENT:any
* URL:http://${IP}/status

![Jenkins Config](images/jenkins_config.png)

## Arduino
1. Change IP and mac in jenkins_notify.ino
2. Upload sketch to Arduino