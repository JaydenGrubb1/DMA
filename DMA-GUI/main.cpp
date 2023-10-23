#include <cstdlib>
#include <Windows.h>

#include "fft.h"
#include "gui.h"
#include "spec.h"
#include "wav.h"

using namespace DMA;

constexpr auto DEFAULT_MAX_FREQ = 6000;

static Audio::WAV wav;
static std::vector<float> freq;

void analyze_audio(void);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	if (!GUI::init(L"Digital Music Analyzer")) {
		return EXIT_FAILURE;
	}

	ImPlot::AddColormap("Spectrum", spec_colormap, 256);

	FFT::init();

	while (GUI::is_active) {
		if (!GUI::begin())
			break;

		ImGui::Begin("Digital Music Analyzer", &GUI::is_active, GUI::WINDOW_FLAGS);

		if (ImGui::Button("Open Audio")) {
			WCHAR file_name[260] = { 0 };
			if (GUI::open_file(file_name, 260, L"WAV Files (*.wav)\0*.wav\0All Files (*.*)\0*.*\0")) {
				wav = Audio::WAV(file_name);
				analyze_audio();
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Open Sheet")) {
			WCHAR file_name[260] = { 0 };
			if (GUI::open_file(file_name, 260, L"XML Files (*.xml)\0*.xml\0All Files (*.*)\0*.*\0")) {
				// TODO: Open file
				MessageBox(nullptr, file_name, L"File Name", MB_OK);
			}
		}

		if (freq.size() > 0) {
			if (ImPlot::BeginPlot("Frequency Spectrum", ImVec2(-1, 0), ImPlotFlags_NoLegend | ImPlotFlags_NoMenus)) {
				ImPlot::PushColormap("Spectrum");
				ImPlot::SetupAxisLimits(ImAxis_Y1, 0, DEFAULT_MAX_FREQ);
				ImPlot::SetupAxisZoomConstraints(ImAxis_Y1, 0, DEFAULT_MAX_FREQ);
				ImPlot::SetupAxisZoomConstraints(ImAxis_X1, 0, wav.num_samples() / wav.sample_rate());
				ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, 0, wav.sample_rate());
				ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, 0, wav.num_samples() / wav.sample_rate());

				ImPlot::PlotHeatmap("spec", freq.data(),
					FFT::WINDOW_SIZE / 2,
					freq.size() / (FFT::WINDOW_SIZE / 2),
					0, 1, nullptr,
					ImPlotPoint(0, wav.sample_rate()),
					ImPlotPoint(wav.num_samples() / wav.sample_rate(), 0),
					ImPlotHeatmapFlags_ColMajor
				);

				ImPlot::PopColormap();
				ImPlot::EndPlot();
			}
		}

		ImGui::End();
		GUI::end();
	}

	GUI::destroy();
	return EXIT_SUCCESS;
}

void analyze_audio(void) {
	std::vector<complex> in(wav.num_samples());
	std::vector<complex> out(wav.num_samples());

	for (size_t i = 0; i < wav.num_samples(); i += wav.sample_size()) {
		in[i] = complex((float)wav.data()[i], 0.0);
	}

	FFT::stft(in, out);
	FFT::format(out, freq);
}