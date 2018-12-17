cmake_minimum_required(VERSION 3.12)
project(server)

set(CMAKE_CXX_STANDARD 17)

link_libraries(pthread)

add_executable(server main.cpp
        RpiCommunication.cpp RpiCommunication.h
        exceptions.h
        Packet.cpp Packet.h
        Const.h
        Server.cpp Server.h
        RpiClient.cpp RpiClient.h
        endpoints/Endpoint.cpp endpoints/Endpoint.h
        common/Logger.cpp common/Logger.h endpoints/Light.cpp endpoints/Light.h endpoints/Shade.cpp endpoints/Shade.h Const.cpp Answer.cpp Answer.h common/Atomic.cpp common/Atomic.h Msg.cpp Msg.h endpoints/Thermometer.cpp endpoints/Thermometer.h)