cmake_minimum_required(VERSION 3.22)

project(OrBit6 VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "")

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.16-security
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_gui_app(OrBit6
    PRODUCT_NAME "OrBit 6"
    COMPANY_NAME "HenrySinger"
    BUNDLE_ID "com.henrysinger.orbit6"
    MICROPHONE_PERMISSION_ENABLED FALSE
    DOCUMENT_EXTENSIONS ""
    ICON_BIG "Assets/Icon.png"
)

juce_generate_juce_header(OrBit6)

target_sources(OrBit6 PRIVATE
    Source/Main.cpp
    Source/MainComponent.cpp
    Source/TextTabButton.cpp
    Source/BrowserView.cpp
    Source/SamplerView.cpp
    Source/SampleLibrary.cpp
    Source/FolderTreeItem.cpp
    Source/SampleListBoxModel.cpp
    Source/SamplePlayer.cpp
    Source/SamplerEngine.cpp
    Source/SamplerKeyboardComponent.cpp
    Source/SamplerWaveformComponent.cpp
    Source/TapeEffect.cpp
    Source/FavoritesStore.cpp
    Source/WaveformComponent.cpp
    Source/Theme.cpp
)

target_compile_definitions(OrBit6 PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:OrBit6,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:OrBit6,JUCE_VERSION>"
)

target_include_directories(OrBit6 PRIVATE Source)

target_link_libraries(OrBit6 PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_gui_extra
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# Ad-hoc sign so Gatekeeper doesn't flag the app as "damaged" when launched
# locally or copied to another Mac (recipients still need to right-click >
# Open the first time, since this isn't a notarized Developer ID signature).
if(APPLE)
    add_custom_command(TARGET OrBit6 POST_BUILD
        COMMAND codesign --force --deep -s - "$<TARGET_BUNDLE_DIR:OrBit6>"
        COMMENT "Ad-hoc signing OrBit 6.app"
    )
endif()
