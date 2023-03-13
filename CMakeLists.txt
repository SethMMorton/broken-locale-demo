cmake_minimum_required (VERSION 3.1)
project (locale-demo)
add_executable (locale-demo locale-demo.c)
target_compile_features(locale-demo PUBLIC c_std_99)
