#pragma once

#include <rune/types.h>

usize rune_platform_vm_page_size(void);
void* rune_platform_vm_reserve(usize size);
bool rune_platform_vm_commit(void* base, usize size);
void rune_platform_vm_decommit(void* base, usize size);
void rune_platform_vm_release(void* base, usize size);