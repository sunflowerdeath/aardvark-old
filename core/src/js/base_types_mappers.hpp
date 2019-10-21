#pragma once

#include "../base_types.hpp"
#include "../box_constraints.hpp"
#include "../events.hpp"
#include "../elements/align.hpp"
#include "../elements/padding.hpp"
#include "../elements/sized.hpp"
#include "helpers.hpp"
#include "mappers.hpp"

namespace aardvark::js {

Mapper<int>* int_mapper();
Mapper<float>* float_mapper();
Mapper<std::string>* str_mapper();
Mapper<UnicodeString>* icu_str_mapper();

Mapper<Size>* size_mapper();
Mapper<Position>* position_mapper();
Mapper<Value>* value_mapper();
Mapper<BoxConstraints>* box_constraints_mapper();
Mapper<SkColor>* color_mapper();

Mapper<PointerEvent>* pointer_event_mapper();
Mapper<KeyEvent>* key_event_mapper();
Mapper<ScrollEvent>* scroll_event_mapper();

Mapper<elements::EdgeInsets>* alignment_mapper();
Mapper<Padding>* padding_mapper();
Mapper<elements::SizeConstraints>* size_constraints_mapper();

}  // namespace aardvark::js
