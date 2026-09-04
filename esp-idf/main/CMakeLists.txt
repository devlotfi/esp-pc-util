idf_component_register(
  SRCS "main.cpp"
  INCLUDE_DIRS "../include"
)

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 23)
set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD_REQUIRED ON)
