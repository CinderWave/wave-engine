Wave Engine

Wave Engine is a handcrafted C++ game engine focused on clarity, precision and long-term maintainability.
Every system is built from the ground up with a modular design, clean architecture and full control over all subsystems.

This repository contains the public build of the engine. It includes the core runtime, essential tools and the foundational systems required for rendering, input, configuration, environment handling and application management.

Wave Engine is designed to be extendable, transparent and approachable for developers who want an engine they can understand from top to bottom.

Features (Current Public Build)
Core Architecture

Fully modular C++20 codebase

Engine-agnostic core library built for maintainability

Clear separation of systems, subsystems and platform layers

Math Module

Vector types (Vec2, Vec3)

Matrix type (Mat4)

Transform utilities (TRS)

Foundation for camera, physics and rendering logic

Input System

Unified input state management for keyboard and mouse

Frame-accurate pressed / released tracking

GLFW input adapter for desktop builds

Configuration

Human-readable config file loader

Automatic parsing of integers, floats, booleans and strings

Filesystem and Environment

Safe file read/write utilities

Engine root discovery

Standardized directory paths for config, logs and resources

Timing & Runtime

High-resolution timing utilities

Frame timing and delta tracking

Runtime bootstrap for core initialization

Logging & Debugging

Formatted logging system

Runtime selectable logging levels

Assertion layer for safe development

Launcher

Minimal engine launcher for testing builds

License key validation placeholder

Integration point for editor and game startup

What This Public Build Is

This is the open, development-ready version of Wave Engine’s core.
It includes everything required to:

Build engine applications

Integrate platform layers

Extend the core systems

Experiment with renderer or editor integration

The public build aligns with the internal version through an automated export pipeline that ensures only clean, engine-agnostic code is included.

What This Public Build Is Not

The following are intentionally not included in the public repository:

Internal tools or pipelines

Any project-specific systems

Proprietary editor extensions

Private debugging or development features

The public version is kept clean and general purpose, suitable for developers exploring engine design or building their own tools on top.

Building

Wave Engine relies on standard CMake and a modern C++20 compiler.

mkdir build
cd build
cmake ..
cmake --build .

Platform requirements:

Windows / Linux supported

Requires a compiler with full C++20 support

GLFW recommended for window + input handling

Roadmap (Public)

Core renderer (Vulkan) introduction

Editor window and UI layer

Scene graph and entity management

Resource system and loaders

Audio subsystem

Cross-platform project templates

The project is evolving actively, and more components will be added as the engine grows.

License

Wave Engine public build is released under the MIT license.

You may use, modify and distribute it freely within the terms of the license.