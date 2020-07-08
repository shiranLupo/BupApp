# BupApp
backup service 

shiranlupo@shiranlupo-Vostro-3580:~/projects/BupApp/appFiles$

g++ _appClient.cpp _clientMain.cpp utils.cpp mqttConfigs.cpp ../*.cpp ../mqtt/*.h -pthread  -lpaho-mqtt3a -lpaho-mqttpp3 -o BupApp

 g++ _appServer.cpp _serverMain.cpp  mqttConfigs.cpp utils.cpp ../*.cpp ../mqtt/*.h -pthread  -lpaho-mqtt3a -lpaho-mqttpp3 -o BupAppServ

./BupApp [local_ip] [broker_ip]

the client sends initialy connection request to the _server
once the server recieve connection req via public chnnle
server set folder and privat chnnle for client and send it the publickey for transform data

clint should wait torecive key to finalize the connenction establishment

retained = true other wise pub key  recievd to client is not stable, no_local= true for both
 

