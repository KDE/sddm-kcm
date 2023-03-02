set(SDDM_CONFIG_FILE                 "/etc/sddm.conf"                                    CACHE PATH      "Path of the sddm config file")
set(SDDM_CONFIG_DIR                  "/etc/sddm.conf.d"                                  CACHE PATH      "Path of the sddm config directory")
set(SDDM_SYSTEM_CONFIG_DIR           "${CMAKE_INSTALL_PREFIX}/lib/sddm/sddm.conf.d"      CACHE PATH      "Path of the system sddm config directory")
set(XSESSIONS_DIR                    "${CMAKE_INSTALL_PREFIX}/share/xsessions"           CACHE PATH      "Path of the xsessions")
set(WAYLAND_SESSIONS_DIR             "${CMAKE_INSTALL_PREFIX}/share/wayland-sessions"    CACHE PATH      "Path of the wayland sessions")

configure_file(config.h.in config.h IMMEDIATE @ONLY)

set(SDDM_KCM_SRCS
    sddmkcm.cpp sddmkcm.h
    themesmodel.cpp themesmodel.h
    thememetadata.cpp thememetadata.h
    usersmodel.cpp usersmodel.h
    sessionmodel.cpp sessionmodel.h
    sddmdata.cpp sddmdata.h
    sddmsettingsbase.cpp sddmsettingsbase.h
)


add_library(kcm_sddm MODULE ${SDDM_KCM_SRCS})

kcmutils_generate_desktop_file(kcm_sddm)
kconfig_add_kcfg_files(kcm_sddm sddmsettings.kcfgc GENERATE_MOC)

target_compile_definitions(kcm_sddm PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

target_link_libraries(kcm_sddm
    KF6::I18n
    KF6::AuthCore
    KF6::KCMUtils
    KF6::KIOGui
    KF6::NewStuff
    KF6::QuickAddons
)

kpackage_install_package(package kcm_sddm kcms)


install(TARGETS kcm_sddm DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/kcms/systemsettings)
