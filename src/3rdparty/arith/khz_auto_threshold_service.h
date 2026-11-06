#pragma once

// 兼容旧的包含路径。实现已改为明确直接调用生产接口的版本。
#include "khz_auto_threshold_direct_service.h"

namespace khz_auto_threshold {

template <typename BBoxT,
          typename MergedBoxT,
          typename HomeWidgetT,
          typename GlobalFlowT,
          typename RuntimeT>
using KHzAutoThresholdService = KHzAutoThresholdDirectService<
    BBoxT, MergedBoxT, HomeWidgetT, GlobalFlowT, RuntimeT>;

} // namespace khz_auto_threshold
