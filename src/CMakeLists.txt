set(SDDM_CONFIG_FILE         "/etc/sddm.conf"             CACHE PATH      "Path of the sddm config file")
    
configure_file(config.h.in config.h IMMEDIATE @ONLY)
# add_subdirectory(configwidgets)

include_directories(configwidgets)

set(SDDM_KCM_SRCS
    sddmkcm.cpp
    themeconfig.cpp
    themesmodel.cpp
    thememetadata.cpp
    themesdelegate.cpp
    advanceconfig.cpp
    usersmodel.cpp
    cursortheme/thememodel.cpp
    cursortheme/xcursortheme.cpp
    cursortheme/cursortheme.cpp
    cursortheme/sortproxymodel.cpp
    cursortheme/dummytheme.cpp

    #temporary until we rebuild configwidgets
    configwidgets/selectimagebutton.cpp
)

set(SDDM_KCM_UI
    ui/themeconfig.ui
    ui/advanceconfig.ui)

qt5_wrap_ui(SDDM_KCM_SRCS ${SDDM_KCM_UI})
kde4_add_plugin(kcm_sddm ${SDDM_KCM_SRCS})

target_link_libraries(kcm_sddm
    Qt5::Widgets
    Qt5::X11Extras
    Qt5::Quick
    Qt5::QuickWidgets
    KF5::KDELibs4Support
#     sddm_config_widgets

    ${X11_LIBRARIES}
    XCB::XCB # For mouse cursor themes
)

if (X11_Xcursor_FOUND)
   target_link_libraries(kcm_sddm ${X11_Xcursor_LIB})
endif (X11_Xcursor_FOUND)
if (X11_Xfixes_FOUND)
   target_link_libraries(kcm_sddm ${X11_Xfixes_LIB})
endif (X11_Xfixes_FOUND)

install(TARGETS kcm_sddm DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES qml/main.qml DESTINATION ${DATA_INSTALL_DIR}/sddm-kcm)


