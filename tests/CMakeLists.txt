include_directories(../src)

add_executable(test_newton test_newton.cpp  )
target_link_libraries(test_newton libAstree)

add_executable(test_perfect_surface test_perfect_surface.cpp  )
target_link_libraries(test_perfect_surface libAstree)

add_executable(test_scaling test_scaling.cpp  )
target_link_libraries(test_scaling libAstree)