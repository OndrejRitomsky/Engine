#pragma once

#include "Test.h"
#include "TestGroup.h"
#include "TestConfig.h"

#include "Console/ConsoleColor.h"
#include <string>
#include <vector>

#include "PerfTestGroup.h"

namespace test
{

  class PerfTestGroupCollection {
  public:
    PerfTestGroupCollection(const char* name, const TestConfig& config);

    void AddTestGroup(PerfTestGroup& testGroup);
    void Run();

  private:
    TestConfig _config;
    std::string _name;
    std::vector<PerfTestGroup> _testUnits;
  };

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------

  //---------------------------------------------------------------------------
  PerfTestGroupCollection::PerfTestGroupCollection(const char* name, const TestConfig& config) :
    _name(name),
    _config(config) {
  }

  //---------------------------------------------------------------------------
  void PerfTestGroupCollection::AddTestGroup(PerfTestGroup& testGroup) {
    _testUnits.push_back(testGroup);
  }

  //---------------------------------------------------------------------------
  void PerfTestGroupCollection::Run() {
    console::SetTextColor(console::WHITE_TEXT);

    size_t size = _testUnits.size();
    printf("Running group collection: %s \n \n", _name.data());

    for (auto testUnit : _testUnits) {
      testUnit.SetConfig(_config);
      testUnit.Run();
    }

    printf("Test group collection: %s completed \n", _name.data());
    console::SetTextColor(console::WHITE_TEXT);

    if (_config.stopOnEnd) {
      char c;
      scanf_s("%c", &c, 1u);
    }
  }

}
