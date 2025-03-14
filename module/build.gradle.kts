import org.gradle.internal.extensions.stdlib.capitalized

plugins {
  alias(libs.plugins.android.library)
}

// choose one arch
// "arm64-v8a"
// "armeabi-v7a"
// "x86_64"
// "x86"
val buildArch = "arm64-v8a"

android {
  namespace = properties["module.id"].toString()
  compileSdk = 35
  ndkVersion = "27.2.12479018"

  buildFeatures {
    prefab = true
  }

  defaultConfig {
    minSdk = 30

    externalNativeBuild {
      cmake {
        arguments += listOf(
          "-DMOD_NAME:STRING=${properties["module.libName"]}",
          "-DMOD_VERSION:STRING=${properties["module.version"]}",
        )
      }
    }

    ndk {
      //noinspection ChromeOsAbiSupport
      abiFilters += listOf(buildArch)
    }
  }

  externalNativeBuild {
    cmake {
      path = file("src/main/cpp/CMakeLists.txt")
      version = "3.31.5"
    }
  }

  packaging {
    jniLibs.pickFirsts += listOf("**/libxdl.so", "**/libshadowhook.so")
  }
}

afterEvaluate {
  android.libraryVariants.forEach { variant ->
    val variantC = variant.name.capitalized()
    val variantL = variant.name.lowercase()
    val targetDir = "${layout.buildDirectory.get()}/outputs/module"

    tasks.register<Sync>("syncModuleFiles${variantC}") {
      group = "Custom"
      description = "synchronize zygisk module files to target directory"
      dependsOn("strip${variantC}DebugSymbols")
      // shadowhook v1.1.1
//      dependsOn("strip${variantC}AndroidTestDebugSymbols")

      into(targetDir)
      from("${rootDir}/module/template/module.prop") {
        expand(
          mapOf(
            "id" to properties["module.id"],
            "name" to properties["module.name"],
            "version" to properties["module.version"],
            "versionCode" to properties["module.versionCode"],
            "author" to properties["module.author"],
            "description" to properties["module.description"],
          )
        )
      }
      from("${rootDir}/module/template") { exclude("module.prop") }
      from("${layout.buildDirectory.get()}/intermediates/stripped_native_libs/${variantL}/strip${variantC}DebugSymbols/out/lib")
      // shadowhook v1.1.1
//      from("${layout.buildDirectory.get()}/intermediates/stripped_native_libs/${variantL}AndroidTest/strip${variantC}AndroidTestDebugSymbols/out/lib") {
//        include("${buildArch}/libshadowhook_nothing.so")
//      }

      doLast {
        file("${targetDir}/zygisk").mkdir()
        file("${targetDir}/system/lib64").mkdirs()

        fileTree("${targetDir}/${buildArch}").visit {
          if (this.isDirectory) return@visit
          if (this.name == "lib${properties["module.libName"]}.so") {
            this.copyTo(file("${targetDir}/zygisk/${buildArch}.so"))
          } else {
            this.copyTo(file("${targetDir}/system/lib64/${this.name}"))
          }
        }
        file("${targetDir}/${buildArch}").deleteRecursively()
      }
    }

    tasks.register<Zip>("createModuleZip${variantC}") {
      group = "Custom"
      description = "create zip file"
      dependsOn("syncModuleFiles${variantC}")
      from(targetDir)
      archiveFileName.set(properties["module.id"].toString().split(".").last() + ".zip")
      destinationDirectory.set(file("${layout.buildDirectory.get()}/outputs"))

      doLast {
        file(targetDir).deleteRecursively()
      }
    }

    tasks.register("postAssemble${variantC}") {
      group = "Custom"
      description = "create zygisk module zip"
      dependsOn("createModuleZip${variantC}")
    }

    variant.assembleProvider.get().finalizedBy("postAssemble${variantC}")
  }
}

dependencies {
  implementation(libs.xdl)
  implementation(libs.aih)
}
