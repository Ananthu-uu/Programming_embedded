# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Uppsala/ZephyrProjects/zephyrproject/zephyr/modules/hal_rpi_pico/bootloader"
  "D:/Uppsala/ZephyrProjects/zephyrproject/zephyr/Project/Combine/Controller_with_Driver/build/bootloader"
  "D:/Uppsala/ZephyrProjects/zephyrproject/zephyr/Project/Combine/Controller_with_Driver/build/modules/hal_rpi_pico/second_stage_bootloader-prefix"
  "D:/Uppsala/ZephyrProjects/zephyrproject/zephyr/Project/Combine/Controller_with_Driver/build/modules/hal_rpi_pico/second_stage_bootloader-prefix/tmp"
  "D:/Uppsala/ZephyrProjects/zephyrproject/zephyr/Project/Combine/Controller_with_Driver/build/modules/hal_rpi_pico/second_stage_bootloader-prefix/src/second_stage_bootloader-stamp"
  "D:/Uppsala/ZephyrProjects/zephyrproject/zephyr/Project/Combine/Controller_with_Driver/build/modules/hal_rpi_pico/second_stage_bootloader-prefix/src"
  "D:/Uppsala/ZephyrProjects/zephyrproject/zephyr/Project/Combine/Controller_with_Driver/build/modules/hal_rpi_pico/second_stage_bootloader-prefix/src/second_stage_bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Uppsala/ZephyrProjects/zephyrproject/zephyr/Project/Combine/Controller_with_Driver/build/modules/hal_rpi_pico/second_stage_bootloader-prefix/src/second_stage_bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Uppsala/ZephyrProjects/zephyrproject/zephyr/Project/Combine/Controller_with_Driver/build/modules/hal_rpi_pico/second_stage_bootloader-prefix/src/second_stage_bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
