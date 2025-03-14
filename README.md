# nitidus
An Android il2cpp hooking module, driven by Zygisk.

## Prerequisites
- Android Studio 2024.2.2 or later
- A rooted arm64-v8a or armeabi-v7a Android device
- Your rooting solution has Zygisk support

## Usage
1. (optional) Open `gradle.properties`, change module properties if you want to give it another name or description or etc.
2. Go to `module/src/main/cpp/includes/main.hpp`, replace `NTD_TARGET_APP` definition with your target app's id. Optionally, you can change module's logcat tag by replacing `NTD_MODULE_NAME` with your own tag name.
3. (optional) Go to `module/build.gradle.kts`, select a `buildArch` for your module.
4. Write your own code (perhaps mainly in `module/src/main/cpp/il2cppHook.cpp`).
5. Build the project. If you are not using an Android Studio GUI run the following command in your terminal.

```bash
./gradlew :app:assembleDebug
# or
./gradlew :app:assembleRelease
```

Output module file is located at `module/build/outputs/${MODULE_NAME}.zip`.

## Credits

- xDL: https://github.com/hexhacking/xDL
- ShadowHook: https://github.com/bytedance/android-inline-hook
- UnityResolve.hpp: https://github.com/issuimo/UnityResolve.hpp
