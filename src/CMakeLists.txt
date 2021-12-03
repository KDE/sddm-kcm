set(SDDM_CONFIG_FILE                 "${CMAKE_INSTALL_FULL_SYSCONFDIR}/sddm.conf"        CACHE PATH      "Path of the sddm config file")
set(SDDM_CONFIG_DIR                  "${CMAKE_INSTALL_FULL_SYSCONFDIR}/sddm.conf.d"      CACHE PATH      "Path of the sddm config directory")
set(SDDM_SYSTEM_CONFIG_DIR           "${CMAKE_INSTALL_PREFIX}/lib/sddm/sddm.conf.d"      CACHE PATH      "Path of the system sddm config directory")
set(XSESSIONS_DIR                    "${CMAKE_INSTALL_PREFIX}/share/xsessions"           CACHE PATH      "Path of the xsessions")
set(WAYLAND_SESSIONS_DIR             "${CMAKE_INSTALL_PREFIX}/share/wayland-sessions"    CACHE PATH      "Path of the wayland sessions")

configure_file(config.h.in config.h IMMEDIATE @ONLY)

set(SDDM_KCM_SRCS
    sddmkcm.cpp
    themesmodel.cpp
    thememetadata.cpp
    usersmodel.cpp
    sessionmodel.cpp
    sddmdata.cpp
    sddmsettingsbase.cpp
)


add_library(kcm_sddm MODULE ${SDDM_KCM_SRCS})

kconfig_add_kcfg_files(kcm_sddm sddmsettings.kcfgc GENERATE_MOC)

target_compile_definitions(kcm_sddm PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

target_link_libraries(kcm_sddm
    KF5::I18n
    KF5::AuthCore
    KF5::KCMUtils
    KF5::NewStuff
    KF5::QuickAddons
)

kpackage_install_package(package kcm_sddm kcms)


install(TARGETS kcm_sddm DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/kcms/systemsettings)
