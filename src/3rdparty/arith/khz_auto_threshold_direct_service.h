#pragma once

// KHZ 自动阈值标定的直接生产链路调用接口。
// 不修改 _preProcessBox、CurBoxMerge、CatchedBoxMerge 及定制合并函数；
// 对每一组候选灵敏度按生产顺序完整重放这些接口。

#include "auto_threshold_calibrator.h"

#include <array>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace khz_auto_threshold {

constexpr int kCameraCount = 4;

// 同一采集时刻的四相机原始框。必须保存调用 _preProcessBox 前的 BBox。
template <typename BBoxT>
struct KHzRawFrame {
    std::array<std::vector<BBoxT>, kCameraCount> camera_boxes;
    int cur_rows = 0;
};

// 上位机录入的人工化验数据。
struct KHzLaboratoryResult {
    std::string batch_id;
    // 与上位机类别占比使用完全相同分母的物料数量。
    std::size_t material_count = 0;
    // 类别顺序必须与 host_category_count 返回值完全一致，比例范围 [0, 1]。
    std::vector<double> category_proportions;
    // 类别绝对误差容差。为空时使用 CalibrationConfig 的自动容差。
    std::vector<double> absolute_tolerance;
    // 多批次联合优化权重。未设置时按 material_count 自动加权。
    double weight = 0.0;
};

// 每个批次必须保存完整的帧序列；CatchedBoxMerge 依赖其先后顺序。
template <typename BBoxT>
struct KHzBatchInput {
    KHzLaboratoryResult laboratory;
    std::vector<KHzRawFrame<BBoxT>> frames;
};

// 上位机的唯一标定入参：人工化验、当前界面灵敏度、四相机原始 BBox。
template <typename BBoxT>
struct KHzCalibrationRequest {
    std::vector<KHzBatchInput<BBoxT>> batches;
    // [camID][labIdx]，界面灵敏度，范围 0..100。
    auto_threshold::SensitivityTable initial_sensitivities;
};

// 生产链路中的固定配置，不是本次优化变量。
struct KHzPipelineSettings {
    float current_box_iou_threshold = 0.0F;
    float catched_box_iou_threshold = 0.0F;
    // CurBoxMerge 的生产固定 conf 参数。既有生产实现中其阈值判断已被注释，
    // 标定变量仅是 _preProcessBox 读取的 strctAI.sens。
    std::vector<float> fixed_cur_box_conf;
    std::vector<int> label_priority;
    std::vector<int> mutually_exclusive_group;
};

struct KHzBatchCalibrationStatus {
    std::string batch_id;
    bool passed = false;
    std::vector<double> human_proportions;
    std::vector<double> model_proportions;
    std::vector<double> signed_error;
    double max_absolute_error = 0.0;
    // 单批次的参考解，仅用于比较批次间差异，不能自动部署。
    auto_threshold::SensitivityTable per_batch_recommended_sensitivities;
};

template <typename MergedBoxT>
struct KHzBatchMergedOutput {
    std::string batch_id;
    // 使用联合推荐值或联合折中值重放得到的最终合并框。
    std::vector<MergedBoxT> merged_boxes;
};

template <typename MergedBoxT>
struct KHzCalibrationResponse {
    // 所有批次、所有类别均落入容差内时为 true。
    bool passed = false;
    // 仅 passed 时为 true；上位机仍需要用户确认才能写入界面。
    bool may_apply_after_host_confirmation = false;

    // 仅 passed 时填充，服务绝不自行写入这些值。
    auto_threshold::SensitivityTable recommended_sensitivities;
    // 始终填充。未达标时只能显示，禁止自动覆盖界面。
    auto_threshold::SensitivityTable compromise_sensitivities;

    std::vector<KHzBatchCalibrationStatus> batch_statuses;
    std::vector<std::string> failed_batch_ids;
    std::vector<KHzBatchMergedOutput<MergedBoxT>> merged_outputs;
    std::vector<auto_threshold::DimensionReport> dimensions;
    std::vector<std::string> warnings;
    std::string message;
};

// HomeWidgetT 需要提供下面这个公开桥接函数。它只转发到原有私有 _preProcessBox，
// 不得在其中复制阈值或标签映射逻辑：
//   std::vector<BBoxT> preProcessBoxForThresholdCalibration(
//       const std::vector<BBoxT>& boxes, int cam_id) {
//       return _preProcessBox(boxes, cam_id);
//   }
// 若 _preProcessBox 已是 public，也仍建议保留该桥接函数，避免标定服务依赖 UI 类私有性。
//
// RuntimeT 负责保存和临时修改 _preProcessBox 所依赖的生产状态，接口如下：
//   using Snapshot = ...;
//   Snapshot save_state() const;
//   void apply_sensitivities(const auto_threshold::SensitivityTable&);
//   void reset_preprocess_state();
//   void restore_state(const Snapshot&);
//   int sensitivity_index(int cam_id, int bbox_label) const;
// sensitivity_index 必须复用 _preProcessBox 中 vlabNamemap 的 label -> labIdx 映射。

template <typename BBoxT,
          typename MergedBoxT,
          typename HomeWidgetT,
          typename GlobalFlowT,
          typename RuntimeT>
class KHzDirectPipeline final : public auto_threshold::IBlackBoxPipeline {
public:
    struct RawPayload final : auto_threshold::IRawBatch {
        explicit RawPayload(const KHzBatchInput<BBoxT>* value)
            : source(value) {}
        const KHzBatchInput<BBoxT>* source = nullptr;
    };

    struct MergedPayload final : auto_threshold::IMergedBatch {
        std::vector<MergedBoxT> boxes;
    };

    // 更换物料时只替换此回调。例如 KHZ：
    // [](GlobalFlow& flow, std::vector<myfvKA1Box>& boxes,
    //    const KHzPipelineSettings& s) {
    //     flow.KHZMergedBoxProcess(boxes, s.label_priority,
    //                               s.mutually_exclusive_group, kCameraCount);
    // }
    using CustomMerge = std::function<void(
        GlobalFlowT&, std::vector<MergedBoxT>&, const KHzPipelineSettings&)>;

    // 与生产端输出时机完全一致的收集规则。默认仅收集定制合并后 count_flag == 1 的新物料。
    // 如果上位机在其他时机消费 vecFvKA1Boxes，应传入生产端同口径的回调。
    using OutputCollector = std::function<void(
        const std::vector<MergedBoxT>&, std::vector<MergedBoxT>&)>;

    KHzDirectPipeline(HomeWidgetT& home_widget,
                      GlobalFlowT& global_flow,
                      RuntimeT& runtime,
                      KHzPipelineSettings settings,
                      CustomMerge custom_merge,
                      OutputCollector output_collector = {})
        : home_widget_(home_widget),
          global_flow_(global_flow),
          runtime_(runtime),
          settings_(std::move(settings)),
          custom_merge_(std::move(custom_merge)),
          output_collector_(std::move(output_collector)) {
        if (!custom_merge_) {
            throw std::invalid_argument("必须提供物料定制合并接口");
        }
        if (!output_collector_) {
            output_collector_ = [](const std::vector<MergedBoxT>& current,
                                   std::vector<MergedBoxT>& output) {
                for (const MergedBoxT& box : current) {
                    if (box.count_flag == 1) {
                        output.push_back(box);
                    }
                }
            };
        }
    }

    std::shared_ptr<const auto_threshold::IMergedBatch> run(
        const auto_threshold::IRawBatch& raw,
        const auto_threshold::SensitivityTable& sensitivities) const override {
        const auto* payload = dynamic_cast<const RawPayload*>(&raw);
        if (payload == nullptr || payload->source == nullptr) {
            throw std::invalid_argument("KHZ 原始 BBox 负载类型不匹配");
        }

        const typename RuntimeT::Snapshot saved = runtime_.save_state();
        try {
            // 仅临时写入内存；无论结果或异常，都会在退出时恢复现场。
            runtime_.apply_sensitivities(sensitivities);
            runtime_.reset_preprocess_state();

            auto output = std::make_shared<MergedPayload>();
            std::vector<MergedBoxT> catched_boxes;

            for (const KHzRawFrame<BBoxT>& frame : payload->source->frames) {
                std::vector<MergedBoxT> current_boxes;

                // 1) _preProcessBox x4；2) CurBoxMerge x4。
                for (int cam_id = 0; cam_id < kCameraCount; ++cam_id) {
                    const std::vector<BBoxT> original_boxes =
                        frame.camera_boxes[static_cast<std::size_t>(cam_id)];
                    std::vector<BBoxT> preprocessed_boxes =
                        home_widget_.preProcessBoxForThresholdCalibration(
                            original_boxes, cam_id);
                    global_flow_.CurBoxMerge(
                        current_boxes, preprocessed_boxes, cam_id,
                        settings_.current_box_iou_threshold,
                        settings_.fixed_cur_box_conf);
                }

                // 3) CatchedBoxMerge；4) 当前物料的定制合并接口。
                global_flow_.CatchedBoxMerge(
                    current_boxes, catched_boxes,
                    settings_.catched_box_iou_threshold, frame.cur_rows);
                custom_merge_(global_flow_, current_boxes, settings_);

                output_collector_(current_boxes, output->boxes);
                // 下一帧 CatchedBoxMerge 的历史输入。
                catched_boxes = current_boxes;
            }

            runtime_.restore_state(saved);
            return output;
        } catch (...) {
            runtime_.restore_state(saved);
            throw;
        }
    }

private:
    HomeWidgetT& home_widget_;
    GlobalFlowT& global_flow_;
    RuntimeT& runtime_;
    KHzPipelineSettings settings_;
    CustomMerge custom_merge_;
    OutputCollector output_collector_;
};

// 必须直接调用上位机当前的类别统计逻辑，返回“类别数量”而不是比例。
// 这避免在标定器中简化解析 myfvKA1Box::label，造成多标签/优先级/分母不一致。
template <typename MergedBoxT>
using HostCategoryCount = std::function<std::vector<double>(
    const std::vector<MergedBoxT>& merged_boxes, std::size_t material_count)>;

// 上位机调用服务。calibrate() 不会持久化任何候选灵敏度，也不会覆盖界面。
template <typename BBoxT,
          typename MergedBoxT,
          typename HomeWidgetT,
          typename GlobalFlowT,
          typename RuntimeT>
class KHzAutoThresholdDirectService {
private:
    using Pipeline = KHzDirectPipeline<
        BBoxT, MergedBoxT, HomeWidgetT, GlobalFlowT, RuntimeT>;
    using RawPayload = typename Pipeline::RawPayload;
    using MergedPayload = typename Pipeline::MergedPayload;

    class HostCategoryCounter final : public auto_threshold::ICategoryCounter {
    public:
        explicit HostCategoryCounter(HostCategoryCount<MergedBoxT> count_function)
            : count_function_(std::move(count_function)) {
            if (!count_function_) {
                throw std::invalid_argument("必须提供上位机类别统计函数");
            }
        }

        auto_threshold::MergeCounts count_categories(
            const auto_threshold::IMergedBatch& merged,
            std::size_t material_count) const override {
            const auto* payload = dynamic_cast<const MergedPayload*>(&merged);
            if (payload == nullptr) {
                throw std::invalid_argument("KHZ 合并框负载类型不匹配");
            }
            return {count_function_(payload->boxes, material_count)};
        }

    private:
        HostCategoryCount<MergedBoxT> count_function_;
    };

public:
    using CustomMerge = typename Pipeline::CustomMerge;
    using OutputCollector = typename Pipeline::OutputCollector;

    KHzAutoThresholdDirectService(
        HomeWidgetT& home_widget,
        GlobalFlowT& global_flow,
        RuntimeT& runtime,
        std::mutex& production_state_mutex,
        auto_threshold::CalibrationConfig optimizer_config,
        KHzPipelineSettings pipeline_settings,
        CustomMerge custom_merge,
        HostCategoryCount<MergedBoxT> host_category_count,
        OutputCollector output_collector = {})
        : runtime_(runtime),
          production_state_mutex_(production_state_mutex),
          optimizer_config_(std::move(optimizer_config)),
          pipeline_(home_widget, global_flow, runtime, std::move(pipeline_settings),
                    std::move(custom_merge), std::move(output_collector)),
          counter_(std::move(host_category_count)) {
        optimizer_config_.camera_count = kCameraCount;
    }

    KHzCalibrationResponse<MergedBoxT> calibrate(
        const KHzCalibrationRequest<BBoxT>& request) {
        // _preProcessBox 会读写 strctAI.sens / vCamLabelMapping，标定期间禁止与生产识别并发。
        std::lock_guard<std::mutex> lock(production_state_mutex_);
        validate_request(request);

        std::vector<auto_threshold::CalibrationBatch> core_batches;
        core_batches.reserve(request.batches.size());
        for (const KHzBatchInput<BBoxT>& source : request.batches) {
            auto_threshold::CalibrationBatch batch;
            batch.batch_id = source.laboratory.batch_id;
            batch.material_count = source.laboratory.material_count;
            batch.human_proportions = source.laboratory.category_proportions;
            batch.absolute_tolerance = source.laboratory.absolute_tolerance;
            batch.weight = source.laboratory.weight;
            batch.raw = std::make_shared<RawPayload>(&source);

            // 仅用于报告未覆盖的灵敏度维度；目标函数仍由完整生产链路重放计算。
            for (const KHzRawFrame<BBoxT>& frame : source.frames) {
                for (int cam_id = 0; cam_id < kCameraCount; ++cam_id) {
                    for (const BBoxT& box :
                         frame.camera_boxes[static_cast<std::size_t>(cam_id)]) {
                        const int sensitivity_index =
                            runtime_.sensitivity_index(cam_id, box.label);
                        if (sensitivity_index >= 0) {
                            batch.score_events.push_back({
                                cam_id, sensitivity_index,
                                static_cast<double>(box.prob)});
                        }
                    }
                }
            }
            core_batches.push_back(std::move(batch));
        }

        auto_threshold::CalibrationConfig config = optimizer_config_;
        config.camera_count = kCameraCount;
        config.baseline_sensitivities = request.initial_sensitivities;

        auto_threshold::AutoThresholdCalibrator calibrator(
            config, pipeline_, counter_);
        return make_response(calibrator.calibrate(core_batches));
    }

private:
    void validate_request(const KHzCalibrationRequest<BBoxT>& request) const {
        if (request.batches.empty()) {
            throw std::invalid_argument("至少需要一批 KHZ 标定数据");
        }
        if (request.initial_sensitivities.size() !=
            static_cast<std::size_t>(kCameraCount)) {
            throw std::invalid_argument("初始灵敏度必须包含四个相机");
        }
        const int sensitivity_count = optimizer_config_.sensitivity_count > 0
            ? optimizer_config_.sensitivity_count : optimizer_config_.class_count;
        if (sensitivity_count <= 0) {
            throw std::invalid_argument("sensitivity_count 或 class_count 必须大于零");
        }
        for (const auto& camera_sensitivities : request.initial_sensitivities) {
            if (static_cast<int>(camera_sensitivities.size()) != sensitivity_count) {
                throw std::invalid_argument(
                    "每个相机的灵敏度数量必须与 sensitivity_count 一致");
            }
            for (int sensitivity : camera_sensitivities) {
                if (sensitivity < 0 || sensitivity > 100) {
                    throw std::invalid_argument("界面灵敏度必须在 [0, 100] 内");
                }
            }
        }
        for (const KHzBatchInput<BBoxT>& batch : request.batches) {
            if (batch.laboratory.batch_id.empty()) {
                throw std::invalid_argument("批次编号不能为空");
            }
            if (batch.laboratory.material_count == 0U) {
                throw std::invalid_argument("人工化验批次的物料数量必须大于零");
            }
            if (batch.frames.empty()) {
                throw std::invalid_argument("每个批次至少需要一帧四相机原始 BBox");
            }
        }
    }

    static KHzCalibrationResponse<MergedBoxT> make_response(
        const auto_threshold::CalibrationResult& source) {
        KHzCalibrationResponse<MergedBoxT> response;
        response.passed = source.feasible;
        response.may_apply_after_host_confirmation = source.feasible;
        response.compromise_sensitivities = source.deployment_sensitivities;
        if (source.feasible) {
            response.recommended_sensitivities = source.deployment_sensitivities;
            response.message =
                "所有批次均达标：上位机确认后可将推荐灵敏度写入界面。";
        } else {
            response.message =
                "联合标定未达标：仅返回折中灵敏度和未达标批次，禁止自动覆盖界面。";
        }

        response.dimensions = source.dimensions;
        response.warnings = source.warnings;
        response.batch_statuses.reserve(source.batch_reports.size());
        for (std::size_t index = 0; index < source.batch_reports.size(); ++index) {
            const auto_threshold::BatchReport& report = source.batch_reports[index];
            KHzBatchCalibrationStatus status;
            status.batch_id = report.batch_id;
            status.passed = report.passed;
            status.human_proportions = report.human_proportions;
            status.model_proportions = report.model_proportions;
            status.signed_error = report.signed_error;
            status.max_absolute_error = report.max_absolute_error;
            if (index < source.per_batch_results.size()) {
                status.per_batch_recommended_sensitivities =
                    source.per_batch_results[index].recommended_sensitivities;
            }
            if (!status.passed) {
                response.failed_batch_ids.push_back(status.batch_id);
            }
            response.batch_statuses.push_back(std::move(status));
        }

        response.merged_outputs.reserve(source.deployment_outputs.size());
        for (const auto_threshold::MergedBatchOutput& core_output :
             source.deployment_outputs) {
            const auto* payload =
                dynamic_cast<const MergedPayload*>(core_output.merged_batch.get());
            if (payload == nullptr) {
                throw std::runtime_error("KHZ 部署合并框负载类型不匹配");
            }
            KHzBatchMergedOutput<MergedBoxT> output;
            output.batch_id = core_output.batch_id;
            output.merged_boxes = payload->boxes;
            response.merged_outputs.push_back(std::move(output));
        }
        return response;
    }

    RuntimeT& runtime_;
    std::mutex& production_state_mutex_;
    auto_threshold::CalibrationConfig optimizer_config_;
    Pipeline pipeline_;
    HostCategoryCounter counter_;
};

} // namespace khz_auto_threshold
