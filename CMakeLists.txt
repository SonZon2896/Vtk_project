cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)

project(VTK_Project)

find_package(VTK COMPONENTS 
  CommonColor
  CommonCore
  CommonDataModel
  CommonTransforms
  FiltersCore
  FiltersGeneral
  FiltersPoints
  FiltersSources
  IOGeometry
  IOImport
  IOInfovis
  IOXML
  IOGeometry
  InteractionStyle
  InteractionWidgets
  RenderingContextOpenGL2
  RenderingCore
  RenderingFreeType
  RenderingGL2PSOpenGL2
  RenderingLabel
  RenderingOpenGL2
)

if (NOT VTK_FOUND)
  message(FATAL_ERROR "VTK_Project: Unable to find the VTK build folder.")
endif()

# Prevent a "command line is too long" failure in Windows.
# set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")
add_executable(VTK_Project 
	"vtk_project.cpp"
	"Application.cpp"
	"Interactors/Interactor.cpp"
	"Interactors/InteractorStyleProject.cpp" 
	"Importers/CSV3DImporter.cpp"
	"Callbacks/ChangeVisionSliderCallback.cpp"
	"Callbacks/ChangeClippingPlaneCallback.cpp"
	"Callbacks/ChangeClippingButtonCallback.cpp"
	)

  target_link_libraries(VTK_Project PRIVATE ${VTK_LIBRARIES}
)

file(COPY Examples DESTINATION "${CMAKE_BINARY_DIR}")

# vtk_module_autoinit is needed
vtk_module_autoinit(
  TARGETS VTK_Project
  MODULES ${VTK_LIBRARIES}
)

set(EXPORT_COMPILE_COMMANDS ON)

# the compiler flags for compiling C sources 
MESSAGE( STATUS "CMAKE_C_FLAGS: " ${CMAKE_C_FLAGS} )

# the compiler flags for compiling C++ sources 
MESSAGE( STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS} )