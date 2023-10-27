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

					char step_val = step->GetText()[0];
					int octave_val = octave->IntText();
					int alter_val = alter ? alter->IntText() : 0;
					int duration_val = duration->IntText();

					_notes.emplace_back(step_val, octave_val, alter_val, duration_val);
				}

				note = note->NextSiblingElement("note");
			}
			measure = measure->NextSiblingElement("measure");
		}
	}
}