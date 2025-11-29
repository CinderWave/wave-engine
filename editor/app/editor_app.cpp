#include "editor/app/editor_app.hpp"

#include <exception>
#include <iostream>
#include <string>

#include "engine/core/runtime/runtime.hpp"
#include "engine/core/time/time.hpp"
#include "engine/core/time/frame_stats.hpp"
#include "engine/core/input/input_system.hpp"
#include "engine/core/logging/log.hpp"
#include "engine/platform/glfw/glfw_window.hpp"
#include "engine/core/events/event_system.hpp"
#include "engine/core/events/editor_events.hpp"
#include "engine/render/render_system.hpp"

namespace wave::editor::app
{
    using wave::engine::core::runtime::RuntimeConfig;
    using wave::engine::core::runtime::initialize;
    using wave::engine::core::runtime::shutdown;
    using wave::engine::core::logging::LogLevel;
    using wave::engine::core::time::Time;
    using wave::engine::core::time::FrameStats;
    using wave::engine::core::input::InputSystem;
    using wave::engine::platform::glfw::GlfwWindow;
    using wave::engine::core::events::EventSystem;
    using wave::engine::core::events::WindowClosedEvent;
    using wave::engine::core::events::WindowResizedEvent;
    using wave::engine::core::events::Event;
    using wave::engine::render::RenderSystem;

    int EditorApp::run(const EditorAppConfig& config,
                       const fs::path& executable_path)
    {
        try
        {
            RuntimeConfig rt_cfg;
            rt_cfg.app_name        = "WaveEditor";
            rt_cfg.min_log_level   = LogLevel::Info;
            rt_cfg.executable_path = executable_path;

            if (!initialize(rt_cfg))
            {
                std::cerr << "[editor] Failed to initialize engine runtime.\n";
                return 1;
            }

            InputSystem::initialize();

            GlfwWindow window(config.window_width,
                              config.window_height,
                              config.window_title);

            if (!RenderSystem::initialize(static_cast<void*>(window.handle()),
                                          window.width(),
                                          window.height()))
            {
                std::cerr << "[editor] Failed to initialize RenderSystem.\n";
                InputSystem::shutdown();
                shutdown();
                return 1;
            }

            EventSystem::subscribe<WindowClosedEvent>(
                [](const Event&)
                {
                    WAVE_LOG_INFO("[editor] WindowClosedEvent received.");
                });

            EventSystem::subscribe<WindowResizedEvent>(
                [](const Event& base)
                {
                    const auto& e = static_cast<const WindowResizedEvent&>(base);
                    WAVE_LOG_INFO("[editor] WindowResizedEvent: ",
                                  e.width, "x", e.height);

                    RenderSystem::resize(e.width, e.height);
                });

            WAVE_LOG_INFO("[editor] Wave Editor starting up.");

            // New: frame stats instance
            FrameStats frameStats;

            const std::string baseTitle = config.window_title;

            bool running = true;
            while (running && !window.should_close())
            {
                InputSystem::begin_frame();
                window.poll_events();

                Time::update();

                // Update FPS statistics
                float dt = Time::delta_seconds(); // assuming Time exposes this
                frameStats.update(dt);

                // Update window title with FPS
                int fpsInt = static_cast<int>(frameStats.fps());
                if (fpsInt > 0)
                {
                    std::string title = baseTitle + "  [" + std::to_string(fpsInt) + " FPS]";
                    window.set_title(title);
                }

                RenderSystem::begin_frame();

                // TODO: editor update + render calls will go here.

                RenderSystem::end_frame();
            }

            WAVE_LOG_INFO("[editor] Wave Editor shutting down.");

            RenderSystem::shutdown();
            InputSystem::shutdown();
            shutdown();

            return 0;
        }
        catch (const std::exception& e)
        {
            std::cerr << "[editor] Fatal error: " << e.what() << "\n";
            return 1;
        }
        catch (...)
        {
            std::cerr << "[editor] Unknown fatal error.\n";
            return 1;
        }
    }

} // namespace wave::editor::app
