#pragma once

#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "helpers.hpp"
#include "mappers.hpp"

namespace aardvark::js {

extern Mapper<float> float_mapper;    
extern Mapper<Size> size_mapper;    
extern Mapper<Position> position_mapper;    
extern Mapper<BoxConstraints> box_constraints_mapper;

}  // namespace aardvark::js
