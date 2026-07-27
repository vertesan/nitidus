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

// Gradle properties
val moduleId = project.findProperty("module.id")?.toString() ?: ""
val moduleName = project.findProperty("module.name")?.toString() ?: ""
val moduleLibName = project.findProperty("module.libName")?.toString() ?: ""
val moduleVersion = project.findProperty("module.version")?.toString() ?: ""
val moduleVersionCode = project.findProperty("module.versionCode")?.toString() ?: ""
val moduleAuthor = project.findProperty("module.author")?.toString() ?: ""
val moduleDescription = project.findProperty("module.description")?.toString() ?: ""

android {
  namespace = moduleId
  compileSdk = 37
  ndkVersion = "27.2.12479018"

  buildFeatures {
    prefab = true
  }

  defaultConfig {
    minSdk = 30

    externalNativeBuild {
      cmake {
        arguments += listOf(
          "-DMOD_NAME:STRING=${moduleLibName}",
          "-DMOD_VERSION:STRING=${moduleVersion}",
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
    jniLibs.pickFirsts += listOf("**/libxdl.so", "**/libshadowhook.so", "**/libshadowhook_nothing.so")
  }
}

androidComponents {
  onVariants { variant ->
    val variantC = variant.name.capitalized()
    val targetDir = "${layout.buildDirectory.get()}/outputs/module"

    val syncTask = tasks.register<Sync>("syncModuleFiles${variantC}") {
      group = "Custom"
      description = "synchronize zygisk module files to target directory"
      dependsOn("strip${variantC}DebugSymbols")
      // shadowhook v1.1.1
      dependsOn("strip${variantC}AndroidTestDebugSymbols")

      into(targetDir)
      from("${rootDir}/module/template/module.prop") {
        expand(
          mapOf(
            "id" to moduleId,
            "name" to moduleName,
            "version" to moduleVersion,
            "versionCode" to moduleVersionCode,
            "author" to moduleAuthor,
            "description" to moduleDescription,
          )
        )
      }
      from("${rootDir}/module/template") { exclude("module.prop") }
      from("${layout.buildDirectory.get()}/intermediates/stripped_native_libs/${variant.name}/strip${variantC}DebugSymbols/out/lib")
      // shadowhook v1.1.1
      from("${layout.buildDirectory.get()}/intermediates/stripped_native_libs/${variant.name}AndroidTest/strip${variantC}AndroidTestDebugSymbols/out/lib") {
        include("${buildArch}/libshadowhook_nothing.so")
      }

      doLast {
        file("${targetDir}/zygisk").mkdir()
        file("${targetDir}/system/lib64").mkdirs()

        fileTree("${targetDir}/${buildArch}").visit {
          if (this.isDirectory) return@visit
          if (this.name == "lib${moduleLibName}.so") {
            this.copyTo(file("${targetDir}/zygisk/${buildArch}.so"))
          } else {
            this.copyTo(file("${targetDir}/system/lib64/${this.name}"))
          }
        }
        file("${targetDir}/${buildArch}").deleteRecursively()
      }
    }

    val zipTask = tasks.register<Zip>("createModuleZip${variantC}") {
      group = "Custom"
      description = "create zip file"
      dependsOn(syncTask)
      from(targetDir)
      archiveFileName.set(moduleId.split(".").last() + ".zip")
      destinationDirectory.set(file("${layout.buildDirectory.get()}/outputs"))

      doLast {
        file(targetDir).deleteRecursively()
      }
    }

    val postAssembleTask = tasks.register("postAssemble${variantC}") {
      group = "Custom"
      description = "create zygisk module zip"
      dependsOn(zipTask)
    }

    tasks.matching { it.name == "assemble${variantC}" }.configureEach {
      finalizedBy(postAssembleTask)
    }
  }
}

dependencies {
  implementation(libs.xdl)
  implementation(libs.aih)
}
