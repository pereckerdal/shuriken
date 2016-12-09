#include <catch.hpp>

#include <unistd.h>

#include "file_lock.h"

namespace shk {
namespace {

const char kTestFilename1[] = "filelock-tempfile1";
const char kTestFilename2[] = "filelock-tempfile2";

}  // anonymous namespace

TEST_CASE("FileLock") {
  // In case a crashing test left a stale file behind.
  unlink(kTestFilename1);
  unlink(kTestFilename2);

  SECTION("Lock") {
    FileLock lock(kTestFilename1);
  }

  SECTION("DeleteFileWhenDone") {
    {
      FileLock lock(kTestFilename1);
    }
    CHECK(unlink(kTestFilename1) == -1);
    CHECK(errno == ENOENT);
  }

  SECTION("LockAfterLock") {
    {
      FileLock lock(kTestFilename1);
    }
    {
      FileLock lock(kTestFilename1);
    }
  }

  SECTION("LockWhileLockIsHeld") {
    FileLock lock(kTestFilename1);
    try {
      FileLock lock(kTestFilename1);
      CHECK(false);  // Should not reach this point
    } catch (const IoError &error) {
      // Success
    }
  }

  unlink(kTestFilename1);
  unlink(kTestFilename2);
}

}  // namespace shk
