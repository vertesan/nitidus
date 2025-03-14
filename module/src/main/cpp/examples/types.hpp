#pragma once

#include "URH.hpp"

namespace types {
  // a general il2cpp class struct
  struct ManifestExample : URT::Object {
    URT::String *SimpleResver;
    uint64_t Checksum;
    uint64_t Seed;
    uint64_t Size;
    uint8_t IsQualified; // boolean takes only 1 byte
    uint8_t align1[7];   // align 7 bytes
    URT::String *Signature;
  };

  // a C# struct doesn't inherit il2cppObject
  struct EntryExample {
    int32_t Priority;
    int32_t ResourceType;
    int32_t NumDeps;
    int32_t NumContents;
    int32_t NumCategories;
    uint8_t align1[4];
    uint64_t Size;
    URT::Array<URT::Byte> *ContentTypeCrcs;
    uint8_t space1[24];
    URT::Array<URT::Byte> *ContentNameCrcs;
    uint8_t space2[40];
  };
}
