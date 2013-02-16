find_package(KDE4 REQUIRED)

include(KDE4Defaults)
include(MacroLibrary)

include_directories(
  ${KDE4_INCLUDES}
)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/cursortheme/ )

set(SDDM_KCM_SRCS
    sddmkcm.cpp
    themeconfig.cpp
    themesmodel.cpp
    thememetadata.cpp
    themesdelegate.cpp
    advanceconfig.cpp
    usersmodel.cpp
    cursortheme/thememodel.cpp
    cursortheme/legacytheme.cpp
    cursortheme/xcursortheme.cpp
    cursortheme/cursortheme.cpp
    cursortheme/sortproxymodel.cpp
)

set(SDDM_KCM_UI
    themeconfig.ui
    advanceconfig.ui)

kde4_add_ui_files(SDDM_KCM_SRCS ${SDDM_KCM_UI})
kde4_add_plugin(kcm_sddm ${SDDM_KCM_SRCS})

target_link_libraries(kcm_sddm
    ${KDE4_KDEUI_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${KDE4_KIO_LIBS}
    ${QT_QTUITOOLS_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${X11_LIBRARIES}
)

if (X11_Xcursor_FOUND)
   target_link_libraries(kcm_sddm ${X11_Xcursor_LIB})
endif (X11_Xcursor_FOUND)
if (X11_Xfixes_FOUND)
   target_link_libraries(kcm_sddm ${X11_Xfixes_LIB})
endif (X11_Xfixes_FOUND)

install(TARGETS kcm_sddm
    DESTINATION ${PLUGIN_INSTALL_DIR}
)

install(FILES kcm_sddm.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
)

install(FILES qml/main.qml
    DESTINATION ${DATA_INSTALL_DIR}/sddm-kcm)

kde4_add_executable(kcmsddm_authhelper sddmauthhelper.cpp)
target_link_libraries(kcmsddm_authhelper ${KDE4_KDECORE_LIBS})

kde4_install_auth_helper_files(kcmsddm_authhelper org.kde.kcontrol.kcmsddm root)
install(TARGETS kcmsddm_authhelper DESTINATION ${LIBEXEC_INSTALL_DIR})

kde4_install_auth_actions(org.kde.kcontrol.kcmsddm kcm_sddm.actions)
