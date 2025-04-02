# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/tsystem/esp/esp-idf/components/bootloader/subproject"
  "/home/tsystem/esp/MotorController_UI/MotorController_UI/build/bootloader"
  "/home/tsystem/esp/MotorController_UI/MotorController_UI/build/bootloader-prefix"
  "/home/tsystem/esp/MotorController_UI/MotorController_UI/build/bootloader-prefix/tmp"
  "/home/tsystem/esp/MotorController_UI/MotorController_UI/build/bootloader-prefix/src/bootloader-stamp"
  "/home/tsystem/esp/MotorController_UI/MotorController_UI/build/bootloader-prefix/src"
  "/home/tsystem/esp/MotorController_UI/MotorController_UI/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/tsystem/esp/MotorController_UI/MotorController_UI/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/tsystem/esp/MotorController_UI/MotorController_UI/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
