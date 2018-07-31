project(sddm-kcm)
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
set(PROJECT_VERSION "5.13.4")
set(QT_MIN_VERSION "5.4.0")
find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Gui Widgets X11Extras Quick QuickWidgets)

find_package(KF5 REQUIRED COMPONENTS
    CoreAddons
    I18n
    XmlGui
    Auth
    ConfigWidgets
    KIO
    Archive
    NewStuff
)


find_package(X11)
set_package_properties(X11 PROPERTIES DESCRIPTION "X11 libraries"
                        URL "http://www.x.org"
                        TYPE REQUIRED
                        PURPOSE "Required for building the X11 based workspace"
                      )

find_package(XCB
    REQUIRED COMPONENTS
        XCB SHM IMAGE
    OPTIONAL_COMPONENTS
        XKB
)


add_definitions(-DTRANSLATION_DOMAIN=\"kcm_sddm\")
add_definitions(-DQT_NO_URL_CAST_FROM_STRING)

install(FILES kcm_sddm.desktop DESTINATION ${CMAKE_INSTALL_KSERVICES5DIR})

kauth_install_actions(org.kde.kcontrol.kcmsddm kcm_sddm.actions)

add_executable(kcmsddm_authhelper sddmauthhelper.cpp)
target_link_libraries(kcmsddm_authhelper KF5::Auth KF5::ConfigCore KF5::Archive KF5::I18n)

kauth_install_helper_files(kcmsddm_authhelper org.kde.kcontrol.kcmsddm root)
install(TARGETS kcmsddm_authhelper DESTINATION ${KAUTH_HELPER_INSTALL_DIR})

#installer tool for knewstuff
add_executable(sddmthemeinstaller
    sddmthemeinstaller.cpp
)

target_link_libraries(sddmthemeinstaller
    KF5::I18n
    KF5::Auth
    KF5::CoreAddons
    KF5::ConfigCore
    KF5::WidgetsAddons)
install(TARGETS sddmthemeinstaller ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(src)
install(FILES sddmtheme.knsrc DESTINATION ${KDE_INSTALL_CONFDIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

