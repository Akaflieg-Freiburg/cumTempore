find_package(Qt6 REQUIRED COMPONENTS Test)

qt_add_executable(tst_countdowntimer
    tst_countdowntimer.cpp
    ${PROJECT_SOURCE_DIR}/src/timer/CountdownTimer.cpp
    ${PROJECT_SOURCE_DIR}/src/timer/CountdownTimer.h
)
target_include_directories(tst_countdowntimer PRIVATE ${PROJECT_SOURCE_DIR}/src/timer)
target_link_libraries(tst_countdowntimer PRIVATE Qt6::Core Qt6::Qml Qt6::Test cumtempore_warnings)

add_test(NAME tst_countdowntimer COMMAND tst_countdowntimer)
