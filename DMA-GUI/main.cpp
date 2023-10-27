#include <cstdlib>
#include <format>
#include <Windows.h>

#include "fft.h"
#include "gui.h"
#include "onset.h"
#include "spec.h"
#include "wav.h"

using namespace DMA;

constexpr auto DEFAULT_MAX_FREQ = 6000;

static Audio::WAV wav;
static std::vector<float> freq;
static std::vector<float> hfc;
static std::vector<float> start_times;
static std::vector<float> stop_times;
static bool audio_file_loaded = false;

void analyze_audio(void);
ImPlotPoint audio_getter(int idx, void* data);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	if (!GUI::init(L"Digital Music Analyzer")) {
		return EXIT_FAILURE;
	}

	ImPlot::AddColormap("Spectrum", spec_colormap, COLOR_MAP_SIZE);
	ImGuiIO& io = ImGui::GetIO();

	FFT::init();

	bool show_onsets = false;
	bool show_offsets = false;

	int current_time = 0;

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
		if (!audio_file_loaded) {
			ImGui::BeginDisabled();
		}
		if (ImGui::Button("Open Sheet")) {
			WCHAR file_name[260] = { 0 };
			if (GUI::open_file(file_name, 260, L"XML Files (*.xml)\0*.xml\0All Files (*.*)\0*.*\0")) {
				// TODO: Open file
				MessageBox(nullptr, file_name, L"File Name", MB_OK);
			}
		}
		if (!audio_file_loaded) {
			ImGui::EndDisabled();
		}

		std::string fps = std::format("FPS: {:.0f} ({:.1f} ms)", io.Framerate, 1000.0f / io.Framerate);
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(fps.c_str()).x);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3, 0.3, 0.3, 1.0));
		ImGui::Text(fps.c_str());
		ImGui::PopStyleColor();

		if (audio_file_loaded) {
			if (ImGui::BeginTabBar("tabs")) {
				if (ImGui::BeginTabItem("Frequency")) {
					ImGui::Checkbox("Show Onsets", &show_onsets);
					ImGui::SameLine();
					ImGui::Checkbox("Show Offsets", &show_offsets);

					float duration = wav.num_samples() / wav.sample_rate();
					static float ratios[] = { 2.0, 1.0 };

					if (ImPlot::BeginSubplots("plots", 2, 1, ImVec2(-1, ImGui::GetContentRegionAvail().y), ImPlotSubplotFlags_NoTitle | ImPlotSubplotFlags_LinkCols, ratios)) {
						if (ImPlot::BeginPlot("Frequency Spectrum", ImVec2(-1, 0), ImPlotFlags_NoLegend | ImPlotFlags_NoMenus)) {
							ImPlot::PushColormap("Spectrum");
							ImPlot::SetupAxisLimits(ImAxis_Y1, 0, DEFAULT_MAX_FREQ);
							ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, 0, wav.sample_rate() / 2);
							ImPlot::SetupAxisZoomConstraints(ImAxis_Y1, 0, DEFAULT_MAX_FREQ);
							ImPlot::SetupAxisLimits(ImAxis_X1, 0, duration);
							ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, 0, duration);
							ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoTickLabels);

							ImPlot::PlotHeatmap("spec", freq.data(),
								FFT::WINDOW_SIZE / 2,
								freq.size() / (FFT::WINDOW_SIZE / 2),
								0, 1, nullptr,
								ImPlotPoint(0, wav.sample_rate() / 2),
								ImPlotPoint((double)wav.num_samples() / wav.sample_rate(), 0),
								ImPlotHeatmapFlags_ColMajor
							);

							ImPlot::PopColormap();
							ImPlot::EndPlot();
						}

						if (ImPlot::BeginPlot("High-Frequency Content", ImVec2(-1, 0), ImPlotFlags_NoMenus)) {
							ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
							ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, 0, 1);
							ImPlot::SetupAxisLimits(ImAxis_X1, 0, duration);
							ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, 0, duration);
							
							ImPlot::PlotLine("##hfc", hfc.data(), hfc.size(), duration / hfc.size());
							if (show_onsets) {
								ImPlot::PlotInfLines("Onsets", start_times.data(), start_times.size());
							}
							if (show_offsets) {
								ImPlot::PlotInfLines("Offsets", stop_times.data(), stop_times.size());
							}

							ImPlot::EndPlot();
						}
						ImPlot::EndSubplots();
					}
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Octaves")) {
					ImGui::PushButtonRepeat(true);
					if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
						if (current_time > 0)
							current_time--;
					}
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 52 + ImGui::GetStyle().ItemSpacing.x * 2);
					ImGui::SliderInt("##current_time", &current_time, 0, hfc.size() - 1);
					ImGui::SameLine();
					if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
						if (current_time < hfc.size() - 1)
							current_time++;
					}
					ImGui::PopButtonRepeat();

					if (ImPlot::BeginSubplots("plots", 2, 1, ImVec2(-1, ImGui::GetContentRegionAvail().y), ImPlotSubplotFlags_NoTitle)) {
						if (ImPlot::BeginPlot("Time Domain", ImVec2(-1, 0), ImPlotFlags_NoLegend | ImPlotFlags_NoMenus)) {
							ImPlot::SetupAxisLimits(ImAxis_Y1, -0.5, 0.5);
							ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, -0.5, 0.5);
							ImPlot::SetupAxisLimits(ImAxis_X1, 0, 500.0 * FFT::WINDOW_SIZE / wav.sample_rate());
							ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, 0, 500.0 * FFT::WINDOW_SIZE / wav.sample_rate());

							ImPlot::PlotLineG("##time", audio_getter, (void*)&current_time, FFT::WINDOW_SIZE / 2);
							ImPlot::EndPlot();
						}

						if (ImPlot::BeginPlot("Frequency Domain", ImVec2(-1, 0), ImPlotFlags_NoLegend | ImPlotFlags_NoMenus)) {
							ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
							ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, 0, 1);
							ImPlot::SetupAxisLimits(ImAxis_X1, 0, DEFAULT_MAX_FREQ);
							ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, 0, wav.sample_rate() / 2);
							ImPlot::SetupAxisZoomConstraints(ImAxis_X1, 0, DEFAULT_MAX_FREQ);

							ImPlot::PlotLine("##freq", freq.data() + current_time * FFT::WINDOW_SIZE / 2, FFT::WINDOW_SIZE / 2, (float)wav.sample_rate() / FFT::WINDOW_SIZE);
							ImPlot::EndPlot();
						}
						ImPlot::EndSubplots();
					}
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
		}
		else {
			std::string str = "Open an audio file to begin.";
			ImVec2 text_size = ImGui::CalcTextSize(str.c_str());
			ImVec2 available = ImGui::GetContentRegionAvail();
			ImGui::SetCursorPos(ImVec2(available.x / 2 - text_size.x / 2, available.y / 2 - text_size.y / 2));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3, 0.3, 0.3, 1.0));
			ImGui::Text(str.c_str());
			ImGui::PopStyleColor();
		}

		ImGui::End();
		GUI::end();
	}

	GUI::destroy();
	return EXIT_SUCCESS;
}

void analyze_audio(void) {
	audio_file_loaded = false;

	std::vector<complex> in(wav.num_samples());
	std::vector<complex> out(wav.num_samples());

	for (size_t i = 0; i < wav.num_samples(); i++) {
		in[i] = complex(wav[i], 0);
	}

	FFT::stft(in, out);
	FFT::format(out, freq);

	std::vector<int> starts;
	std::vector<int> stops;
	std::vector<float> notes;

	Onset::analyze(freq, hfc);
	Onset::detect(hfc, starts, stops);
	Onset::identify(in, starts, stops, wav.sample_rate(), notes);

	start_times.clear();
	stop_times.clear();
	starts.reserve(starts.size());
	stop_times.reserve(stops.size());

	float duration = wav.num_samples() / wav.sample_rate();
	for (size_t i = 0; i < starts.size(); i++) {
		start_times.push_back(((float)starts[i] / hfc.size()) * duration);
		stop_times.push_back(((float)stops[i] / hfc.size()) * duration);
	}

	audio_file_loaded = true;
}

ImPlotPoint audio_getter(int idx, void* data) {
	int offset = *(int*)data;
	return ImPlotPoint(idx * 1000.0 / wav.sample_rate(), wav[idx + offset * FFT::WINDOW_SIZE / 2]);
}