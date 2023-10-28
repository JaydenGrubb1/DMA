#include <fstream>
#include <stdexcept>

#include "sheet.h"
#include "tinyxml2/tinyxml2.h"

using namespace tinyxml2;

namespace DMA::Music {

	// TODO: Add wchar* constructor

	Sheet::Sheet(const char* file) {
		XMLDocument doc;
		doc.LoadFile(file);

		if (!doc.RootElement()) {
			throw std::runtime_error("Failed to open file");
		}

		auto score = doc.FirstChildElement("score-partwise");
		auto part = score->FirstChildElement("part");
		auto measure = part->FirstChildElement("measure");

		while (measure) {
			auto note = measure->FirstChildElement("note");
			while (note) {
				auto pitch = note->FirstChildElement("pitch");

				if (pitch) {
					auto step = pitch->FirstChildElement("step");
					auto octave = pitch->FirstChildElement("octave");
					auto alter = pitch->FirstChildElement("alter");
					auto duration = note->FirstChildElement("duration");

					if (!step || !octave || !duration) {
						throw std::runtime_error("Invalid sheet file");
					}

					_notes.emplace_back(
						step->GetText()[0],
						octave->IntText(),
						alter ? alter->IntText() : 0,
						duration->IntText()
					);
				}

				note = note->NextSiblingElement("note");
			}
			measure = measure->NextSiblingElement("measure");
		}
	}
}