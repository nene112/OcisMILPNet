add_executable(OcisMILPNetBin src/main.cpp)
set_target_properties(OcisMILPNetBin PROPERTIES OUTPUT_NAME OcisMILPNet)
target_compile_options(OcisMILPNetBin PRIVATE /utf-8)
target_link_libraries(OcisMILPNetBin PRIVATE OcisMILPNetDll)
target_compile_definitions(OcisMILPNetBin PRIVATE
    OCIS_PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)
add_custom_command(TARGET OcisMILPNetBin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:OcisMILPNetDll>
            $<TARGET_FILE_DIR:OcisMILPNetBin>
)
if(IS_DIRECTORY "${SCIP_RUNTIME_DIR}")
    add_custom_command(TARGET OcisMILPNetBin POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${SCIP_RUNTIME_DIR}
                $<TARGET_FILE_DIR:OcisMILPNetBin>
    )
endif()

add_test(NAME standalone_sj_sparsity_bin COMMAND OcisMILPNetBin)
set_tests_properties(standalone_sj_sparsity_bin PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
