#include "manifest/indexed_manifest.h"

namespace shk {
namespace detail {

template <typename Step>
OutputFileMap computeOutputFileMapGeneric(
    const std::vector<Step> &steps) throw(BuildError) {
  OutputFileMap result;

  for (size_t i = 0; i < steps.size(); i++) {
    const auto &step = steps[i];
    for (const auto &output : step.outputs) {
      const auto ins = result.emplace(output, i);
      if (!ins.second) {
        throw BuildError("Multiple rules generate " + output.original());
      }
    }
  }

  return result;
}

OutputFileMap computeOutputFileMap(
    const std::vector<RawStep> &steps) throw(BuildError) {
  return computeOutputFileMapGeneric(steps);
}

OutputFileMap computeOutputFileMap(
    const std::vector<Step> &steps) throw(BuildError) {
  return computeOutputFileMapGeneric(steps);
}

StepHashes computeStepHashes(const std::vector<RawStep> &steps) {
  StepHashes hashes;
  hashes.reserve(steps.size());

  for (const auto &step : steps) {
    hashes.push_back(step.hash());
  }

  return hashes;
}

}  // namespace detail

namespace {

Step convertRawStep(RawStep &&raw) {
  Step::Builder builder;
  builder.setInputs(std::move(raw.inputs));
  builder.setImplicitInputs(std::move(raw.implicit_inputs));
  builder.setDependencies(std::move(raw.dependencies));
  builder.setOutputs(std::move(raw.outputs));
  builder.setPoolName(std::move(raw.pool_name));
  builder.setCommand(std::move(raw.command));
  builder.setDescription(std::move(raw.description));
  builder.setGenerator(std::move(raw.generator));
  builder.setDepfile(std::move(raw.depfile));
  builder.setRspfile(std::move(raw.rspfile));
  builder.setRspfileContent(std::move(raw.rspfile_content));
  return builder.build();
}

std::vector<Step> convertStepVector(std::vector<RawStep> &&steps) {
  std::vector<Step> ans;
  ans.reserve(steps.size());

  for (auto &step : steps) {
    ans.push_back(convertRawStep(std::move(step)));
  }

  return ans;
}

}  // anonymous namespace

IndexedManifest::IndexedManifest(RawManifest &&manifest)
    : output_file_map(detail::computeOutputFileMap(manifest.steps)),
      step_hashes(detail::computeStepHashes(manifest.steps)),
      steps(convertStepVector(std::move(manifest.steps))),
      defaults(std::move(manifest.defaults)),
      pools(std::move(manifest.pools)),
      build_dir(std::move(manifest.build_dir)) {}

}  // namespace shk
