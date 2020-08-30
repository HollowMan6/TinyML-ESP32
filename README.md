# TinyML ESP32

[![last-commit](https://img.shields.io/github/last-commit/HollowMan6/TinyML-ESP32)](../../graphs/commit-activity)

[![Followers](https://img.shields.io/github/followers/HollowMan6?style=social)](https://github.com/HollowMan6?tab=followers)
[![watchers](https://img.shields.io/github/watchers/HollowMan6/TinyML-ESP32?style=social)](../../watchers)
[![stars](https://img.shields.io/github/stars/HollowMan6/TinyML-ESP32?style=social)](../../stargazers)
[![forks](https://img.shields.io/github/forks/HollowMan6/TinyML-ESP32?style=social)](../../network/members)

[![Open Source Love](https://img.shields.io/badge/-%E2%9D%A4%20Open%20Source-Green?style=flat-square&logo=Github&logoColor=white&link=https://hollowman6.github.io/fund.html)](https://hollowman6.github.io/fund.html)
[![GPL Licence](https://img.shields.io/badge/license-GPL-blue)](https://opensource.org/licenses/GPL-3.0/)
[![Repo-Size](https://img.shields.io/github/repo-size/HollowMan6/TinyML-ESP32.svg)](../../archive/master.zip)

This is the TinyML programs for ESP32 according to BlackWalnut Labs Tutorials. https://blackwalnut.zucc.edu.cn/codelabs/TinyML

Thank BlackWalnut Labs for providing the ESP32-WROOM-32 Development Board. It contains a mirophone, WS2812 LED light, GY-25Z Gyroscope and a button from top to bottom.

The program runs on arduino with the following library installed: 

* https://github.com/espressif/arduino-esp32

* https://github.com/adafruit/Adafruit_NeoPixel

* https://github.com/tanakamasayuki/Arduino_TensorFlowLite_ESP32

The Programs uses 115200 baud to send Serial data.

# TinyML ESP32

此仓库是黑胡桃实验室的TinyML教程中的程序集合 https://blackwalnut.zucc.edu.cn/codelabs/TinyML

感谢BlackWalnut实验室提供的ESP32-WROOM-32开发板。它从上到下包括一个麦克风，WS2812 LED灯，GY-25Z陀螺仪和一个按钮。

程序可以在安装了以下库的arduino上运行：

* https://github.com/espressif/arduino-esp32

* https://github.com/adafruit/Adafruit_NeoPixel

* https://github.com/tanakamasayuki/Arduino_TensorFlowLite_ESP32

程序使用115200波特发送串行数据。

# Details about the board 开发板细节

![image](https://user-images.githubusercontent.com/43995067/91662908-f840e880-eb17-11ea-8cd0-452fa56a5634.png)

![image](https://user-images.githubusercontent.com/43995067/91662915-00992380-eb18-11ea-8c48-5b74fe9ca23e.png)

# Content 目录

[Project I : Gesture Recognition 姿态识别](P1%20Gesture%20Recognition)

* [Get Gyroscope Data 获取陀螺仪数据](P1%20Gesture%20Recognition/get_gyroscope_data/get_gyroscope_data.ino)
  * 将陀螺仪的数据通过串口发送
  * The data of gyroscope is sent through serial port
* [Vibration Induction 振动感应](P1%20Gesture%20Recognition/vibration_induction/vibration_induction.ino)
  * 当检测到运动时，LED灯将会亮起
  * The LED lights up when movement is detected
* [Directional Sensing 方向感应](P1%20Gesture%20Recognition/directional_sensing/directional_sensing.ino)
  * LED灯用不同颜色指示方向
  * LED lights indicate direction with different colors
* [Get Training Data 获取训练数据](P1%20Gesture%20Recognition/get_training_data/get_training_data.ino)
  * 收集同一手势的100次重复操作，达到100次时亮红灯，超过亮绿灯，并将csv格式的数据内容通过串口发送
  * Collect 100 repeated operations of the same gesture, and when it reaches 100 times, the red light will be on. The green light will be on when the operations exceed 100 times, and the data in the format of CSV will be sent through the serial port
* [Train Gesture Model 训练手势模型](P1%20Gesture%20Recognition/Train%20Gesture%20Model/app.ipynb)
  * 使用Jupyter Notebook训练模型
  * Using jupyter notebook to train model
* [Predict Gesture 预测手势](P1%20Gesture%20Recognition/predict_gesture/predict_gesture.ino)
  * 预测手势，将2种手势可能性通过串口发送
  * Predict the gesture and send the two gesture possibilities through the serial port
