#pragma once

#include "SkMatrix.h"
#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../events.hpp"
#include "../elements/align.hpp"
#include "../elements/padding.hpp"
#include "../elements/sized.hpp"
#include "helpers.hpp"
#include "mappers.hpp"

namespace aardvark::js {

extern Mapper<int>* int_mapper;
extern Mapper<float>* float_mapper;
extern Mapper<std::string>* str_mapper;
extern Mapper<UnicodeString>* icu_str_mapper;

extern Mapper<Size>* size_mapper;
extern Mapper<Position>* position_mapper;
extern Mapper<Value>* value_mapper;
extern Mapper<BoxConstraints>* box_constraints_mapper;
extern Mapper<SkColor>* color_mapper;
extern Mapper<SkMatrix>* matrix_mapper;

extern Mapper<PointerEvent>* pointer_event_mapper;
extern Mapper<KeyEvent>* key_event_mapper;
extern Mapper<ScrollEvent>* scroll_event_mapper;

extern Mapper<elements::EdgeInsets>* alignment_mapper;
extern Mapper<Padding>* padding_mapper;
extern Mapper<elements::SizeConstraints>* size_constraints_mapper;

}  // namespace aardvark::js
