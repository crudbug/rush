#pragma once

#ifdef _MSC_VER
#include "cpp/xf.hpp"
#endif

#ifdef __GNUC__
#include "cpp/xf.hpp"
#endif

#ifdef _RJIT
#include "rs/xf.rs"
#endif

#ifdef _RNASM
#include "rs/xf.rs"
#endif

#ifdef _RVM
#include "rs/xf.rs"
#endif

#ifdef _RGPP
#include "gpp/xf.rs"
#endif
