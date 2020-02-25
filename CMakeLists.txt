project(sddm-kcm)
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
set(PROJECT_VERSION "5.18.2")

set(QT_MIN_VERSION "5.12.0")
set(KF5_MIN_VERSION "5.66.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(FeatureSummary)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Gui Widgets Quick QuickWidgets)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    CoreAddons
    I18n
    XmlGui
    Auth
    ConfigWidgets
    KIO
    Archive
    NewStuff
)

if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
   add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)
   add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x060000)
endif()


add_definitions(-DTRANSLATION_DOMAIN=\"kcm_sddm\")
add_definitions(-DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT)

install(FILES kcm_sddm.desktop DESTINATION ${CMAKE_INSTALL_KSERVICES5DIR})

kauth_install_actions(org.kde.kcontrol.kcmsddm kcm_sddm.actions)

add_executable(kcmsddm_authhelper sddmauthhelper.cpp)
target_link_libraries(kcmsddm_authhelper KF5::AuthCore KF5::ConfigCore KF5::Archive KF5::I18n)

kauth_install_helper_files(kcmsddm_authhelper org.kde.kcontrol.kcmsddm root)
install(TARGETS kcmsddm_authhelper DESTINATION ${KAUTH_HELPER_INSTALL_DIR})

#installer tool for knewstuff
add_executable(sddmthemeinstaller
    sddmthemeinstaller.cpp
)

target_link_libraries(sddmthemeinstaller
    KF5::I18n
    KF5::AuthCore
    KF5::CoreAddons
    KF5::ConfigCore
    KF5::WidgetsAddons)
install(TARGETS sddmthemeinstaller ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(src)
install(FILES sddmtheme.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR})

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
