add_executable(OcisMILPNetDllTest src/main.cpp)
set_target_properties(OcisMILPNetDllTest PROPERTIES OUTPUT_NAME OcisMILPNet_dlltest)
target_compile_options(OcisMILPNetDllTest PRIVATE /utf-8)
target_link_libraries(OcisMILPNetDllTest PRIVATE OcisMILPNetDll)
target_compile_definitions(OcisMILPNetDllTest PRIVATE
    OCIS_PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)
add_custom_command(TARGET OcisMILPNetDllTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:OcisMILPNetDll>
            $<TARGET_FILE_DIR:OcisMILPNetDllTest>
)
if(IS_DIRECTORY "${SCIP_RUNTIME_DIR}")
    add_custom_command(TARGET OcisMILPNetDllTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${SCIP_RUNTIME_DIR}
                $<TARGET_FILE_DIR:OcisMILPNetDllTest>
    )
endif()

add_test(NAME standalone_sj_sparsity_dlltest COMMAND OcisMILPNetDllTest)
set_tests_properties(standalone_sj_sparsity_dlltest PROPERTIES
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
