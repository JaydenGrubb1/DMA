#include <cstdlib>
#include <Windows.h>

#include "gui.h"

using namespace DMA;

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	if (!GUI::init(L"Digital Music Analyzer")) {
		return EXIT_FAILURE;
	}

	while (GUI::is_active) {
		if (!GUI::begin())
			break;

		ImGui::Begin("Digital Music Analyzer", &GUI::is_active, GUI::WINDOW_FLAGS);

		ImGui::Button("Hello, world!");

		ImGui::End();
		GUI::end();
	}

	GUI::destroy();
	return EXIT_SUCCESS;
}