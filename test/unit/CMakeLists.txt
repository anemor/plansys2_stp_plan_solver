ament_add_gtest(stp_test stp_test.cpp)
target_link_libraries(stp_test ${PROJECT_NAME} dl)
