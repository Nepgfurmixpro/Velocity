#pragma once
#ifdef VELOCITY_DEBUG
#define __VLY_ASSERT_CRITICAL_LOG(val, msg, spc) if (!(val)) { spdlog::debug("Assertion Failed: {0}", #val); throw std::runtime_error(msg spc #val); }
#endif

#define VLY_ASSERT_CRITICAL_LOG(val, msg) __VLY_ASSERT_CRITICAL_LOG(val, msg, " ")
