#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <cstdint>

namespace wave::engine::tools {

namespace fs = std::filesystem;

// Supported shader stages for compilation.
enum class ShaderStage {
    Vertex,
    Fragment,
    Compute,
    Geometry,
    TessControl,
    TessEvaluation
};

struct ShaderCompileResult {
    bool        success{false};
    std::string commandLine;   // full command that was executed
    std::string errorLog;      // stderr output or synthesized message
    fs::path    inputPath;
    fs::path    outputPath;
};

// Thin wrapper around an external GLSL → SPIR-V compiler (e.g. glslc).
// This does not depend on Vulkan headers and does not load SPIR-V itself;
// it only produces .spv binaries on disk.
class ShaderCompiler final {
public:
    ShaderCompiler() = default;
    ~ShaderCompiler() = default;

    ShaderCompiler(const ShaderCompiler&) = delete;
    ShaderCompiler& operator=(const ShaderCompiler&) = delete;

    ShaderCompiler(ShaderCompiler&&) noexcept = default;
    ShaderCompiler& operator=(ShaderCompiler&&) noexcept = default;

    // Set path to the GLSL compiler executable (e.g. "glslc" or "C:/VulkanSDK/.../glslc.exe").
    // If not set explicitly, the compiler name defaults to "glslc" and relies on PATH.
    void set_compiler_path(fs::path path);

    const fs::path& compiler_path() const { return m_compilerPath; }

    // Add a global include directory for all compilations.
    // (Translated to "-I<dir>" in the command line.)
    void add_include_dir(fs::path dir);

    const std::vector<fs::path>& include_dirs() const { return m_includeDirs; }

    // Add a global macro definition (e.g. "DEBUG=1").
    // (Translated to "-D<define>" in the command line.)
    void add_define(std::string define);

    const std::vector<std::string>& defines() const { return m_defines; }

    // Compile a GLSL file to SPIR-V output file.
    //   - input: path to .vert/.frag/.comp/etc
    //   - output: desired .spv path
    //   - stage: shader stage enum
    //   - additionalArgs: extra raw command line flags if needed
    //
    // Returns a ShaderCompileResult with success flag and log.
    ShaderCompileResult compile_to_file(
        const fs::path& input,
        const fs::path& output,
        ShaderStage stage,
        const std::vector<std::string>& additionalArgs = {}
    ) const;

private:
    std::string build_stage_flag(ShaderStage stage) const;
    std::string escape_path(const fs::path& p) const;

private:
    fs::path                m_compilerPath{"glslc"};
    std::vector<fs::path>   m_includeDirs;
    std::vector<std::string> m_defines;
};

} // namespace wave::engine::tools
