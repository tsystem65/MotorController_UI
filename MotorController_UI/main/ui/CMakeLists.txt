SET(SOURCES screens/ui_Screen1.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    t_system_logo.c)

add_library(ui ${SOURCES})
