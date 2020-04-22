# BupApp
backup service 

~/projects/BupApp/samples$ g++ appServer.cpp ../*.cpp ../mqtt/*.h -pthread  -lpaho-mqtt3a -lpaho-mqttpp3 -o BupAppServer

~/projects/BupApp/samples$ g++ appClient.cpp ../*.cpp ../mqtt/*.h -pthread  -lpaho-mqtt3a -lpaho-mqttpp3 -o BupApp
