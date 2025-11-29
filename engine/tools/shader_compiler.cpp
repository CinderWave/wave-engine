#include "shader_compiler.hpp"

#include <cstdlib>     // std::system
#include <sstream>

namespace wave::engine::tools {

// -----------------------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------------------

void ShaderCompiler::set_compiler_path(fs::path path) {
    m_compilerPath = std::move(path);
}

void ShaderCompiler::add_include_dir(fs::path dir) {
    m_includeDirs.push_back(std::move(dir));
}

void ShaderCompiler::add_define(std::string define) {
    m_defines.push_back(std::move(define));
}

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

std::string ShaderCompiler::build_stage_flag(ShaderStage stage) const {
    // glslc-style stage flags
    switch (stage) {
    case ShaderStage::Vertex:         return std::string{"-fshader-stage=vert"};
    case ShaderStage::Fragment:       return std::string{"-fshader-stage=frag"};
    case ShaderStage::Compute:        return std::string{"-fshader-stage=comp"};
    case ShaderStage::Geometry:       return std::string{"-fshader-stage=geom"};
    case ShaderStage::TessControl:    return std::string{"-fshader-stage=tesc"};
    case ShaderStage::TessEvaluation: return std::string{"-fshader-stage=tese"};
    }

    return {};
}

std::string ShaderCompiler::escape_path(const fs::path& p) const {
    const std::string s = p.string();

#if defined(_WIN32)
    // On Windows, wrap in quotes if spaces
    if (s.find(' ') != std::string::npos) {
        return "\"" + s + "\"";
    }
    return s;
#else
    // On POSIX, also wrap in quotes for simplicity
    if (s.find(' ') != std::string::npos) {
        return "\"" + s + "\"";
    }
    return s;
#endif
}

// -----------------------------------------------------------------------------
// Compilation
// -----------------------------------------------------------------------------

ShaderCompileResult ShaderCompiler::compile_to_file(
    const fs::path& input,
    const fs::path& output,
    ShaderStage stage,
    const std::vector<std::string>& additionalArgs
) const {
    ShaderCompileResult result{};
    result.inputPath  = input;
    result.outputPath = output;

    std::ostringstream cmd;

    // Compiler
    cmd << escape_path(m_compilerPath) << ' ';

    // Stage flag
    const std::string stageFlag = build_stage_flag(stage);
    if (!stageFlag.empty()) {
        cmd << stageFlag << ' ';
    }

    // Includes
    for (const auto& inc : m_includeDirs) {
        cmd << "-I" << escape_path(inc) << ' ';
    }

    // Defines
    for (const auto& def : m_defines) {
        cmd << "-D" << def << ' ';
    }

    // Additional args
    for (const auto& arg : additionalArgs) {
        cmd << arg << ' ';
    }

    // Input / output
    cmd << "-o " << escape_path(output) << ' '
        << escape_path(input);

    const std::string commandLine = cmd.str();
    result.commandLine = commandLine;

    // For now, we use std::system and cannot robustly capture stderr.
    // We only get the exit code. Future: redirect stderr to a temp file.
    const int exitCode = std::system(commandLine.c_str());

    if (exitCode == 0) {
        result.success  = true;
        result.errorLog = {};
    } else {
        result.success  = false;
        // Minimal message; editor can show this in the console panel.
        result.errorLog = "Shader compilation failed with exit code " + std::to_string(exitCode);
    }

    return result;
}

} // namespace wave::engine::tools
