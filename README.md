# __WireWayServer__

![Static Badge](https://img.shields.io/badge/C%2B%2B-v20-red?labelColor=gray&color=orange) ![Static Badge](https://img.shields.io/badge/boost-v1.82.0-red?labelColor=gray&color=turquoise) 


__WireWayServer__ - this application is part of the WireWay software system, responsible for transmitting packets between users

<br/>
<br/>

## __Build__
---
___Before building, make sure you have the correct language version and boost 

```cmd
mkdir build 
cd build
cmake ..
cmake --build .
```

<br/>
<br/>

## __Specification__
---
With the help of the server, the primary communication between the user and the contact is supported. Its tasks are to process connections of users and contacts and their further interaction using packages.

The server supports the following packages:

+ CONNECT
+ CONNACK 
+ INTERCEPT 
+ INTERCAPT 
+ SCREEN 
+ PUBLISH
+ DISCONNECT

Server uses __boost.asio__ and __STL__

<br/>
<br/>

## __Extra__
---
Description of package functions can be found [___here___](https://github.com/RePlay-h/WireWay-packets)
