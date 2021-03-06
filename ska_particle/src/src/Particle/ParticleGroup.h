#pragma once
#include <vector>
#include "Core/Point.h"
#include "Core/Draw/Color.h"
#include "Graphic/Texture.h"
#include "Logging/Logger.h"

namespace ska {

	struct ParticlePhysicData {
		Point<float> velocity;
		Point<float> acceleration;
	};

	class ParticleGroup {
		friend class ParticleFactory;

	private:
		std::size_t activeIndex;
		const std::size_t groupSize;
		unsigned int particleSize;

	public:
		explicit ParticleGroup(std::size_t gSize) :
			activeIndex(0),
			groupSize(gSize),
			particleSize(1),
			pos(gSize),
			physics(gSize),
			lifetime(gSize),
			color(gSize),
			startColor(gSize),
			endColor(gSize),
			timeSinceLastGeneration(0),
			generationDelay(0) { }

		~ParticleGroup() = default;

		const std::size_t& getLength() const {
			return activeIndex;
		}

		void swapParticles(std::size_t index, std::size_t index2) {
			std::swap(pos[index], pos[index2]);
			std::swap(physics[index], physics[index2]);
			std::swap(lifetime[index], lifetime[index2]);
			std::swap(color[index], color[index2]);
			std::swap(startColor[index], startColor[index2]);
			std::swap(endColor[index], endColor[index2]);
		}

		void kill(std::size_t index) {
			if (activeIndex > 0 && index < activeIndex - 1) {
				swapParticles(index, --activeIndex);
				//std::cout << "Swaping particle " << index << " to " << activeIndex << std::endl;
			} else if (index < activeIndex) {
			    --activeIndex;
			}
		}

		void addIndex(unsigned density) {
			activeIndex += density;
			if (activeIndex >= groupSize) {
				activeIndex = groupSize;
				SLOG(LogLevel::Error) << "Particle group full. Cannot add another particle";
			}
		}

		//SOA for cache locality
		std::vector<Point<float>> pos;
		std::vector<ParticlePhysicData> physics;
		std::vector<int> lifetime;

		std::vector<Color> color;
		std::vector<Color> startColor;
		std::vector<Color> endColor;

		ska::Texture appearance;
		unsigned int timeSinceLastGeneration;
		unsigned int generationDelay;
	};
}
