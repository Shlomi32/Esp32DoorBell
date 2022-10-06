idf_component_register(SRCS "main.cpp"
                            "Wifi.cpp"
                            "ChipInfo.cpp"
                            "Mqtt.cpp"
                    INCLUDE_DIRS "")

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
