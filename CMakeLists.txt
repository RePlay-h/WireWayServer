cmake_minimum_required(VERSION 3.22.1)

project(server)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED)

add_executable(server 
    main.cpp 

    network/server.cpp 
    network/server.hpp 
    network/session.hpp
    network/user_session.hpp
    network/user_session.cpp
    network/contact_session.hpp
    network/contact_session.cpp

    handlers/handlers.cpp 
    handlers/handlers.hpp 

    log/log.hpp
    log/log.cpp
    )

target_include_directories(server PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(server ${Boost_LIBRARIES})
