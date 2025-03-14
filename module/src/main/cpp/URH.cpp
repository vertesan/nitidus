#include "URH.hpp"
#include "main.hpp"

namespace URH {

  UR::Class *GetClass(
    const std::string &assemblyName,
    const std::string &spaceName,
    const std::string &className
  ) {
    const auto urAssembly = UR::Get(assemblyName);
    if (!urAssembly) {
      LOGE("assembly not found: %s", assemblyName.c_str());
      return nullptr;
    }
    const auto urClass = urAssembly->Get(className, spaceName);
    if (!urClass) {
      LOGE("class not found: %s", className.c_str());
      return nullptr;
    }
    return urClass;
  }

  std::shared_ptr<UR::Class> GetNestedClass(
    const std::string &assemblyName,
    const std::string &spaceName,
    const std::string &className,
    const std::string &nestedClassName
  ) {
    auto outerUrClass = GetClass(assemblyName, spaceName, className);
    if (!outerUrClass) {
      LOGE("outer class for %s not found: %s", nestedClassName.c_str(), className.c_str());
      return nullptr;
    }
    return GetNestedClass(outerUrClass->address, nestedClassName);
  }

  UR::Method *GetMethod(
    const std::string &assemblyName,
    const std::string &spaceName,
    const std::string &className,
    const std::string &methodName,
    const std::vector<std::string> &args
  ) {
    auto urClass = GetClass(assemblyName, spaceName, className);
    if (!urClass) return nullptr;
    auto method = urClass->Get<UR::Method>(methodName, args);
    if (!method) {
      LOGE("method not found: %s", methodName.c_str());
      return nullptr;
    }
    return method;
  }

  std::shared_ptr<UR::Class> GetNestedClass(
    void *uClassPtr,
    const std::string &nestedClassName
  ) {
    void *iter = nullptr;
    while (auto *nestedClass = UR::Invoke<void *>("il2cpp_class_get_nested_types", uClassPtr, &iter)) {
      auto nestedName = UR::Invoke<const char *>("il2cpp_class_get_name", nestedClass);
      if (strcmp(nestedName, nestedClassName.c_str()) != 0) {
        continue;
      }

      auto urClass     = std::make_shared<UR::Class>();
      urClass->name    = nestedName;
      urClass->address = nestedClass;
      if (const auto pPClass = UR::Invoke<void *>("il2cpp_class_get_parent", nestedClass)) urClass->parent = UR::Invoke<const char *>("il2cpp_class_get_name", pPClass);
      urClass->namespaze = UR::Invoke<const char *>("il2cpp_class_get_namespace", nestedClass);

      UR::FillClassMember(urClass.get(), nestedClass);
      void *i_class;
      void *iter{};
      do {
        if ((i_class = UR::Invoke<void *>("il2cpp_class_get_interfaces", nestedClass, &iter))) {
          UR::FillClassMember(urClass.get(), i_class);
        }
      } while (i_class);
      return urClass;
    }

    LOGE("nested class not found: %s", nestedClassName.c_str());
    return nullptr;
  }

  void PrintClassFields(const UR::Class *urClass) {
    LOGI("iterating fields of class [%s]", urClass->name.c_str());
    for (auto field : urClass->fields) {
      LOGI("field: %s, offset: %u", field->name.c_str(), field->offset);
    }
  }
}
