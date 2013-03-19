cmake_minimum_required(VERSION 2.8.8)

find_package(KDE4 REQUIRED)
include_directories( 
    ${KDE4_INCLUDES}
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_subdirectory(src)

install(FILES kcm_sddm.desktop DESTINATION ${SERVICES_INSTALL_DIR})

kde4_install_auth_actions(org.kde.kcontrol.kcmsddm kcm_sddm.actions)

kde4_add_executable(kcmsddm_authhelper sddmauthhelper.cpp)
target_link_libraries(kcmsddm_authhelper ${KDE4_KDECORE_LIBS})

kde4_install_auth_helper_files(kcmsddm_authhelper org.kde.kcontrol.kcmsddm root)
install(TARGETS kcmsddm_authhelper DESTINATION ${LIBEXEC_INSTALL_DIR})