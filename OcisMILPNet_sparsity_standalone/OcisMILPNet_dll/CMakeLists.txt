set(OCIS_CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/edges_utils.cpp
    ${CMAKE_SOURCE_DIR}/src/FileInteraction.cpp
    ${CMAKE_SOURCE_DIR}/src/ocis_scip_utils.cpp
    ${CMAKE_SOURCE_DIR}/src/ocsi_MILP_scip_raii_wrapper.cpp
    ${CMAKE_SOURCE_DIR}/src/TimeConversion.cpp
)

add_library(OcisMILPNetDll SHARED
    src/standalone_api.cpp
    ${OCIS_CORE_SOURCES}
)
set_target_properties(OcisMILPNetDll PROPERTIES OUTPUT_NAME OcisMILPNet)
target_compile_definitions(OcisMILPNetDll PRIVATE OCIS_STANDALONE_EXPORTS)
target_compile_options(OcisMILPNetDll PRIVATE /utf-8)
target_compile_options(OcisMILPNetDll PRIVATE /Gy)
target_link_options(OcisMILPNetDll PRIVATE
    /OPT:REF
)
target_include_directories(OcisMILPNetDll PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/fast_master
    ${CMAKE_SOURCE_DIR}/include
    ${SCIP_INCLUDE_DIR}
)
target_link_libraries(OcisMILPNetDll PRIVATE
    ${SCIP_LIBRARY}
)

if(IS_DIRECTORY "${SCIP_RUNTIME_DIR}")
    add_custom_command(TARGET OcisMILPNetDll POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${SCIP_RUNTIME_DIR}
                $<TARGET_FILE_DIR:OcisMILPNetDll>
    )
endif()
