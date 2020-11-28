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

Thank BlackWalnut Labs for providing the ESP32-WROOM-32 Development Board. It contains a mirophone (Inter-IC Sound), WS2812 LED light, GY-25Z Gyroscope and a button from top to bottom.

On the basis of the instructions in the BlackWalnut Lab tutorial, I changed part of the program code to adapt to the latest situation.

The program runs on arduino with the following library installed: 

* https://github.com/espressif/arduino-esp32

* https://github.com/adafruit/Adafruit_NeoPixel

* https://github.com/tanakamasayuki/Arduino_TensorFlowLite_ESP32

* https://github.com/Links2004/arduinoWebSockets

* https://github.com/bblanchon/ArduinoJson

* https://github.com/kosme/arduinoFFT

The Programs uses 115200 baud to send Serial data.

# TinyML ESP32

此仓库是黑胡桃实验室的TinyML教程中的程序集合 https://blackwalnut.zucc.edu.cn/codelabs/TinyML

感谢黑胡桃实验室提供的ESP32-WROOM-32开发板。它从上到下包括一个麦克风(Inter-IC Sound)，WS2812 LED灯，GY-25Z陀螺仪和一个按钮。

程序在黑胡桃实验室教程中所述说明的基础上，改动了部分代码以适应最新情况。

程序可以在安装了以下库的arduino上运行：

* https://github.com/espressif/arduino-esp32

* https://github.com/adafruit/Adafruit_NeoPixel

* https://github.com/tanakamasayuki/Arduino_TensorFlowLite_ESP32

* https://github.com/Links2004/arduinoWebSockets

* https://github.com/bblanchon/ArduinoJson

* https://github.com/kosme/arduinoFFT

程序使用115200波特发送串行数据。

# Details about the board 开发板细节

![image](https://user-images.githubusercontent.com/43995067/91662908-f840e880-eb17-11ea-8cd0-452fa56a5634.png)

![image](https://user-images.githubusercontent.com/43995067/91662915-00992380-eb18-11ea-8c48-5b74fe9ca23e.png)

# Content 目录

[Project I : Gesture Recognition 姿态识别](https://www.bilibili.com/video/BV1Xg4y1q7EF)
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
* [Visualize Predict Gesture 可视化预测手势](P1%20Gesture%20Recognition/visualize_predict_gesture/visualize_predict_gesture.ino)
  * 预测手势，将2种手势可能性通过网页客户端进行显示
  * Predict the gesture and show the two gesture possibilities through web pages.

[Project II : Open LED using Voice 热词唤醒](https://www.bilibili.com/video/BV1Uk4y1m7uy)
* [Audio Data Acquisition 获取声音数据](P2%20Open%20LED%20using%20Voice/audio_data_acquisition/audio_data_acquisition.ino)
  * 将麦克风的数据通过串口发送
  * The data of microphone is sent through serial port
* [Record 2s Audio Data 获取2秒声音数据](P2%20Open%20LED%20using%20Voice/Get%20Recording/record_2s_audio_data/record_2s_audio_data.ino)
  * 开启时将麦克风的2秒录音数据通过串口发送
  * When it is turned on, the 2 second recording data of the microphone is sent through the serial port
* [Record Audio Data 录音获取声音数据](P2%20Open%20LED%20using%20Voice/Get%20Recording/record_audio_data/record_audio_data.ino)
  * 开启时将麦克风的录音数据显示在网页界面
  * When on, recording data of microphone will be displayed on the web page
* [Get audio and visualize waveform 获取音频，可视化波形](P2%20Open%20LED%20using%20Voice/Get%20Recording/recorder.ipynb)
  * 使用Jupyter Notebook获取音频并可视化波形
  * Use Jupyter Notebook to get audio and visualize waveform
* [FFT Visualize Realtime FFT可视化数据](P2%20Open%20LED%20using%20Voice/FFT_visualize_realtime/FFT_visualize_realtime.ino)
  * 将实时频域数据显示在网页界面
  * Display real-time frequency domain data in web pages
* [Get Training Data 获取训练数据](P2%20Open%20LED%20using%20Voice/get_training_data/get_training_data.ino)
  * 收集同一语音的100次重复操作，达到100次时亮绿灯，超过亮蓝灯，并将csv格式的数据内容通过串口发送
  * Collect 100 repeated operations of the same voice, and when it reaches 100 times, the green light will be on. The blue light will be on when the operations exceed 100 times, and the data in the format of CSV will be sent through the serial port
* [Train Voice Model 训练语音模型](P2%20Open%20LED%20using%20Voice/Train%20Voice%20Model/app.ipynb)
  * 使用Jupyter Notebook训练模型
  * Using jupyter notebook to train model
* [Open LED using Voice 热词唤醒](P2%20Open%20LED%20using%20Voice/open_led_using_voice/open_led_using_voice.ino)
  * 预测语音，根据语音指示开关LED灯
  * Predict voice, switch LED light according to voice content

[Project III : Count rope jumping 跳绳计数器](https://blackwalnut.zucc.edu.cn/codelabs/detail?id=5fc1421a714e390022c55345)
* [Get UART Data 获取UART数据](P3%20Count%20rope%20jumping/get_UART_data/get_UART_data.ino)
  * 将六轴姿态传感器的数据通过串口发送
  * The data of UART is sent through serial port
* [Get Training Data 获取训练数据](P3%20Count%20rope%20jumping/get_training_data/get_training_data.ino)
  * 收集同一跳绳动作的100次重复操作，工作时亮红灯，达到100次时亮绿灯，并将csv格式的数据内容通过串口发送。同理收集100次其它任意动作的信息。
  * Collect 100 repeated data of jumping actions, red light will be on when working, and when it reaches 100 times, the green light will be on. The data in the format of CSV will be sent through the serial port. Same as above, collect data of 100 ramdom actions.
* [Train Model 训练模型](P3%20Count%20rope%20jumping/Train%20Model/Train.ipynb)
  * 使用Jupyter Notebook训练模型
  * Using jupyter notebook to train model
* [Predict Jumping 预测手势](P3%20Count%20rope%20jumping/use_model_to_predict/use_model_to_predict.ino)
  * 预测手势，将跳绳的可能性通过串口发送
  * Predict the jumping and send the possibility through the serial port
* [Count rope jumping 跳绳计数器](P3%20Count%20rope%20jumping/showing_jump_times/showing_jump_times.ino)
  * 预测跳绳动作手势，将2次数通过网页客户端进行显示
  * Predict the rope-jumping action and show the count times through web pages.
