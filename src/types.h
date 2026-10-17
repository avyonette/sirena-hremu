#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>

#define stringify(x) #x
#define expand_stringify(x) stringify(x)

#ifdef __cplusplus
	#include <string>
	#include <string_view>

	typedef std::string string;
	typedef std::string_view string_view;

	#include <memory>
	using std::unique_ptr;
	using std::shared_ptr;
	using std::weak_ptr;

	using std::make_unique;
	using std::make_shared;
#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef intptr_t iptr;
typedef uintptr_t uptr;

typedef size_t usize;
