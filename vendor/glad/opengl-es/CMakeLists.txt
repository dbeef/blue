project(glad)

    add_library(glad STATIC
            src/glad.c
            src/glad_egl.c
            include/glad/glad.h
            include/glad/glad_egl.h
            include/glad/khrplatform.h
    )

    target_include_directories(glad PUBLIC include PRIVATE include/glad)

    set_target_properties (glad PROPERTIES
        FOLDER vendor
    )
