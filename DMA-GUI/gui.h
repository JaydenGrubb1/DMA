#pragma once

#include "ImGui/imgui.h"
#include "ImPlot/implot.h"

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
	/// <param name="title">The title of the main window</param>
	/// <returns>True if the GUI was initialized successfully, false otherwise</returns>
	bool init(LPCWSTR title);

	/// <summary>
	/// Starts the GUI render loop
	/// </summary>
	/// <returns>True if the GUI render loop should continue, false otherwise</returns>
	bool begin(void);

	/// <summary>
	/// Ends the GUI render loop
	/// </summary>
	void end(void);

	/// <summary>
	/// Cleans up the GUI
	/// </summary>
	void destroy(void);
}