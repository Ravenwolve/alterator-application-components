macro(add_alterator_application_components_executable name)
  add_executable(${name} ${ARGN})

  # add install target
  set_target_properties(${name} PROPERTIES OUTPUT_NAME ${name})
  install(TARGETS ${name} DESTINATION bin
      PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

  # copy binary
  add_custom_command(TARGET ${name} POST_BUILD COMMAND
      ${CMAKE_COMMAND} ARGS -E copy
        $<TARGET_FILE:${name}>
        ${CMAKE_BINARY_DIR}/bin/$<TARGET_FILE_NAME:${name}>
      COMMENT "Copy ${name} to ${CMAKE_BINARY_DIR}/bin")
endmacro()

find_package(qbase COMPONENTS logger REQUIRED)
find_package(Qt5 COMPONENTS Widgets Core Gui DBus Xml LinguistTools REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(HEADERS
  constants.h
  amcapp.h

  componentswidget.h
  mainwindow.h
  controllerinterface.h
  controller.h
  mainwindowsettings.h
  packagesdialog.h
  waitdialog.h

  model/componentcategory.h
  model/componentobject.h
  componentsapplicator.h

  model/model.h
  model/item.h
  model/modelbuilder.h
  model/amcobject.h

  model/parsers/objectparserinterface.h
  model/parsers/baseobjectparser.h
)

set(SOURCES
  main.cpp
  amcapp.cpp

  mainwindow.cpp
  componentswidget.cpp
  controller.cpp
  mainwindowsettings.cpp
  packagesdialog.cpp
  waitdialog.cpp

  model/componentobject.cpp
  componentsapplicator.cpp

  model/model.cpp
  model/item.cpp
  model/modelbuilder.cpp
  model/amcobject.cpp

  model/parsers/baseobjectparser.cpp
)

set(UI_FORMS
  mainwindow.ui
  componentswidget.ui
  packagesdialog.ui
  waitdialog.ui
)
set(RESOURCES
  icon.qrc
)
set(TS_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/ru.ts
  ${CMAKE_CURRENT_SOURCE_DIR}/en.ts
)

add_custom_target(app_ts
  DEPENDS ${TS_FILES}
  COMMAND ${Qt5_LUPDATE_EXECUTABLE} * -recursive ${CMAKE_CURRENT_SOURCE_DIR} -ts ${TS_FILES} -no-obsolete
)

set_source_files_properties(${TS_FILES}
  PROPERTIES OUTPUT_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}
)

add_translation(QM_FILES ${TS_FILES})

qt5_wrap_ui(UI_SOURCES ${UI_FORMS})

add_translation_resource(RESOURCES_SRC "alterator-application-components" ${QM_FILES})

qt5_add_resources(RCC_SOURCES ${RESOURCES} ${RESOURCES_SRC})

add_alterator_application_components_executable(
    alterator-application-components ${SOURCES} ${HEADERS} ${UI_SOURCES} ${RCC_SOURCES}
)

target_link_libraries(alterator-application-components
  qbase::logger
  Qt5::Widgets
  Qt5::Core
  Qt5::DBus
  Qt5::Xml
  Qt5::Gui
)

