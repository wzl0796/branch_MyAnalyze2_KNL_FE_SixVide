#include "auto_threshold_calibrator.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <random>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace auto_threshold {
namespace {

constexpr double kEpsilon = 1e-12;

bool is_finite(double value) {
    return std::isfinite(value) != 0;
}

struct SearchState {
    std::vector<std::size_t> candidate_index;

    bool operator==(const SearchState& other) const {
        return candidate_index == other.candidate_index;
    }
};

struct SearchStateHash {
    std::size_t operator()(const SearchState& state) const {
        std::size_t seed = state.candidate_index.size();
        for (std::size_t value : state.candidate_index) {
            seed ^= value + static_cast<std::size_t>(0x9e3779b9U) +
                    (seed << 6U) + (seed >> 2U);
        }
        return seed;
    }
};

struct Evaluation {
    std::vector<std::vector<double>> model_proportions;
    std::vector<std::vector<double>> signed_errors;
    std::vector<double> batch_max_absolute_error;
    double worst_normalized_error = std::numeric_limits<double>::infinity();
    double weighted_mean_normalized_error = std::numeric_limits<double>::infinity();
    double stability_penalty = std::numeric_limits<double>::infinity();
    double secondary_score = std::numeric_limits<double>::infinity();
};

bool better(const Evaluation& lhs, const Evaluation& rhs) {
    if (lhs.worst_normalized_error + kEpsilon < rhs.worst_normalized_error) {
        return true;
    }
    if (rhs.worst_normalized_error + kEpsilon < lhs.worst_normalized_error) {
        return false;
    }

    return lhs.secondary_score + kEpsilon < rhs.secondary_score;
}

std::vector<std::size_t> scan_indices(std::size_t size,
                                      std::size_t limit,
                                      std::size_t current) {
    std::vector<std::size_t> out;
    if (size == 0) {
        return out;
    }
    if (limit == 0 || size <= limit) {
        out.resize(size);
        std::iota(out.begin(), out.end(), 0U);
        return out;
    }
    if (limit == 1) {
        out.push_back(current);
        return out;
    }

    out.reserve(limit + 1U);
    for (std::size_t i = 0; i < limit; ++i) {
        const double position = static_cast<double>(i) *
                                static_cast<double>(size - 1U) /
                                static_cast<double>(limit - 1U);
        out.push_back(static_cast<std::size_t>(std::llround(position)));
    }
    out.push_back(current);
    std::sort(out.begin(), out.end());
    out.erase(std::unique(out.begin(), out.end()), out.end());
    return out;
}

} // 匿名命名空间

FunctionBlackBoxPipeline::FunctionBlackBoxPipeline(Callback callback)
    : callback_(std::move(callback)) {
    if (!callback_) {
        throw std::invalid_argument("FunctionBlackBoxPipeline callback is empty");
    }
}

std::shared_ptr<const IMergedBatch> FunctionBlackBoxPipeline::run(
    const IRawBatch& raw,
    const SensitivityTable& sensitivities) const {
    return callback_(raw, sensitivities);
}

FunctionCategoryCounter::FunctionCategoryCounter(Callback callback)
    : callback_(std::move(callback)) {
    if (!callback_) {
        throw std::invalid_argument("FunctionCategoryCounter callback is empty");
    }
}

MergeCounts FunctionCategoryCounter::count_categories(
    const IMergedBatch& merged,
    std::size_t material_count) const {
    return callback_(merged, material_count);
}

struct AutoThresholdCalibrator::Impl {
    Impl(const CalibrationConfig& config,
         const IBlackBoxPipeline& pipeline,
         const ICategoryCounter& counter,
         const std::vector<CalibrationBatch>& batches)
        : config(config), pipeline(pipeline), counter(counter), batches(batches) {
        initialize();
    }

    const CalibrationConfig& config;
    const IBlackBoxPipeline& pipeline;
    const ICategoryCounter& counter;
    const std::vector<CalibrationBatch>& batches;

    int sensitivity_count = 0;
    SensitivityTable baseline;
    std::vector<std::vector<double>> tolerances;
    std::vector<double> batch_weights;
    std::vector<bool> calibrate_sensitivity;
    std::vector<bool> calibrate_dimension;
    std::vector<std::vector<int>> candidates;
    std::vector<std::size_t> variable_dimensions;
    std::vector<std::string> warnings;
    std::unordered_map<SearchState, Evaluation, SearchStateHash> cache;

    std::size_t flat(int camera, int sensitivity_index) const {
        return static_cast<std::size_t>(camera) *
               static_cast<std::size_t>(sensitivity_count) +
               static_cast<std::size_t>(sensitivity_index);
    }

    int camera_of(std::size_t dimension) const {
        return static_cast<int>(dimension /
            static_cast<std::size_t>(sensitivity_count));
    }

    int sensitivity_of(std::size_t dimension) const {
        return static_cast<int>(dimension %
            static_cast<std::size_t>(sensitivity_count));
    }

    SensitivityBounds bounds_for(int camera, int sensitivity_index) const {
        if (config.sensitivity_bounds.empty()) {
            return SensitivityBounds{};
        }
        return config.sensitivity_bounds[static_cast<std::size_t>(camera)]
                                        [static_cast<std::size_t>(sensitivity_index)];
    }

    void initialize() {
        if (config.camera_count <= 0 || config.class_count <= 0) {
            throw std::invalid_argument("camera_count and class_count must be positive");
        }
        sensitivity_count = config.sensitivity_count > 0
            ? config.sensitivity_count : config.class_count;
        if (!is_finite(config.mean_error_weight) || config.mean_error_weight < 0.0 ||
            !is_finite(config.baseline_distance_weight) || config.baseline_distance_weight < 0.0 ||
            !is_finite(config.camera_spread_weight) || config.camera_spread_weight < 0.0) {
            throw std::invalid_argument("search tie-break weights must be finite and non-negative");
        }
        if (batches.empty()) {
            throw std::invalid_argument("at least one calibration batch is required");
        }
        if (config.good_class < -1 || config.good_class >= config.class_count) {
            throw std::invalid_argument("good_class is outside the class range");
        }
        if (config.good_sensitivity_index < -1 ||
            config.good_sensitivity_index >= sensitivity_count) {
            throw std::invalid_argument(
                "good_sensitivity_index is outside the sensitivity range");
        }
        if (!config.calibrate_sensitivity.empty() &&
            static_cast<int>(config.calibrate_sensitivity.size()) != sensitivity_count) {
            throw std::invalid_argument(
                "calibrate_sensitivity size does not match sensitivity_count");
        }
        if (!config.class_weights.empty() &&
            static_cast<int>(config.class_weights.size()) != config.class_count) {
            throw std::invalid_argument("class_weights size does not match class_count");
        }
        if (!config.batch_weights.empty() &&
            config.batch_weights.size() != batches.size()) {
            throw std::invalid_argument("batch_weights size does not match batch count");
        }
        if (!config.sensitivity_bounds.empty()) {
            if (static_cast<int>(config.sensitivity_bounds.size()) != config.camera_count) {
                throw std::invalid_argument(
                    "sensitivity_bounds camera dimension does not match camera_count");
            }
            for (const auto& row : config.sensitivity_bounds) {
                if (static_cast<int>(row.size()) != sensitivity_count) {
                    throw std::invalid_argument(
                        "sensitivity_bounds slot dimension does not match sensitivity_count");
                }
            }
        }

        calibrate_sensitivity.assign(static_cast<std::size_t>(sensitivity_count), true);
        if (!config.calibrate_sensitivity.empty()) {
            calibrate_sensitivity = config.calibrate_sensitivity;
        } else if (config.good_sensitivity_index >= 0) {
            calibrate_sensitivity[
                static_cast<std::size_t>(config.good_sensitivity_index)] = false;
        }

        baseline.assign(static_cast<std::size_t>(config.camera_count),
                        std::vector<int>(static_cast<std::size_t>(sensitivity_count), 50));
        for (int camera = 0; camera < config.camera_count; ++camera) {
            for (int sensitivity_index = 0;
                 sensitivity_index < sensitivity_count;
                 ++sensitivity_index) {
                const auto& source = config.baseline_sensitivities;
                if (static_cast<std::size_t>(camera) < source.size() &&
                    static_cast<std::size_t>(sensitivity_index) <
                        source[static_cast<std::size_t>(camera)].size()) {
                    const int configured = source[static_cast<std::size_t>(camera)]
                                                 [static_cast<std::size_t>(sensitivity_index)];
                    if (configured < 0 || configured > 100) {
                        throw std::invalid_argument(
                            "baseline sensitivities must be inside [0, 100]");
                    }
                    baseline[static_cast<std::size_t>(camera)]
                            [static_cast<std::size_t>(sensitivity_index)] = configured;
                }
                const SensitivityBounds limit = bounds_for(camera, sensitivity_index);
                if (limit.lower < 0 || limit.upper > 100 ||
                    limit.lower > limit.upper) {
                    throw std::invalid_argument(
                        "each sensitivity bound must be inside [0, 100]");
                }
                baseline[static_cast<std::size_t>(camera)]
                        [static_cast<std::size_t>(sensitivity_index)] =
                    std::max(limit.lower, std::min(limit.upper,
                        baseline[static_cast<std::size_t>(camera)]
                                [static_cast<std::size_t>(sensitivity_index)]));
            }
        }

        validate_batches_and_build_tolerances();
        build_candidate_domains();
    }

    void validate_batches_and_build_tolerances() {
        tolerances.assign(batches.size(),
                          std::vector<double>(static_cast<std::size_t>(config.class_count), 0.0));
        batch_weights.resize(batches.size(), 1.0);

        std::size_t total_materials = 0;
        for (const CalibrationBatch& batch : batches) {
            if (!batch.raw) {
                throw std::invalid_argument("every calibration batch needs a raw payload");
            }
            if (batch.material_count == 0) {
                throw std::invalid_argument("every calibration batch needs material_count > 0");
            }
            if (static_cast<int>(batch.human_proportions.size()) != config.class_count) {
                throw std::invalid_argument("human_proportions size does not match class_count");
            }
            if (!batch.absolute_tolerance.empty() &&
                static_cast<int>(batch.absolute_tolerance.size()) != config.class_count) {
                throw std::invalid_argument("absolute_tolerance size does not match class_count");
            }
            for (double proportion : batch.human_proportions) {
                if (!is_finite(proportion) || proportion < 0.0 || proportion > 1.0) {
                    throw std::invalid_argument("human proportions must be finite values in [0, 1]");
                }
            }
            for (const RawScoreEvent& event : batch.score_events) {
                if (event.camera < 0 || event.camera >= config.camera_count ||
                    event.sensitivity_index < 0 ||
                    event.sensitivity_index >= sensitivity_count ||
                    !is_finite(event.probability) || event.probability < 0.0 || event.probability > 1.0) {
                    throw std::invalid_argument("a score event is outside its configured range");
                }
            }
            total_materials += batch.material_count;
        }

        for (std::size_t batch_index = 0; batch_index < batches.size(); ++batch_index) {
            const CalibrationBatch& batch = batches[batch_index];
            if (!config.batch_weights.empty()) {
                batch_weights[batch_index] = config.batch_weights[batch_index];
            } else if (batch.weight > 0.0) {
                batch_weights[batch_index] = batch.weight;
            } else {
                batch_weights[batch_index] = static_cast<double>(batch.material_count) /
                                             static_cast<double>(total_materials);
            }
            if (!is_finite(batch_weights[batch_index]) || batch_weights[batch_index] <= 0.0) {
                throw std::invalid_argument("batch weights must be finite and positive");
            }

            for (int class_id = 0; class_id < config.class_count; ++class_id) {
                double tolerance = 0.0;
                if (!batch.absolute_tolerance.empty()) {
                    tolerance = batch.absolute_tolerance[static_cast<std::size_t>(class_id)];
                } else if (config.default_absolute_tolerance > 0.0) {
                    tolerance = config.default_absolute_tolerance;
                } else if (config.auto_tolerance) {
                    tolerance = AutoThresholdCalibrator::suggest_absolute_tolerance(
                        batch.human_proportions[static_cast<std::size_t>(class_id)],
                        batch.material_count, config);
                } else {
                    throw std::invalid_argument(
                        "a tolerance is required when auto_tolerance is disabled");
                }
                if (!is_finite(tolerance) || tolerance <= 0.0 || tolerance > 1.0) {
                    throw std::invalid_argument("each tolerance must be in (0, 1]");
                }
                tolerances[batch_index][static_cast<std::size_t>(class_id)] = tolerance;
            }
        }
    }

    void build_candidate_domains() {
        const std::size_t dimension_count =
            static_cast<std::size_t>(config.camera_count) *
            static_cast<std::size_t>(sensitivity_count);
        candidates.assign(dimension_count, {});
        calibrate_dimension.assign(dimension_count, false);

        for (int camera = 0; camera < config.camera_count; ++camera) {
            for (int sensitivity_index = 0;
                 sensitivity_index < sensitivity_count;
                 ++sensitivity_index) {
                const std::size_t dimension = flat(camera, sensitivity_index);
                const SensitivityBounds limit = bounds_for(camera, sensitivity_index);
                std::vector<int>& values = candidates[dimension];

                // strctAI.sens 是整数 0..100。完整枚举这一离散域可以精确复现
                // float sensVal 和 prob < sensVal 的边界，不依赖近似换算。
                for (int value = limit.lower; value <= limit.upper; ++value) {
                    values.push_back(value);
                }

                if (!calibrate_sensitivity[
                        static_cast<std::size_t>(sensitivity_index)]) {
                    values.assign(1U, baseline[static_cast<std::size_t>(camera)]
                                           [static_cast<std::size_t>(sensitivity_index)]);
                } else {
                    calibrate_dimension[dimension] = true;
                    variable_dimensions.push_back(dimension);
                    bool has_score_event = false;
                    for (const CalibrationBatch& batch : batches) {
                        for (const RawScoreEvent& event : batch.score_events) {
                            if (event.camera == camera &&
                                event.sensitivity_index == sensitivity_index) {
                                has_score_event = true;
                                break;
                            }
                        }
                        if (has_score_event) {
                            break;
                        }
                    }
                    if (!has_score_event) {
                        std::ostringstream message;
                        message << "camera " << camera << ", sensitivity slot "
                                << sensitivity_index
                                << " has no raw score event; its sensitivity is not identifiable";
                        warnings.push_back(message.str());
                        // 该维度不会进入 _preProcessBox 的对应阈值比较；固定为初始值，
                        // 避免无意义地枚举 0..100 并反复重放整条生产链路。
                        values.assign(1U, baseline[static_cast<std::size_t>(camera)]
                                                   [static_cast<std::size_t>(sensitivity_index)]);
                        calibrate_dimension[dimension] = false;
                        variable_dimensions.pop_back();
                    }
                }
            }
        }
    }

    std::size_t equivalent_candidate_index(std::size_t dimension, int value) const {
        const std::vector<int>& values = candidates[dimension];
        const auto it = std::lower_bound(values.begin(), values.end(), value);
        if (it == values.end()) {
            return values.size() - 1U;
        }
        return static_cast<std::size_t>(std::distance(values.begin(), it));
    }

    SearchState baseline_state() const {
        SearchState state;
        state.candidate_index.resize(candidates.size(), 0U);
        for (int camera = 0; camera < config.camera_count; ++camera) {
            for (int sensitivity_index = 0;
                 sensitivity_index < sensitivity_count;
                 ++sensitivity_index) {
                const std::size_t dimension = flat(camera, sensitivity_index);
                state.candidate_index[dimension] = equivalent_candidate_index(
                    dimension, baseline[static_cast<std::size_t>(camera)]
                                       [static_cast<std::size_t>(sensitivity_index)]);
            }
        }
        return state;
    }

    SearchState constant_state(int value) const {
        SearchState state = baseline_state();
        for (std::size_t dimension : variable_dimensions) {
            state.candidate_index[dimension] = equivalent_candidate_index(dimension, value);
        }
        return state;
    }

    SensitivityTable sensitivities_for(const SearchState& state) const {
        SensitivityTable sensitivities = baseline;
        for (std::size_t dimension : variable_dimensions) {
            const int camera = camera_of(dimension);
            const int sensitivity_index = sensitivity_of(dimension);
            sensitivities[static_cast<std::size_t>(camera)]
                         [static_cast<std::size_t>(sensitivity_index)] =
                candidates[dimension][state.candidate_index[dimension]];
        }
        return sensitivities;
    }

    const Evaluation& evaluate(const SearchState& state) {
        const auto cached = cache.find(state);
        if (cached != cache.end()) {
            return cached->second;
        }

        const SensitivityTable sensitivities = sensitivities_for(state);
        Evaluation evaluation;
        evaluation.model_proportions.resize(batches.size());
        evaluation.signed_errors.resize(batches.size());
        evaluation.batch_max_absolute_error.assign(batches.size(), 0.0);

        double weighted_sum = 0.0;
        double total_weight = 0.0;
        evaluation.worst_normalized_error = 0.0;
        for (std::size_t batch_index = 0; batch_index < batches.size(); ++batch_index) {
            const CalibrationBatch& batch = batches[batch_index];
            const std::shared_ptr<const IMergedBatch> merged =
                pipeline.run(*batch.raw, sensitivities);
            if (!merged) {
                throw std::runtime_error("black-box pipeline returned a null merged batch");
            }
            const MergeCounts counts = counter.count_categories(
                *merged, batch.material_count);
            if (static_cast<int>(counts.category_counts.size()) != config.class_count) {
                throw std::runtime_error("category counter returned a count vector with the wrong class count");
            }

            auto& proportions = evaluation.model_proportions[batch_index];
            auto& errors = evaluation.signed_errors[batch_index];
            proportions.resize(static_cast<std::size_t>(config.class_count));
            errors.resize(static_cast<std::size_t>(config.class_count));
            for (int class_id = 0; class_id < config.class_count; ++class_id) {
                const double count = counts.category_counts[static_cast<std::size_t>(class_id)];
                if (!is_finite(count) || count < 0.0) {
                    throw std::runtime_error("category counter returned an invalid category count");
                }
                const double proportion = count / static_cast<double>(batch.material_count);
                const double error = proportion -
                    batch.human_proportions[static_cast<std::size_t>(class_id)];
                const double absolute_error = std::fabs(error);
                const double normalized = absolute_error /
                    tolerances[batch_index][static_cast<std::size_t>(class_id)];
                const double class_weight = config.class_weights.empty() ? 1.0 :
                    config.class_weights[static_cast<std::size_t>(class_id)];
                if (!is_finite(class_weight) || class_weight <= 0.0) {
                    throw std::invalid_argument("class weights must be finite and positive");
                }

                proportions[static_cast<std::size_t>(class_id)] = proportion;
                errors[static_cast<std::size_t>(class_id)] = error;
                evaluation.batch_max_absolute_error[batch_index] = std::max(
                    evaluation.batch_max_absolute_error[batch_index], absolute_error);
                evaluation.worst_normalized_error = std::max(
                    evaluation.worst_normalized_error, normalized);
                const double weight = batch_weights[batch_index] * class_weight;
                weighted_sum += weight * normalized;
                total_weight += weight;
            }
        }
        evaluation.weighted_mean_normalized_error = total_weight > 0.0
            ? weighted_sum / total_weight : 0.0;
        evaluation.stability_penalty = stability_penalty(sensitivities);
        evaluation.secondary_score =
            config.mean_error_weight * evaluation.weighted_mean_normalized_error +
            evaluation.stability_penalty;

        const auto inserted = cache.emplace(state, std::move(evaluation));
        return inserted.first->second;
    }

    double stability_penalty(const SensitivityTable& sensitivities) const {
        double baseline_distance = 0.0;
        double dimension_count = 0.0;
        for (std::size_t dimension : variable_dimensions) {
            const int camera = camera_of(dimension);
            const int sensitivity_index = sensitivity_of(dimension);
            const SensitivityBounds limit = bounds_for(camera, sensitivity_index);
            const double range = std::max(
                kEpsilon, static_cast<double>(limit.upper - limit.lower));
            baseline_distance += std::fabs(
                static_cast<double>(sensitivities[static_cast<std::size_t>(camera)]
                                                 [static_cast<std::size_t>(sensitivity_index)] -
                                    baseline[static_cast<std::size_t>(camera)]
                                            [static_cast<std::size_t>(sensitivity_index)])) / range;
            dimension_count += 1.0;
        }
        baseline_distance = dimension_count > 0.0
            ? baseline_distance / dimension_count : 0.0;

        double spread = 0.0;
        int spread_slot_count = 0;
        for (int sensitivity_index = 0;
             sensitivity_index < sensitivity_count;
             ++sensitivity_index) {
            if (!calibrate_sensitivity[
                    static_cast<std::size_t>(sensitivity_index)]) {
                continue;
            }
            int lo = 101;
            int hi = -1;
            for (int camera = 0; camera < config.camera_count; ++camera) {
                const int value = sensitivities[static_cast<std::size_t>(camera)]
                                               [static_cast<std::size_t>(sensitivity_index)];
                lo = std::min(lo, value);
                hi = std::max(hi, value);
            }
            spread += static_cast<double>(hi - lo) / 100.0;
            ++spread_slot_count;
        }
        if (spread_slot_count > 0) {
            spread /= static_cast<double>(spread_slot_count);
        }
        return config.baseline_distance_weight * baseline_distance +
               config.camera_spread_weight * spread;
    }

    bool is_feasible(const Evaluation& evaluation) const {
        return evaluation.worst_normalized_error <= 1.0 + kEpsilon;
    }

    void coordinate_descent(SearchState& state,
                            int max_sweeps,
                            std::size_t candidate_limit) {
        for (int sweep = 0; sweep < max_sweeps; ++sweep) {
            bool improved = false;
            for (std::size_t dimension : variable_dimensions) {
                SearchState best_state = state;
                const Evaluation* best_evaluation = &evaluate(best_state);
                const std::vector<std::size_t> options = scan_indices(
                    candidates[dimension].size(), candidate_limit,
                    state.candidate_index[dimension]);
                for (std::size_t option : options) {
                    if (option == state.candidate_index[dimension]) {
                        continue;
                    }
                    SearchState trial = state;
                    trial.candidate_index[dimension] = option;
                    const Evaluation& trial_evaluation = evaluate(trial);
                    if (better(trial_evaluation, *best_evaluation)) {
                        best_state = std::move(trial);
                        best_evaluation = &evaluate(best_state);
                    }
                }
                if (best_state.candidate_index[dimension] != state.candidate_index[dimension]) {
                    state = std::move(best_state);
                    improved = true;
                }
            }
            if (!improved) {
                return;
            }
        }
    }

    SearchState solve() {
        SearchState best_state = baseline_state();
        const Evaluation* best_evaluation = &evaluate(best_state);
        std::mt19937 random(config.random_seed);
        const int start_count = std::max(1, config.multi_start_count);

        for (int start = 0; start < start_count; ++start) {
            SearchState state;
            if (start == 0) {
                state = baseline_state();
            } else if (start == 1) {
                state = constant_state(20);
            } else if (start == 2) {
                state = constant_state(50);
            } else if (start == 3) {
                state = constant_state(80);
            } else {
                state = baseline_state();
                for (std::size_t dimension : variable_dimensions) {
                    std::uniform_int_distribution<std::size_t> distribution(
                        0U, candidates[dimension].size() - 1U);
                    state.candidate_index[dimension] = distribution(random);
                }
            }

            coordinate_descent(state, std::max(0, config.coarse_sweeps),
                               config.coarse_candidate_limit);
            coordinate_descent(state, std::max(0, config.exact_sweeps), 0U);
            const Evaluation& evaluation = evaluate(state);
            if (better(evaluation, *best_evaluation)) {
                best_state = std::move(state);
                best_evaluation = &evaluate(best_state);
            }
        }
        return best_state;
    }

    SensitivityTable deployment_sensitivities(const SearchState& state) const {
        // 界面灵敏度本身是整数离散量，搜索值可直接部署，无需再取区间中点。
        return sensitivities_for(state);
    }

    std::vector<FeasibleRange> conditional_ranges(const SearchState& solution,
                                                   std::size_t dimension) {
        const std::vector<int>& values = candidates[dimension];
        std::vector<bool> pass(values.size(), false);
        for (std::size_t index = 0; index < values.size(); ++index) {
            SearchState trial = solution;
            trial.candidate_index[dimension] = index;
            pass[index] = is_feasible(evaluate(trial));
        }

        std::vector<FeasibleRange> ranges;
        std::size_t index = 0;
        while (index < pass.size()) {
            if (!pass[index]) {
                ++index;
                continue;
            }
            const std::size_t first = index;
            while (index + 1U < pass.size() && pass[index + 1U]) {
                ++index;
            }
            const std::size_t last = index;
            FeasibleRange range;
            range.lower = values[first];
            range.upper = values[last];
            ranges.push_back(range);
            ++index;
        }
        return ranges;
    }

    CalibrationResult make_result(const SearchState& solution) {
        const Evaluation& evaluation = evaluate(solution);
        CalibrationResult result;
        result.feasible = is_feasible(evaluation);
        result.worst_normalized_error = evaluation.worst_normalized_error;
        result.weighted_mean_normalized_error = evaluation.weighted_mean_normalized_error;
        result.search_sensitivities = sensitivities_for(solution);
        result.deployment_sensitivities = deployment_sensitivities(solution);
        result.warnings = warnings;
        if (!result.feasible) {
            result.warnings.push_back(
                "No common sensitivity table satisfies every batch/class tolerance; "
                "review the per-batch errors before deploying a compromise.");
        }

        for (std::size_t batch_index = 0; batch_index < batches.size(); ++batch_index) {
            BatchReport report;
            report.batch_id = batches[batch_index].batch_id;
            report.model_proportions = evaluation.model_proportions[batch_index];
            report.human_proportions = batches[batch_index].human_proportions;
            report.signed_error = evaluation.signed_errors[batch_index];
            report.max_absolute_error = evaluation.batch_max_absolute_error[batch_index];
            report.passed = true;
            for (int class_id = 0; class_id < config.class_count; ++class_id) {
                const double normalized = std::fabs(report.signed_error[static_cast<std::size_t>(class_id)]) /
                    tolerances[batch_index][static_cast<std::size_t>(class_id)];
                if (normalized > 1.0 + kEpsilon) {
                    report.passed = false;
                    break;
                }
            }
            result.batch_reports.push_back(std::move(report));

            MergedBatchOutput output;
            output.batch_id = batches[batch_index].batch_id;
            output.sensitivities = result.deployment_sensitivities;
            output.merged_batch = pipeline.run(
                *batches[batch_index].raw, result.deployment_sensitivities);
            if (!output.merged_batch) {
                throw std::runtime_error("black-box pipeline returned a null deployment output");
            }
            result.deployment_outputs.push_back(std::move(output));
        }

        for (int camera = 0; camera < config.camera_count; ++camera) {
            for (int sensitivity_index = 0;
                 sensitivity_index < sensitivity_count;
                 ++sensitivity_index) {
                const std::size_t dimension = flat(camera, sensitivity_index);
                DimensionReport report;
                report.camera = camera;
                report.sensitivity_index = sensitivity_index;
                report.calibrated = calibrate_dimension[dimension];
                report.candidate_count = candidates[dimension].size();
                report.search_sensitivity =
                    result.search_sensitivities[static_cast<std::size_t>(camera)]
                                               [static_cast<std::size_t>(sensitivity_index)];
                report.deployment_sensitivity =
                    result.deployment_sensitivities[static_cast<std::size_t>(camera)]
                                                   [static_cast<std::size_t>(sensitivity_index)];
                if (report.calibrated) {
                    report.conditional_feasible_ranges = conditional_ranges(solution, dimension);
                }
                result.dimensions.push_back(std::move(report));
            }
        }
        return result;
    }
};

AutoThresholdCalibrator::AutoThresholdCalibrator(
    CalibrationConfig config,
    const IBlackBoxPipeline& pipeline,
    const ICategoryCounter& counter)
    : config_(std::move(config)), pipeline_(pipeline), counter_(counter) {
}

CalibrationResult AutoThresholdCalibrator::calibrate(
    const std::vector<CalibrationBatch>& batches) const {
    Impl impl(config_, pipeline_, counter_, batches);
    const SearchState solution = impl.solve();
    CalibrationResult result = impl.make_result(solution);

    result.per_batch_results.reserve(batches.size());
    for (std::size_t batch_index = 0; batch_index < batches.size(); ++batch_index) {
        CalibrationConfig single_config = config_;
        if (!config_.batch_weights.empty()) {
            single_config.batch_weights = {config_.batch_weights[batch_index]};
        }
        const std::vector<CalibrationBatch> single_batch = {batches[batch_index]};
        Impl single_impl(single_config, pipeline_, counter_, single_batch);
        const SearchState single_solution = single_impl.solve();
        CalibrationResult single_result = single_impl.make_result(single_solution);

        PerBatchCalibrationResult per_batch;
        per_batch.batch_id = batches[batch_index].batch_id;
        per_batch.feasible = single_result.feasible;
        per_batch.worst_normalized_error = single_result.worst_normalized_error;
        per_batch.weighted_mean_normalized_error =
            single_result.weighted_mean_normalized_error;
        per_batch.search_sensitivities =
            std::move(single_result.search_sensitivities);
        per_batch.recommended_sensitivities =
            std::move(single_result.deployment_sensitivities);
        per_batch.report = std::move(single_result.batch_reports.front());
        per_batch.dimensions = std::move(single_result.dimensions);
        per_batch.merged_batch =
            std::move(single_result.deployment_outputs.front().merged_batch);
        per_batch.warnings = std::move(single_result.warnings);
        result.per_batch_results.push_back(std::move(per_batch));
    }
    return result;
}

BatchThresholdEvaluation AutoThresholdCalibrator::evaluate_batch(
    const CalibrationBatch& batch,
    const SensitivityTable& sensitivities) const {
    if (!batch.raw) {
        throw std::invalid_argument("calibration batch needs a raw payload");
    }
    if (batch.material_count == 0U) {
        throw std::invalid_argument("calibration batch needs material_count > 0");
    }
    const int sensitivity_count = config_.sensitivity_count > 0
        ? config_.sensitivity_count : config_.class_count;
    if (static_cast<int>(sensitivities.size()) != config_.camera_count) {
        throw std::invalid_argument(
            "sensitivity camera dimension does not match camera_count");
    }
    for (const auto& row : sensitivities) {
        if (static_cast<int>(row.size()) != sensitivity_count) {
            throw std::invalid_argument(
                "sensitivity slot dimension does not match sensitivity_count");
        }
        for (int value : row) {
            if (value < 0 || value > 100) {
                throw std::invalid_argument(
                    "sensitivity values must be inside [0, 100]");
            }
        }
    }

    BatchThresholdEvaluation evaluation;
    evaluation.batch_id = batch.batch_id;
    evaluation.sensitivities = sensitivities;
    evaluation.merged_batch = pipeline_.run(*batch.raw, sensitivities);
    if (!evaluation.merged_batch) {
        throw std::runtime_error("black-box pipeline returned a null merged batch");
    }
    const MergeCounts counts = counter_.count_categories(
        *evaluation.merged_batch, batch.material_count);
    if (static_cast<int>(counts.category_counts.size()) != config_.class_count) {
        throw std::runtime_error("category counter returned a count vector with the wrong class count");
    }
    evaluation.category_counts = counts.category_counts;
    evaluation.category_proportions.resize(counts.category_counts.size(), 0.0);
    for (std::size_t class_id = 0; class_id < counts.category_counts.size(); ++class_id) {
        const double count = counts.category_counts[class_id];
        if (!is_finite(count) || count < 0.0) {
            throw std::runtime_error("category counter returned an invalid category count");
        }
        evaluation.category_proportions[class_id] =
            count / static_cast<double>(batch.material_count);
    }
    return evaluation;
}

float AutoThresholdCalibrator::sensitivity_to_probability_threshold(
    int sensitivity) {
    if (sensitivity < 0 || sensitivity > 100) {
        throw std::invalid_argument("sensitivity must be inside [0, 100]");
    }
    return static_cast<float>(100 - sensitivity) * 0.01F;
}

double AutoThresholdCalibrator::suggest_absolute_tolerance(
    double human_proportion,
    std::size_t material_count,
    const CalibrationConfig& config) {
    if (!is_finite(human_proportion) || human_proportion < 0.0 ||
        human_proportion > 1.0 || material_count == 0U) {
        throw std::invalid_argument("invalid input for automatic tolerance");
    }
    if (!is_finite(config.confidence_z) || config.confidence_z < 0.0 ||
        !is_finite(config.manual_tolerance_floor) || config.manual_tolerance_floor < 0.0 ||
        !is_finite(config.rare_class_count_floor) || config.rare_class_count_floor < 0.0) {
        throw std::invalid_argument("invalid automatic tolerance configuration");
    }

    const double n = static_cast<double>(material_count);
    const double sampling_error = config.confidence_z *
        std::sqrt(human_proportion * (1.0 - human_proportion) / n);
    const double rare_class_error = config.rare_class_count_floor / n;
    return std::min(1.0, std::max(config.manual_tolerance_floor,
                                  std::max(sampling_error, rare_class_error)));
}

} // 命名空间 auto_threshold
