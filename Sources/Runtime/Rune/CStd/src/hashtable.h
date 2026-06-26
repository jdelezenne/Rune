#pragma once

#include <rune/allocator.h>
#include <rune/hashmap.h>
#include <rune/hashset.h>
#include <rune/memory.h>
#include <rune/types.h>

typedef enum
{
    HT_SLOT_EMPTY = 0,
    HT_SLOT_OCCUPIED = 1,
    HT_SLOT_DELETED = 2,
} HtSlotState;

typedef struct
{
    RuneHashSet* set;
    usize slot_stride;
} SetTable;

typedef struct
{
    RuneHashMap* map;
    usize slot_stride;
} MapTable;

static inline uint64 hashtable_hash_bytes(void const* bytes, usize length)
{
    uint8 const* data = (uint8 const*)(bytes);
    uint64 hash = 14695981039346656037ULL;
    for (uintptr index = 0; index < length; ++index)
    {
        hash ^= (uint64)(data[index]);
        hash *= 1099511628211ULL;
    }

    return hash;
}

static inline uint64 hashtable_hash_element(void const* key, RuneHashFn hash_fn, usize size)
{
    return hash_fn != nullptr ? hash_fn(key) : hashtable_hash_bytes(key, size);
}

static inline bool hashtable_elements_equal(void const* a, void const* b, RuneEqualFn equal_fn, usize size)
{
    if (equal_fn != nullptr)
    {
        return equal_fn(a, b);
    }

    return rune_memory_compare(a, b, size) == 0;
}

static inline HtSlotState hashtable_slot_state(uint8 const* slot)
{
    return (HtSlotState)(slot[0]);
}

static inline void hashtable_set_slot_state(uint8* slot, HtSlotState state)
{
    slot[0] = (uint8)(state);
}

static inline uint64 hashtable_set_slot_stride(usize elementSize)
{
    return rune_align_up(1, RUNE_MAX_ALIGN) + rune_align_up(elementSize, RUNE_MAX_ALIGN);
}

static inline uint64 hashtable_map_slot_stride(usize keySize, usize valueSize)
{
    uint64 const keyOffset = rune_align_up(1, RUNE_MAX_ALIGN);
    uint64 const valueOffset = rune_align_up(keyOffset + keySize, RUNE_MAX_ALIGN);
    return valueOffset + valueSize;
}

static inline uint8* hashtable_set_value(uint8* slot)
{
    return slot + rune_align_up(1, RUNE_MAX_ALIGN);
}

static inline uint8 const* hashtable_set_value_const(uint8 const* slot)
{
    return slot + rune_align_up(1, RUNE_MAX_ALIGN);
}

static inline uint8* hashtable_map_key(uint8* slot)
{
    return slot + rune_align_up(1, RUNE_MAX_ALIGN);
}

static inline uint8 const* hashtable_map_key_const(uint8 const* slot)
{
    return slot + rune_align_up(1, RUNE_MAX_ALIGN);
}

static inline uint8* hashtable_map_value(uint8* slot, usize keySize)
{
    uint64 const keyOffset = rune_align_up(1, RUNE_MAX_ALIGN);
    return slot + rune_align_up(keyOffset + keySize, RUNE_MAX_ALIGN);
}

static inline uint8 const* hashtable_map_value_const(uint8 const* slot, usize keySize)
{
    uint64 const keyOffset = rune_align_up(1, RUNE_MAX_ALIGN);
    return slot + rune_align_up(keyOffset + keySize, RUNE_MAX_ALIGN);
}

static inline bool hashtable_allocator_ready(RuneAllocator const* allocator)
{
    return rune_allocator_is_valid(allocator);
}

static inline void hashtable_ensure_allocator(RuneAllocator const** allocator)
{
    if (!hashtable_allocator_ready(*allocator))
    {
        *allocator = rune_get_default_allocator();
    }
}

static inline bool hashtable_allocate_slots(
    RuneAllocator const* allocator,
    uint8** slots,
    usize* capacity,
    usize newCapacity,
    usize slotStride)
{
    if (newCapacity == 0)
    {
        return true;
    }

    usize const bytes = (usize)(newCapacity * slotStride);
    void* const memory = rune_allocator_allocate(allocator, bytes, RUNE_MAX_ALIGN);
    if (memory == nullptr)
    {
        return false;
    }

    rune_memory_zero(memory, bytes);
    *slots = (uint8*)(memory);
    *capacity = newCapacity;
    return true;
}

static inline void hashtable_free_slots(RuneAllocator const* allocator, uint8* slots)
{
    if (slots != nullptr && hashtable_allocator_ready(allocator))
    {
        rune_allocator_deallocate(allocator, slots);
    }
}

// MARK: - SetTable functions

static inline SetTable set_table_init(RuneHashSet* self)
{
    SetTable t = {0};
    t.set = self;
    if (self != nullptr && self->element_size > 0)
    {
        t.slot_stride = hashtable_set_slot_stride(self->element_size);
    }
    return t;
}

static inline uint64 set_table_find_occupied(SetTable const* t, void const* element)
{
    RuneHashSet* set = t->set;
    if (set == nullptr || set->capacity == 0 || element == nullptr)
    {
        return set != nullptr ? set->capacity : 0;
    }

    uintptr index = hashtable_hash_element(element, set->hash_fn, set->element_size) % set->capacity;
    for (uint64 i = 0; i < set->capacity; ++i)
    {
        uint8 const* const slot = set->slots + index * t->slot_stride;
        HtSlotState const state = hashtable_slot_state(slot);

        if (state == HT_SLOT_EMPTY)
        {
            return set->capacity;
        }

        if (state == HT_SLOT_OCCUPIED &&
            hashtable_elements_equal(hashtable_set_value_const(slot), element, set->equal_fn, set->element_size))
        {
            return index;
        }

        index = (index + 1) % set->capacity;
    }

    return set->capacity;
}

static inline bool set_table_rehash(SetTable* t, uint64 requestedCapacity);

static inline void set_table_insert_bytes(SetTable* t, void const* element)
{
    RuneHashSet* set = t->set;
    if (set == nullptr || element == nullptr || t->slot_stride == 0)
    {
        return;
    }

    if ((set->count + 1) * 4 >= set->capacity * 3)
    {
        set_table_rehash(t, set->capacity == 0 ? 8 : set->capacity * 2);
    }

    if (set->capacity == 0)
    {
        set_table_rehash(t, 8);
    }

    uint64 const existing = set_table_find_occupied(t, element);
    if (existing != set->capacity)
    {
        rune_memory_copy(element, hashtable_set_value(set->slots + existing * t->slot_stride), set->element_size);
        return;
    }

    uint64 firstDeleted = set->capacity;
    uintptr index = hashtable_hash_element(element, set->hash_fn, set->element_size) % set->capacity;
    for (uint64 i = 0; i < set->capacity; ++i)
    {
        uint8* const slot = set->slots + index * t->slot_stride;
        HtSlotState const state = hashtable_slot_state(slot);

        if (state == HT_SLOT_OCCUPIED)
        {
            index = (index + 1) % set->capacity;
            continue;
        }

        if (state == HT_SLOT_DELETED && firstDeleted == set->capacity)
        {
            firstDeleted = index;
        }

        if (state == HT_SLOT_EMPTY)
        {
            uint64 const target = firstDeleted == set->capacity ? index : firstDeleted;
            uint8* const targetSlot = set->slots + target * t->slot_stride;
            hashtable_set_slot_state(targetSlot, HT_SLOT_OCCUPIED);
            rune_memory_copy(element, hashtable_set_value(targetSlot), set->element_size);
            ++set->count;
            return;
        }

        index = (index + 1) % set->capacity;
    }
}

static inline bool set_table_rehash(SetTable* t, uint64 requestedCapacity)
{
    RuneHashSet* set = t->set;
    if (set == nullptr || t->slot_stride == 0)
    {
        return false;
    }

    hashtable_ensure_allocator(&set->allocator);

    usize newCapacity = set->capacity == 0 ? 8 : set->capacity;
    while (newCapacity < requestedCapacity)
    {
        newCapacity *= 2;
    }

    uint8* oldSlots = set->slots;
    uint64 const oldCapacity = set->capacity;
    set->slots = nullptr;
    set->count = 0;
    set->capacity = 0;

    if (!hashtable_allocate_slots(set->allocator, &set->slots, &set->capacity, newCapacity, t->slot_stride))
    {
        set->slots = oldSlots;
        set->capacity = oldCapacity;
        return false;
    }
    t->slot_stride = hashtable_set_slot_stride(set->element_size);

    if (oldSlots != nullptr)
    {
        for (uintptr index = 0; index < oldCapacity; ++index)
        {
            uint8* const slot = oldSlots + index * t->slot_stride;
            if (hashtable_slot_state(slot) == HT_SLOT_OCCUPIED)
            {
                set_table_insert_bytes(t, hashtable_set_value(slot));
            }
        }

        hashtable_free_slots(set->allocator, oldSlots);
    }

    return true;
}

// MARK: - MapTable functions

static inline MapTable map_table_init(RuneHashMap* self)
{
    MapTable t = {0};
    t.map = self;
    if (self != nullptr && self->key_size > 0 && self->value_size > 0)
    {
        t.slot_stride = hashtable_map_slot_stride(self->key_size, self->value_size);
    }
    return t;
}

static inline uint64 map_table_find_occupied(MapTable const* t, void const* key)
{
    RuneHashMap* map = t->map;
    if (map == nullptr || map->capacity == 0 || key == nullptr)
    {
        return map != nullptr ? map->capacity : 0;
    }

    uintptr index = hashtable_hash_element(key, map->hash_fn, map->key_size) % map->capacity;
    for (uint64 i = 0; i < map->capacity; ++i)
    {
        uint8 const* const slot = map->slots + index * t->slot_stride;
        HtSlotState const state = hashtable_slot_state(slot);

        if (state == HT_SLOT_EMPTY)
        {
            return map->capacity;
        }

        if (state == HT_SLOT_OCCUPIED &&
            hashtable_elements_equal(hashtable_map_key_const(slot), key, map->equal_fn, map->key_size))
        {
            return index;
        }

        index = (index + 1) % map->capacity;
    }

    return map->capacity;
}

static inline bool map_table_rehash(MapTable* t, uint64 requestedCapacity);

static inline void map_table_insert_entry(MapTable* t, void const* key, void const* value)
{
    RuneHashMap* map = t->map;
    if (map == nullptr || key == nullptr || value == nullptr || t->slot_stride == 0)
    {
        return;
    }

    if ((map->count + 1) * 4 >= map->capacity * 3)
    {
        map_table_rehash(t, map->capacity == 0 ? 8 : map->capacity * 2);
    }

    if (map->capacity == 0)
    {
        map_table_rehash(t, 8);
    }

    uint64 const existing = map_table_find_occupied(t, key);
    if (existing != map->capacity)
    {
        rune_memory_copy(value, hashtable_map_value(map->slots + existing * t->slot_stride, map->key_size), map->value_size);
        return;
    }

    uint64 firstDeleted = map->capacity;
    uintptr index = hashtable_hash_element(key, map->hash_fn, map->key_size) % map->capacity;
    for (uint64 i = 0; i < map->capacity; ++i)
    {
        uint8* const slot = map->slots + index * t->slot_stride;
        HtSlotState const state = hashtable_slot_state(slot);

        if (state == HT_SLOT_OCCUPIED)
        {
            index = (index + 1) % map->capacity;
            continue;
        }

        if (state == HT_SLOT_DELETED && firstDeleted == map->capacity)
        {
            firstDeleted = index;
        }

        if (state == HT_SLOT_EMPTY)
        {
            uint64 const target = firstDeleted == map->capacity ? index : firstDeleted;
            uint8* const targetSlot = map->slots + target * t->slot_stride;
            hashtable_set_slot_state(targetSlot, HT_SLOT_OCCUPIED);
            rune_memory_copy(key, hashtable_map_key(targetSlot), map->key_size);
            rune_memory_copy(value, hashtable_map_value(targetSlot, map->key_size), map->value_size);
            ++map->count;
            return;
        }

        index = (index + 1) % map->capacity;
    }
}

static inline bool map_table_rehash(MapTable* t, uint64 requestedCapacity)
{
    RuneHashMap* map = t->map;
    if (map == nullptr || t->slot_stride == 0)
    {
        return false;
    }

    hashtable_ensure_allocator(&map->allocator);

    usize newCapacity = map->capacity == 0 ? 8 : map->capacity;
    while (newCapacity < requestedCapacity)
    {
        newCapacity *= 2;
    }

    uint8* oldSlots = map->slots;
    uint64 const oldCapacity = map->capacity;
    map->slots = nullptr;
    map->count = 0;
    map->capacity = 0;

    if (!hashtable_allocate_slots(map->allocator, &map->slots, &map->capacity, newCapacity, t->slot_stride))
    {
        map->slots = oldSlots;
        map->capacity = oldCapacity;
        return false;
    }
    t->slot_stride = hashtable_map_slot_stride(map->key_size, map->value_size);

    if (oldSlots != nullptr)
    {
        for (uintptr index = 0; index < oldCapacity; ++index)
        {
            uint8* const slot = oldSlots + index * t->slot_stride;
            if (hashtable_slot_state(slot) == HT_SLOT_OCCUPIED)
            {
                map_table_insert_entry(t, hashtable_map_key(slot), hashtable_map_value(slot, map->key_size));
            }
        }

        hashtable_free_slots(map->allocator, oldSlots);
    }

    return true;
}

// MARK: - Clear helpers

static inline void hashtable_clear_set(RuneHashSet* set)
{
    if (set == nullptr)
    {
        return;
    }

    hashtable_free_slots(set->allocator, set->slots);
    set->slots = nullptr;
    set->count = 0;
    set->capacity = 0;
    set->element_size = 0;
}

static inline void hashtable_clear_map(RuneHashMap* map)
{
    if (map == nullptr)
    {
        return;
    }

    hashtable_free_slots(map->allocator, map->slots);
    map->slots = nullptr;
    map->count = 0;
    map->capacity = 0;
    map->key_size = 0;
    map->value_size = 0;
}
