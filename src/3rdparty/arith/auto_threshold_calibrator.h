#pragma once

// 面向生产部署的黑盒阈值标定接口。
// 使用 C++17，不依赖特定的检测模型或合并逻辑实现。

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace auto_threshold {

// 界面灵敏度表，索引为 [camID][labIdx]，取值范围为 [0, 100]。
// _preProcessBox 内部使用 (100 - sensitivity) * 0.01F 作为置信度判定线，
// 因此数值越大，保留原标签的检测框越多。
using SensitivityTable = std::vector<std::vector<int>>;

// 标定器仅使用这些字段构建精确的阈值切换状态。
// 每个事件对应原始 BBox 的 camID、_preProcessBox 使用的 labIdx 和 prob。
struct RawScoreEvent {
    int camera = 0;
    int sensitivity_index = 0;
    double probability = 0.0;
};

// 各物料的原始批次数据应继承此类型。实际实现通常保存按采集顺序排列的
// 四路 vector<BBox>、curRows 以及回放 CatchedBoxMerge 所需的状态。
struct IRawBatch {
    virtual ~IRawBatch() = default;
};

// 黑盒流水线输出的通用基类。实际实现可在派生类中直接保存
// std::vector<myfvKA1Box>，上位机通过派生类型读取最终合并结果。
struct IMergedBatch {
    virtual ~IMergedBatch() = default;
};

struct CalibrationBatch {
    std::string batch_id;
    std::size_t material_count = 0;

    // 人工占比，按类别索引。无需作为黑盒流水线的输入，
    // 但必须与生产环境采用相同的类别定义。
    std::vector<double> human_proportions;

    // 可选的各类别绝对误差容差。为空时自动计算容差。
    std::vector<double> absolute_tolerance;

    // _preProcessBox 判断前的原始分数。必须包含所有可能被灵敏度接受的候选，
    // 包括当前阈值下已被置为 good 的候选。
    std::vector<RawScoreEvent> score_events;

    // 由 IBlackBoxPipeline 使用的物料特定原始 BBox 批次载荷。
    std::shared_ptr<const IRawBatch> raw;

    // 可选的批次权重。<= 0 时标定器按 material_count 自动计算权重。
    double weight = 0.0;
};

struct SensitivityBounds {
    int lower = 0;
    int upper = 100;
};

struct CalibrationConfig {
    int camera_count = 4;
    // 上位机最终统计的类别数量。
    int class_count = 0;
    int good_class = -1;
    // 每个相机界面的灵敏度槽位数量；0 表示与 class_count 相同。
    int sensitivity_count = 0;
    // good 对应的灵敏度槽位；-1 表示不自动排除任何槽位。
    int good_sensitivity_index = -1;

    // 为空时，除 good_sensitivity_index 外的所有灵敏度槽位都参与标定。
    std::vector<bool> calibrate_sensitivity;

    // 当前界面灵敏度。缺失维度默认使用 50。
    SensitivityTable baseline_sensitivities;

    // 可选的相机-灵敏度槽位边界，闭区间 [lower, upper]。
    std::vector<std::vector<SensitivityBounds>> sensitivity_bounds;

    // 可选的重要性权重。缺失值默认使用 1。
    std::vector<double> class_weights;
    std::vector<double> batch_weights;

    // 容差策略。批次自身的 absolute_tolerance 优先。
    bool auto_tolerance = true;
    double default_absolute_tolerance = 0.0;
    double manual_tolerance_floor = 0.005;
    double rare_class_count_floor = 3.0;
    double confidence_z = 1.96;

    // 搜索控制参数。候选数为 0 表示对该变量的候选点进行全量扫描。
    std::size_t coarse_candidate_limit = 81;
    int coarse_sweeps = 8;
    int exact_sweeps = 4;
    int multi_start_count = 8;
    std::uint32_t random_seed = 42;

    // 最差归一化误差相同时的次级排序项。
    double mean_error_weight = 0.10;
    double baseline_distance_weight = 0.01;
    double camera_spread_weight = 0.02;
};

struct MergeCounts {
    // 每个类别一个计数，含义必须与生产统计口径一致。
    // 互斥输出时，其总和通常等于 material_count；
    // 多标签物料的总和可以合法地大于 material_count。
    std::vector<double> category_counts;
};

class IBlackBoxPipeline {
public:
    virtual ~IBlackBoxPipeline() = default;

    // 将 sensitivities[camID][labIdx] 临时写入 strctAI.sens，完整回放：
    // _preProcessBox x 4 -> CurBoxMerge x 4 -> CatchedBoxMerge
    // -> 物料专用合并接口。
    // 不得修改调用方的原始载荷；每次调用必须从同一初始状态开始。
    virtual std::shared_ptr<const IMergedBatch> run(
        const IRawBatch& raw,
        const SensitivityTable& sensitivities) const = 0;
};

// 上位机类别统计适配器。必须复用上位机对最终 vector<myfvKA1Box>
// 的实际统计口径，不能在标定器中另写一套简化计数逻辑。
class ICategoryCounter {
public:
    virtual ~ICategoryCounter() = default;
    virtual MergeCounts count_categories(
        const IMergedBatch& merged,
        std::size_t material_count) const = 0;
};

// 已有流水线封装为回调时使用的便捷适配器。
class FunctionBlackBoxPipeline final : public IBlackBoxPipeline {
public:
    using Callback = std::function<std::shared_ptr<const IMergedBatch>(
        const IRawBatch&, const SensitivityTable&)>;

    explicit FunctionBlackBoxPipeline(Callback callback);
    std::shared_ptr<const IMergedBatch> run(
        const IRawBatch& raw,
        const SensitivityTable& sensitivities) const override;

private:
    Callback callback_;
};

// 已有上位机统计封装为回调时使用的便捷适配器。
class FunctionCategoryCounter final : public ICategoryCounter {
public:
    using Callback = std::function<MergeCounts(
        const IMergedBatch&, std::size_t)>;

    explicit FunctionCategoryCounter(Callback callback);
    MergeCounts count_categories(
        const IMergedBatch& merged,
        std::size_t material_count) const override;

private:
    Callback callback_;
};

struct BatchReport {
    std::string batch_id;
    std::vector<double> model_proportions;
    std::vector<double> human_proportions;
    std::vector<double> signed_error;
    double max_absolute_error = 0.0;
    bool passed = false;
};

struct FeasibleRange {
    int lower = 0;
    int upper = 0;
};

struct DimensionReport {
    int camera = 0;
    int sensitivity_index = 0;
    int search_sensitivity = 0;
    int deployment_sensitivity = 0;
    bool calibrated = false;
    std::size_t candidate_count = 0;

    // 这是条件可行区间：扫描当前维度时，其他维度固定为最终解。
    std::vector<FeasibleRange> conditional_feasible_ranges;
};

// 某个灵敏度表对应的最终黑盒输出，上位机可将 merged_batch 转回
// 自己定义的派生类型并取得 std::vector<myfvKA1Box>。
struct MergedBatchOutput {
    std::string batch_id;
    SensitivityTable sensitivities;
    std::shared_ptr<const IMergedBatch> merged_batch;
};

// 使用指定四相机灵敏度执行一次完整黑盒回放的结果。
struct BatchThresholdEvaluation {
    std::string batch_id;
    SensitivityTable sensitivities;
    std::shared_ptr<const IMergedBatch> merged_batch;
    std::vector<double> category_counts;
    std::vector<double> category_proportions;
};

// 单个批次独立标定结果，用于判断各批次自身是否存在可行灵敏度，
// 以及分析不同批次的灵敏度漂移。
struct PerBatchCalibrationResult {
    std::string batch_id;
    bool feasible = false;
    double worst_normalized_error = 0.0;
    double weighted_mean_normalized_error = 0.0;
    SensitivityTable search_sensitivities;
    SensitivityTable recommended_sensitivities;
    BatchReport report;
    std::vector<DimensionReport> dimensions;
    std::shared_ptr<const IMergedBatch> merged_batch;
    std::vector<std::string> warnings;
};

struct CalibrationResult {
    bool feasible = false;
    double worst_normalized_error = 0.0;
    double weighted_mean_normalized_error = 0.0;
    SensitivityTable search_sensitivities;
    // 可直接写入 strctAI.sens[camID][sensIdx] 的统一部署值。
    SensitivityTable deployment_sensitivities;
    std::vector<BatchReport> batch_reports;
    std::vector<DimensionReport> dimensions;
    // 联合部署灵敏度在每个批次上的最终 vector<myfvKA1Box> 包装结果。
    std::vector<MergedBatchOutput> deployment_outputs;
    // 每个批次分别独立标定得到的灵敏度与最终输出。
    std::vector<PerBatchCalibrationResult> per_batch_results;
    std::vector<std::string> warnings;
};

class AutoThresholdCalibrator {
public:
    AutoThresholdCalibrator(CalibrationConfig config,
                            const IBlackBoxPipeline& pipeline,
                            const ICategoryCounter& counter);

    // 输入格式错误时抛出 std::invalid_argument。黑盒流水线或上位机统计
    // 适配器抛出的物料特定异常会透传给调用方，不在此处吞掉。
    CalibrationResult calibrate(
        const std::vector<CalibrationBatch>& batches) const;

    // 不搜索灵敏度，仅使用调用方给定的界面灵敏度执行一次黑盒流水线。
    // 返回的 merged_batch 可由上位机转回实际的 vector<myfvKA1Box> 包装类型。
    BatchThresholdEvaluation evaluate_batch(
        const CalibrationBatch& batch,
        const SensitivityTable& sensitivities) const;

    // 界面灵敏度转换为 _preProcessBox 实际使用的置信度判定线。
    static float sensitivity_to_probability_threshold(int sensitivity);

    static double suggest_absolute_tolerance(double human_proportion,
                                             std::size_t material_count,
                                             const CalibrationConfig& config);

private:
    struct Impl;
    CalibrationConfig config_;
    const IBlackBoxPipeline& pipeline_;
    const ICategoryCounter& counter_;
};

} // 命名空间 auto_threshold
