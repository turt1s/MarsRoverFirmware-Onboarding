add_executable(tutorial-servo-pwm)
target_sources(tutorial-servo-pwm PRIVATE src/main.cpp)
target_link_libraries(tutorial-servo-pwm PRIVATE mbed-os)
mbed_set_post_build(tutorial-servo-pwm)
