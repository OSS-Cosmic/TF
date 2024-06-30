function(tf_add_test target)
add_executable(${target} ${ARGN})
target_link_libraries(${target} TF)
tf_set_output_dir(${target} "Test")
endfunction()

tf_add_test(TF_StringTest TF_StringTest.cpp)
#tf_add_test(TF_StringTest_C TF_StringTest.c)
tf_add_test(TF_ScratchAllocatorTest TF_ScratchAllocatorTest.cpp)
tf_add_test(TF_GPUConfigTest TF_GPUConfigTest.cpp)



