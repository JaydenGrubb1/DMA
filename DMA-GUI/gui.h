#pragma once

#include "ImGui/imgui.h"

namespace DMA::GUI {
	/// <summary>
	/// ImGui flags for the main window
	/// </summary>
	constexpr auto WINDOW_FLAGS =
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus;

	/// <summary>
	/// The GUI active state
	/// </summary>
	static bool is_active = true;

	/// <summary>
	/// Initializes the GUI
	/// </summary>
	/// <returns>True if the GUI was initialized successfully, false otherwise</returns>
	bool init(void);

	/// <summary>
	/// Starts the GUI render loop
	/// </summary>
	void render_start(void);

	/// <summary>
	/// Ends the GUI render loop
	/// </summary>
	void render_end(void);

	/// <summary>
	/// Cleans up the GUI
	/// </summary>
	void cleanup(void);
}