# ICU data

Since ICU is statically linked, I try to remove as much data as possible.
Current build includes only data that is required to work.

Later task is to determine what data files could be used in some way, and what
are totally useless. For example, JavascriptCore can be compiled with collation
support.
Then I may add some amount of that data, maybe create different presets or allow
users to customize it manually - select which features and locales they need.
In default preset, compiled executable should be as compact as possible, ideally
less than 5MB after zip, and 10MB in the worst possible case.

What is currently removed:

- removed dictionary break iterator rules (leave only default break iterators)
- removed collation
- removed currencies
- removed langs
- removed locales
- removed mappings for codepages
- removed misc data
- removed rbnf
- removed regions
- removed sprep
- removed translit
- removed unit
- removed zones

