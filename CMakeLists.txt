cmake_minimum_required(VERSION 3.22)

project(ApolloCrate VERSION 0.1.0)

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

juce_add_gui_app(ApolloCrate
    PRODUCT_NAME "Apollo Crate"
    COMPANY_NAME "HenrySinger"
    BUNDLE_ID "com.henrysinger.apollocrate"
    MICROPHONE_PERMISSION_ENABLED FALSE
    DOCUMENT_EXTENSIONS ""
)

juce_generate_juce_header(ApolloCrate)

target_sources(ApolloCrate PRIVATE
    Source/Main.cpp
    Source/MainComponent.cpp
    Source/SampleLibrary.cpp
    Source/FolderTreeItem.cpp
    Source/SampleListBoxModel.cpp
    Source/SamplePlayer.cpp
    Source/Theme.cpp
)

target_compile_definitions(ApolloCrate PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:ApolloCrate,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:ApolloCrate,JUCE_VERSION>"
)

target_include_directories(ApolloCrate PRIVATE Source)

target_link_libraries(ApolloCrate PRIVATE
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
    add_custom_command(TARGET ApolloCrate POST_BUILD
        COMMAND codesign --force --deep -s - "$<TARGET_BUNDLE_DIR:ApolloCrate>"
        COMMENT "Ad-hoc signing Apollo Crate.app"
    )
endif()
