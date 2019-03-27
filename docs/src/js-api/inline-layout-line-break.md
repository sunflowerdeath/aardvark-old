# `enum class inline_layout::LineBreak`

**Defined in:** [`"aardvark/inline_layout/text_span.hpp"`]()

### Values:

- `words` &ndash; Use default unicode line breaking algorithm

- `chars` &ndash; Line break is allowed between any two characters

- `nobreak` &ndash; Never allow break text

- `overflow` &ndash; If word can not fit in the line when using default breaking 
algorithm, it is allowed to break it at arbitrary point.

