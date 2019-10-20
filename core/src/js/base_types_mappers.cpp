#include "base_types_mappers.hpp"

namespace aardvark::js {

Mapper<float> float_mapper = SimpleMapper<float, float_to_js, float_from_js>();

Mapper<Size> size_mapper = ObjectMapper<Size>(
    {ADV_PROP_MAPPER(Size, float, float_mapper, "width", width),
     ADV_PROP_MAPPER(Size, float, float_mapper, "height", height)});

Mapper<Position> position_mapper = ObjectMapper<Position>(
    {ADV_PROP_MAPPER(Position, float, float_mapper, "left", left),
     ADV_PROP_MAPPER(Position, float, float_mapper, "top", top)});

Mapper<BoxConstraints> box_constraints_mapper = ObjectMapper<BoxConstraints>(
    {ADV_PROP_MAPPER(BoxConstraints, float, float_mapper, "minWidth",
                     min_width),
     ADV_PROP_MAPPER(BoxConstraints, float, float_mapper, "maxWidth",
                     max_width),
     ADV_PROP_MAPPER(BoxConstraints, float, float_mapper, "minHeight",
                     min_height),
     ADV_PROP_MAPPER(BoxConstraints, float, float_mapper, "maxHeight",
                     max_height)});

}  // namespace aardvark::js
