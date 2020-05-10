# BupApp
backup service 

shiranlupo@shiranlupo-Vostro-3580:~/projects/BupApp/appFiles$

g++ _appClient.cpp _clientMain.cpp utils.cpp mqttConfigs.cpp ../*.cpp ../mqtt/*.h -pthread  -lpaho-mqtt3a -lpaho-mqttpp3 -o BupApp

 g++ _appServer.cpp _serverMain.cpp  mqttConfigs.cpp utils.cpp ../*.cpp ../mqtt/*.h -pthread  -lpaho-mqtt3a -lpaho-mqttpp3 -o BupAppServ

./BupApp [local_ip] [broker_ip]
