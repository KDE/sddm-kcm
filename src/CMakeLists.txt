set(SDDM_CONFIG_FILE                 "${CMAKE_INSTALL_FULL_SYSCONFDIR}/sddm.conf"        CACHE PATH      "Path of the sddm config file")
set(SDDM_CONFIG_DIR                  "${CMAKE_INSTALL_FULL_SYSCONFDIR}/sddm.conf.d"      CACHE PATH      "Path of the sddm config directory")
set(SDDM_SYSTEM_CONFIG_DIR           "${CMAKE_INSTALL_PREFIX}/lib/sddm/sddm.conf.d"      CACHE PATH      "Path of the system sddm config directory")
set(XSESSIONS_DIR                    "${CMAKE_INSTALL_PREFIX}/share/xsessions"           CACHE PATH      "Path of the xsessions")
set(WAYLAND_SESSIONS_DIR             "${CMAKE_INSTALL_PREFIX}/share/wayland-sessions"    CACHE PATH      "Path of the wayland sessions")

configure_file(config.h.in config.h IMMEDIATE @ONLY)
# add_subdirectory(configwidgets)

include_directories(configwidgets)

set(SDDM_KCM_SRCS
    sddmkcm.cpp
    themeconfig.cpp
    themesmodel.cpp
    thememetadata.cpp
    themesdelegate.cpp
    advancedconfig.cpp
    usersmodel.cpp
    sessionmodel.cpp
    configwidgets/selectimagebutton.cpp
)

set(SDDM_KCM_UI
    ui/themeconfig.ui
    ui/advancedconfig.ui)

ki18n_wrap_ui(SDDM_KCM_SRCS ${SDDM_KCM_UI})
add_library(kcm_sddm MODULE ${SDDM_KCM_SRCS})
target_compile_definitions(kcm_sddm PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

target_link_libraries(kcm_sddm
    Qt5::Widgets
    Qt5::X11Extras
    Qt5::Quick
    Qt5::QuickWidgets
    KF5::I18n
    KF5::ConfigWidgets
    KF5::AuthCore
    KF5::KIOWidgets
    KF5::NewStuff
)

install(TARGETS kcm_sddm DESTINATION ${CMAKE_INSTALL_PLUGINDIR})

install(FILES qml/main.qml DESTINATION ${CMAKE_INSTALL_DATADIR}/sddm-kcm)


