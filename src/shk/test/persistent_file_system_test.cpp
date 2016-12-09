#include <catch.hpp>

#include <fcntl.h>
#include <unistd.h>

#include "persistent_file_system.h"

namespace shk {
namespace {

const char kTestFilename1[] = "filesystem-tempfile1";
const char kTestFilename2[] = "filesystem-tempfile2";

int numOpenFds() {
  const auto num_handles = getdtablesize();
  int count = 0;
  for (int i = 0; i < num_handles; i++) {
    const auto fd_flags = fcntl(i, F_GETFD);
    if (fd_flags != -1) {
      count++;
    }
  }
  return count;
}

}  // anonymous namespace

TEST_CASE("PersistentFileSystem") {
  // In case a crashing test left a stale file behind.
  unlink(kTestFilename1);
  unlink(kTestFilename2);

  const auto fs = persistentFileSystem();

  SECTION("Mmap") {
    SECTION("MissingFile") {
      CHECK_THROWS_AS(fs->mmap("nonexisting.file"), IoError);
    }

    SECTION("FileWithContents") {
      fs->writeFile(kTestFilename1, "data");
      CHECK(fs->mmap(kTestFilename1)->memory().asString() == "data");
    }

    SECTION("EmptyFile") {
      fs->writeFile(kTestFilename1, "");
      CHECK(fs->mmap(kTestFilename1)->memory().asString() == "");
    }
  }

  SECTION("mkstemp") {
    SECTION("don't leak file descriptor") {
      const auto before = numOpenFds();
      const auto path = fs->mkstemp("test.XXXXXXXX");
      fs->unlink(path);
      const auto after = numOpenFds();
      CHECK(before == after);
    }
  }

  unlink(kTestFilename1);
  unlink(kTestFilename2);
}

}  // namespace shk
