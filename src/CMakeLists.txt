set(PROJECT_NAME ${CMAKE_PROJECT_NAME})

set(UNIT_SOURCE_CPPS
  units/arithmetic_logic_unit.cpp
  units/instruction_unit.cpp
  units/load_store_buffer.cpp
  units/memory_unit.cpp
  units/reorder_buffer.cpp
  units/reservation_station.cpp
  units/register_file.cpp
)

set(UTILS_SOURCE_CPPS
  utils/utils.cpp
)

add_library(simulator ${UNIT_SOURCE_CPPS} ${UTILS_SOURCE_CPPS} simulator.cpp)
add_library(naive_simulator ${UNIT_SOURCE_CPPS} ${UTILS_SOURCE_CPPS} naive_simulator.cpp)
