#pragma once

#include <string>
#include <vector>

#include "hash.h"
#include "optional.h"
#include "manifest/raw_step.h"

namespace shk {

/**
 * Manifest objects contain an std::vector<Step>. A StepIndex is an index into
 * that vector, or into a vector of the same length that refers to the same Step
 * objects.
 */
using StepIndex = size_t;

/**
 * A Step is a dumb data object that represents one build statment in the
 * build manifest.
 *
 * Parsing the build manifest and evaluating the rules results in a list of
 * Step objects. When the Steps object have been created, the manifest and the
 * variable environments etc can be discarded. It is not possible to recreate
 * the manifest from the list of steps; Step objects contain already evaluated
 * commands.
 */
struct Step {
  class Builder {
   public:
    Builder &setHash(Hash &&hash);
    Builder &setDependencies(std::vector<StepIndex> &&dependencies);
    Builder &setOutputDirs(std::vector<std::string> &&output_dirs);
    Builder &setPoolName(std::string &&pool_name);
    Builder &setCommand(std::string &&command);
    Builder &setDescription(std::string &&description);
    Builder &setGenerator(bool &&generator);
    Builder &setDepfile(std::string &&depfile);
    Builder &setRspfile(std::string &&rspfile);
    Builder &setRspfileContent(std::string &&rspfile_content);

    Step build();

   private:
    Hash _hash;
    std::vector<StepIndex> _dependencies;
    std::vector<std::string> _output_dirs;
    std::string _pool_name;
    std::string _command;
    std::string _description;
    bool _generator = false;
    std::string _depfile;
    std::string _rspfile;
    std::string _rspfile_content;
  };

  /**
   * Builder is recommended to use over this constructor.
   */
  Step(
      Hash &&hash,
      std::vector<StepIndex> &&dependencies,
      std::vector<std::string> &&output_dirs,
      std::string &&pool_name,
      std::string &&command,
      std::string &&description,
      bool &&generator,
      std::string &&depfile,
      std::string &&rspfile,
      std::string &&rspfile_content);
  Step();

  Builder toBuilder() const;

  /**
   * A hash of this build step. The hash is used when comparing against old
   * build steps that have been run to see if the build step is clean.
   */
  const Hash hash{};

  /**
   * A list of indices for steps that must be done and clean before this step
   * can be run. These correspond to "order only", "implicit inputs" and
   * "inputs" in a build.ninja manifest.
   */
  const std::vector<StepIndex> dependencies;

  /**
   * A list of directories that Shuriken should ensure are there prior to
   * invoking the command.
   */
  const std::vector<std::string> output_dirs;

  const std::string pool_name;

  /**
   * Command that should be invoked in order to perform this build step.
   *
   * The command string is empty for phony rules.
   */
  const std::string command;

  /**
   * A short description of the command. Used for prettifying output while
   * running builds.
   */
  const std::string description;

  bool phony() const {
    return command.empty();
  }

  /**
   * It set to true, Shuriken will treat this build step as one that rewrites
   * manifest files. They are treated specially in the following ways:
   *
   * * They are not rebuilt if the command line changes
   * * Files are checked for dirtiness via mtime checks rather than file hashes
   * * They are not cleaned
   */
  const bool generator = false;

  /**
   * For compatibility reasons with Ninja, Shuriken keeps track of the path to
   * a potential depfile generated by the build steps. Shuriken does not use
   * this file, it just removes it immediately after the build step has
   * completed.
   */
  const std::string depfile;

  /**
   * If rspfile is not empty, Shuriken will write rspfile_content to the path
   * specified by rspfile before running the build step and then remove the file
   * after the build step has finished running. Useful on Windows, where
   * commands have a rather short maximum length.
   */
  const std::string rspfile;
  const std::string rspfile_content;
};

inline bool isConsolePool(const std::string &pool_name) {
  return pool_name == "console";
}

}  // namespace shk
