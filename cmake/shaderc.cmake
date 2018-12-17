

function(shaderc _SHADER_TITLE)

    cmake_parse_arguments(_SHADERC "" "NAME;INPUT;OUTPUT;TYPE" "" ${ARGN})

    if(NOT _SHADERC_INPUT)
        message(FATAL_ERROR "You must provide a shader INPUT")
    elseif(NOT _SHADERC_OUTPUT)
        message(FATAL_ERROR "You must provide a shader OUTPUT.")
    elseif(NOT _SHADERC_TYPE)
        message(FATAL_ERROR "You must provide a TYPE. can be only a choice between 'v' (vertex), 'f' (fragment)")
    endif()

    get_filename_component(_SHADERC_INPUT_DIRECTORY ${_SHADERC_INPUT} DIRECTORY)
    get_filename_component(_SHADERC_INPUT_NAME_WE ${_SHADERC_INPUT} NAME_WE)

    unset(${_SHADER_TITLE})

    if(WIN32)
        add_custom_command(
            OUTPUT ${_SHADERC_OUTPUT}/dx11/${_SHADERC_INPUT_NAME_WE}.bin
            COMMAND bkaradzic::shaderc -f ${_SHADERC_INPUT} -i ${_SHADERC_INPUT_DIRECTORY} -i ${bgfx_DIR}/../../../include/bgfx -o ${_SHADERC_OUTPUT}/dx11/${_SHADERC_INPUT_NAME_WE}.bin --type ${_SHADERC_TYPE} --platform windows -p $<IF:$<STREQUAL:${_SHADERC_TYPE},v>,v,p>s_4_0 -O 3
            DEPENDS ${_SHADERC_INPUT})
        list(APPEND ${_SHADER_TITLE} ${_SHADERC_OUTPUT}/dx11/${_SHADERC_INPUT_NAME_WE}.bin)
    endif()

    add_custom_command(
        OUTPUT ${_SHADERC_OUTPUT}/essl/${_SHADERC_INPUT_NAME_WE}.bin
        COMMAND COMMAND bkaradzic::shaderc -f ${_SHADERC_INPUT} -i ${_SHADERC_INPUT_DIRECTORY} -i ${bgfx_DIR}/../../../include/bgfx -o ${_SHADERC_OUTPUT}/essl/${_SHADERC_INPUT_NAME_WE}.bin  --type ${_SHADERC_TYPE} --platform android
        DEPENDS ${_SHADERC_INPUT})

    add_custom_command(
        OUTPUT ${_SHADERC_OUTPUT}/glsl/${_SHADERC_INPUT_NAME_WE}.bin
        COMMAND bkaradzic::shaderc -f ${_SHADERC_INPUT} -i ${_SHADERC_INPUT_DIRECTORY} -i ${bgfx_DIR}/../../../include/bgfx -o ${_SHADERC_OUTPUT}/glsl/${_SHADERC_INPUT_NAME_WE}.bin --type ${_SHADERC_TYPE} --platform linux -p 120
        DEPENDS ${_SHADERC_INPUT})

    add_custom_command(
        OUTPUT ${_SHADERC_OUTPUT}/metal/${_SHADERC_INPUT_NAME_WE}.bin
        COMMAND bkaradzic::shaderc -f ${_SHADERC_INPUT} -i ${_SHADERC_INPUT_DIRECTORY} -i ${bgfx_DIR}/../../../include/bgfx -o ${_SHADERC_OUTPUT}/metal/${_SHADERC_INPUT_NAME_WE}.bin --type ${_SHADERC_TYPE} --platform osx -p metal
        DEPENDS ${_SHADERC_INPUT})

    add_custom_command(
        OUTPUT ${_SHADERC_OUTPUT}/spirv/${_SHADERC_INPUT_NAME_WE}.bin
        COMMAND bkaradzic::shaderc -f ${_SHADERC_INPUT} -i ${_SHADERC_INPUT_DIRECTORY} -i ${bgfx_DIR}/../../../include/bgfx -o ${_SHADERC_OUTPUT}/spirv/${_SHADERC_INPUT_NAME_WE}.bin --type ${_SHADERC_TYPE} --platform linux -p spirv
        DEPENDS ${_SHADERC_INPUT})

    list(APPEND ${_SHADER_TITLE}
        ${_SHADERC_OUTPUT}/essl/${_SHADERC_INPUT_NAME_WE}.bin
        ${_SHADERC_OUTPUT}/glsl/${_SHADERC_INPUT_NAME_WE}.bin
        ${_SHADERC_OUTPUT}/metal/${_SHADERC_INPUT_NAME_WE}.bin
        ${_SHADERC_OUTPUT}/spirv/${_SHADERC_INPUT_NAME_WE}.bin
    )

    set(${_SHADER_TITLE} ${${_SHADER_TITLE}} PARENT_SCOPE)

endfunction()
