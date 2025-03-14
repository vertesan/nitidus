#pragma once

#include "UnityResolve.hpp"

using UR  = UnityResolve;
using URT = UnityResolve::UnityType;

namespace URH {
  UR::Class *GetClass(
    const std::string &assemblyName,
    const std::string &spaceName,  // if absent, use "*"
    const std::string &className
  );
  std::shared_ptr<UR::Class> GetNestedClass(
    const std::string &assemblyName,
    const std::string &spaceName,
    const std::string &className,
    const std::string &nestedClassName
  );
  UR::Method *GetMethod(
    const std::string &assemblyName,
    const std::string &spaceName,
    const std::string &className,
    const std::string &methodName,
    const std::vector<std::string> &args = {}
  );
  std::shared_ptr<UR::Class> GetNestedClass(
    void *uClassPtr,
    const std::string &nestedClassName
  );
  void PrintClassFields(const UR::Class *urClass);
}
