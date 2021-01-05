if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/data)
    set(BASE_URL "https://github.com/Fahien/ncJump/releases/download")
    set(JUMP_VERSION "2021.01")
    set(FILE_NAME "data.tar.gz")
    set(URL "${BASE_URL}/${JUMP_VERSION}/${FILE_NAME}")
    set(OUT "${CMAKE_CURRENT_SOURCE_DIR}/${FILE_NAME}")
    file(DOWNLOAD ${URL} ${OUT})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf ${FILE_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    file(REMOVE ${OUT})
    if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/data)
        message(FATAL_ERROR "Failed to download and unpack ${FILE_NAME}")
    endif()
endif()
