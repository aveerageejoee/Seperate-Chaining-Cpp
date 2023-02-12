# Seperate-Chaining-C++ 

ADS_set Class

This is a C++ class template for an associative data structure (ADS) set that follows Seperate Chaining

    Key: The type of elements stored in the set.
    N: The size of the initial table (defaults to 7).

Member Types

    value_type: Alias for Key.
    key_type: Alias for Key.
    reference: Reference to value_type.
    const_reference: Constant reference to value_type.
    size_type: Type for representing the size of the set (size_t).
    difference_type: Type for representing the difference between two iterators (std::ptrdiff_t).
    const_iterator: Alias for Iterator.
    iterator: Alias for const_iterator.
    key_compare: Type for comparing keys, used for the B+-Tree implementation (std::less<key_type>).
    key_equal: Type for comparing keys for equality, used for the Hashing implementation (std::equal_to<key_type>).
    hasher: Type for computing the hash value of a key, used for the Hashing implementation (std::hash<key_type>).

Member Functions

The class has many member functions, including insert, erase, find, begin, end, and others. Please refer to the full implementation for details.
Iterator

The Iterator class provides an iterator for the set. It has functions such as operator*, operator++, and others. Please refer to the full implementation for details.
Implementation

The set is implemented with Seperate Chaining. A block data structure is used to store elements in the set, and the table is resized dynamically when the load factor reaches 0.7.
