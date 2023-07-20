/*
 * Copyright (C) 2023 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dlfcn.h>

#include "Power.h"

#include <android-base/logging.h>

#ifdef TAP_TO_WAKE_NODE
#include <android-base/file.h>
#endif

namespace aidl {
namespace android {
namespace hardware {
namespace power {
namespace impl {
namespace hisi {

#ifdef MODE_EXT
extern bool isDeviceSpecificModeSupported(Mode type, bool* _aidl_return);
extern bool setDeviceSpecificMode(Mode type, bool enabled);
#endif

const std::vector<Boost> SUPPORTED_BOOSTS {
    Boost::INTERACTION,
};

const std::vector<Mode> SUPPORTED_MODES {
#ifdef TAP_TO_WAKE_NODE
    Mode::DOUBLE_TAP_TO_WAKE,
#endif
    Mode::LOW_POWER,
    Mode::SUSTAINED_PERFORMANCE,
    Mode::LAUNCH,
    Mode::EXPENSIVE_RENDERING,
    Mode::INTERACTIVE,
};

Power::Power() {
    mLowPowerEnabled = 0;
}

Power::~Power() { }

ndk::ScopedAStatus Power::setMode(Mode type, bool enabled) {
    LOG(VERBOSE) << "Power setMode: " << static_cast<int32_t>(type) << " to: " << enabled;

#ifdef MODE_EXT
    if (setDeviceSpecificMode(type, enabled)) {
        return ndk::ScopedAStatus::ok();
    }
#endif
    switch (type) {
#ifdef TAP_TO_WAKE_NODE
        case Mode::DOUBLE_TAP_TO_WAKE:
        {
            ::android::base::WriteStringToFile(enabled ? "1" : "0", TAP_TO_WAKE_NODE, true);
            break;
        }
#endif
        case Mode::LAUNCH:
        {
            break;
        }
        case Mode::INTERACTIVE:
        {
            break;
        }
        case Mode::EXPENSIVE_RENDERING:
        {
            break;
        }
        case Mode::SUSTAINED_PERFORMANCE:
        {
            break;
        }
        case Mode::LOW_POWER:
            mLowPowerEnabled = enabled;
            break;
        default:
            break;
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Power::isModeSupported(Mode type, bool* _aidl_return) {
#ifdef MODE_EXT
    if (isDeviceSpecificModeSupported(type, _aidl_return)) {
        return ndk::ScopedAStatus::ok();
    }
#endif

    LOG(INFO) << "Power isModeSupported: " << static_cast<int32_t>(type);
    *_aidl_return = std::find(SUPPORTED_MODES.begin(), SUPPORTED_MODES.end(), type) != SUPPORTED_MODES.end();

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Power::setBoost(Boost type, int32_t durationMs) {
    if (mLowPowerEnabled) {
        LOG(VERBOSE) << "Will not perform boosts in LOW_POWER";
        return ndk::ScopedAStatus::ok();
    }

    switch (type) {
        case Boost::INTERACTION:
            LOG(VERBOSE) << "Power setBoost INTERACTION for: " << durationMs << "ms";
            break;
        default:
            LOG(ERROR) << "Power unknown boost type: " << static_cast<int32_t>(type);
            return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    }

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Power::isBoostSupported(Boost type, bool* _aidl_return) {
    LOG(INFO) << "Power isBoostSupported: " << static_cast<int32_t>(type);
    *_aidl_return = std::find(SUPPORTED_BOOSTS.begin(), SUPPORTED_BOOSTS.end(), type) != SUPPORTED_BOOSTS.end();
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Power::createHintSession(int32_t, int32_t, const std::vector<int32_t>&, int64_t,
                                            std::shared_ptr<IPowerHintSession>* _aidl_return) {
    *_aidl_return = nullptr;
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

ndk::ScopedAStatus Power::getHintSessionPreferredRate(int64_t* outNanoseconds) {
    *outNanoseconds = -1;
    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

}  // namespace mediatek
}  // namespace impl
}  // namespace power
}  // namespace hardware
}  // namespace android
}  // namespace aidl
