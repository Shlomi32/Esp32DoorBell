idf_component_register(SRCS "GpioInput.cpp" "GpioOutput.cpp" "main.cpp"
                            "Wifi.cpp"
                            "ChipInfo.cpp"
                            "Mqtt.cpp"
                            "GpioInput.cpp"
                            "GpioOutput.cpp"
                    INCLUDE_DIRS "")

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
