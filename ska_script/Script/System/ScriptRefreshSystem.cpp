#include <limits>
#include "ScriptRefreshSystem.h"
#include "Utils/FileUtils.h"
#include "Core/CodeDebug/CodeDebug.h"
#include "ECS/EntityManager.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "ECS/Basics/Script/ScriptTriggerType.h"
#include "ECS/Basics/Script/ScriptPositionedGetter.h"
#include "Data/BlockContainer.h"
#include "Data/Events/InputKeyEvent.h"
#include "Core/CodeDebug/CodeDebug.h"

ska::ScriptRefreshSystem::ScriptRefreshSystem(EntityManager& entityManager, GameEventDispatcher& ged, ScriptAutoSystem& scriptAutoSystem, ScriptPositionedGetter& spg, BlockContainer& bc) :
	ScriptRefreshSystemBase(entityManager),
	ScriptPositionSystemAccess(entityManager),
	ska::Observer<InputKeyEvent>(std::bind(&ScriptRefreshSystem::onKeyEvent, this, std::placeholders::_1)),
	m_eventDispatcher(ged),
	m_scriptPositionedGetter(spg), 
	m_blockContainer(bc), 
	m_scriptAutoSystem(scriptAutoSystem),
	m_action(false) {
	m_eventDispatcher.ska::Observable<InputKeyEvent>::addObserver(*this);
}


bool ska::ScriptRefreshSystem::onKeyEvent(InputKeyEvent& ike) {
	m_action = ike.icm.getActions()[DoAction];
	return true;
}

void ska::ScriptRefreshSystem::refresh(unsigned int ellapsedTime) {
	auto& components = ScriptRefreshSystemBase::m_componentAccessor;
	auto& componentsPossible = ScriptRefreshSystemBase::m_componentPossibleAccessor;
	auto& componentsSP = ScriptPositionSystemAccess::m_componentAccessor;
	std::vector<EntityId> toDelete;

	const auto& processed = ScriptRefreshSystemBase::getEntities();
	for (const auto& entityId : processed) {
		auto& sac = components.get<ScriptAwareComponent>(entityId);
		const auto& pc = components.get<PositionComponent>(entityId);
		const auto& hc = components.get<HitboxComponent>(entityId);
		const auto& dac = components.get<AnimationComponent>(entityId);
		
		const Point<int>& frontPos = PositionComponent::getFrontPosition(pc, hc, dac);
		const Point<int>& centerPos = PositionComponent::getCenterPosition(pc, hc);

		const auto& subProcessed = ScriptPositionSystemAccess::getEntities();
		EntityId scriptEntity;
		for (auto targets : subProcessed) {
			auto& scriptData = componentsSP.get<ScriptSleepComponent>(targets);

			switch (scriptData.triggeringType) {
			case EnumScriptTriggerType::AUTO:
				startScript(targets, entityId);
				toDelete.push_back(targets);
				break;

			case EnumScriptTriggerType::ACTION:
				if (!m_action) {
					break;
				}

			case EnumScriptTriggerType::MOVE_IN:
				scriptEntity = findNearScriptComponentEntity(pc, targets);
				if (scriptEntity != std::numeric_limits<unsigned int>().max()) {
					startScript(scriptEntity, entityId);
				}
				break;
			default:
				break;
			}

		}

		//TODO D�clencher un �v�nement
		/* TileWorld based events */
		
		std::vector<ScriptTriggerType> reasons;
		const auto blockSize = m_blockContainer.getBlockSize();
		const auto oldCenterPos = Point<int>(sac.lastBlockPos);

		auto worldScripts = m_scriptPositionedGetter.chipsetScript(oldCenterPos, centerPos, centerPos, EnumScriptTriggerType::AUTO, static_cast<const unsigned int>(-1));
		if (m_action) {
			//clog << "Enter Pressed" << std::endl;
			auto tmp = m_scriptPositionedGetter.chipsetScript(oldCenterPos, frontPos, frontPos, EnumScriptTriggerType::ACTION, 0);
			worldScripts.insert(worldScripts.end(), tmp.begin(), tmp.end());

			auto tmp2 = m_scriptPositionedGetter.chipsetScript(oldCenterPos, frontPos, frontPos, EnumScriptTriggerType::ACTION, 1);
			worldScripts.insert(worldScripts.end(), tmp2.begin(), tmp2.end());
		}

		//TODO ajouter v�rification de changement de bloc ? pour �viter de d�tecter tous les lancements d'�v�nements de collisions
		const auto& wccPtr = componentsPossible.get<WorldCollisionComponent>(entityId);
		if (wccPtr != nullptr) {
			const auto& wcc = *wccPtr;
			//std::clog << "Block collision" << std::endl;
			if ((wcc.blockColPosX != wcc.lastBlockColPosX && wcc.blockColPosX != wcc.lastBlockColPosY) ||
				(wcc.blockColPosY != wcc.lastBlockColPosY && wcc.blockColPosY != wcc.lastBlockColPosX)) {
				
				auto tmp = m_scriptPositionedGetter.chipsetScript(oldCenterPos, frontPos, frontPos, EnumScriptTriggerType::TOUCH, 0);
				SKA_DBG_ONLY(
					if (!tmp.empty()) {
						SKA_LOG_DEBUG("Chipset script TOUCH layer bot");
					}
				);
				worldScripts.insert(worldScripts.end(), tmp.begin(), tmp.end());

				auto tmp2 = m_scriptPositionedGetter.chipsetScript(oldCenterPos, frontPos, frontPos, EnumScriptTriggerType::TOUCH, 1);
				SKA_DBG_ONLY(
					if (!tmp2.empty()) {
						SKA_LOG_DEBUG("Chipset script TOUCH layer mid");
					}
				);
				worldScripts.insert(worldScripts.end(), tmp2.begin(), tmp2.end());
			}
		}

		/* If we are moving to another block, triggers a MOVE_OUT event on previous block and MOVE_IN on the next one */
		const auto sameBlockBot = m_blockContainer.isSameBlockId(centerPos, oldCenterPos, 0);
		const auto sameBlockMid = m_blockContainer.isSameBlockId(centerPos, oldCenterPos, 1);
		if (!sameBlockBot || !sameBlockMid) {
#ifdef SKA_DEBUG_GRAPHIC
			auto& dgc = entityManager.getComponent<DebugGraphicComponent>(entityId);
			dgc.typeMask = DebugGraphicType::WALK;
			entityManager.addComponent<DebugGraphicComponent>(entityId, dgc);
#endif

			auto tmpOut = m_scriptPositionedGetter.chipsetScript(oldCenterPos, centerPos, oldCenterPos, EnumScriptTriggerType::MOVE_OUT, !sameBlockBot ? 0 : 1);
			SKA_DBG_ONLY(
				if (!tmpOut.empty()) {
					SKA_LOG_DEBUG("Chipset script MOVE_OUT");
				}
			);
			worldScripts.insert(worldScripts.end(), tmpOut.begin(), tmpOut.end());

			auto tmpIn = m_scriptPositionedGetter.chipsetScript(oldCenterPos, centerPos, centerPos, EnumScriptTriggerType::MOVE_IN, !sameBlockBot ? 0 : 1);
			SKA_DBG_ONLY(
				if (!tmpIn.empty()) {
					SKA_LOG_DEBUG("Chipset script MOVE_IN");
				}
			);
			worldScripts.insert(worldScripts.end(), tmpIn.begin(), tmpIn.end());
		}

		if (oldCenterPos / blockSize != centerPos / blockSize) {
			sac.lastBlockPos.x = centerPos.x;
			sac.lastBlockPos.y = centerPos.y;
		}

		//TODO r�ception d'un �v�nement
		for (const ScriptSleepComponent* ssc : worldScripts) {
			if (ssc != nullptr) {
				EntityId script = ScriptRefreshSystemBase::createEntity();
				components.add<PositionComponent>(script, std::move(PositionComponent(pc)));
				components.add<ScriptSleepComponent>(script, std::move(ScriptSleepComponent(*ssc)));
				componentsSP.get<ScriptSleepComponent>(script).deleteEntityWhenFinished = true;
				startScript(script, entityId);
				if (ssc->triggeringType == EnumScriptTriggerType::AUTO) {
					toDelete.push_back(script);
				}
			}
		}

	}

	for (EntityId targets : toDelete) {
		components.remove<ScriptSleepComponent>(targets);
	}


	m_scriptAutoSystem.update(ellapsedTime);

}

void ska::ScriptRefreshSystem::update(unsigned int ellapsedTime) {
	ScriptRefreshSystemBase::update(ellapsedTime);
}

void ska::ScriptRefreshSystem::registerNamedScriptedEntity(const std::string& nameEntity, const EntityId entity) {
	m_scriptAutoSystem.registerNamedScriptedEntity(nameEntity, entity);
}

void ska::ScriptRefreshSystem::clearNamedScriptedEntities() {
	m_scriptAutoSystem.clearNamedScriptedEntities();
}

void ska::ScriptRefreshSystem::startScript(const EntityId scriptEntity, const EntityId origin) {
	m_scriptAutoSystem.registerScript(nullptr, scriptEntity, origin);
}

ska::EntityId ska::ScriptRefreshSystem::findNearScriptComponentEntity(const PositionComponent& entityPos, EntityId script) const {
	const unsigned int blockSizeSquared = m_blockContainer.getBlockSize() * m_blockContainer.getBlockSize();
	auto& componentsSP = ScriptPositionSystemAccess::m_componentAccessor;
	
	PositionComponent& scriptPos = componentsSP.get<PositionComponent>(script);

	int varX = (scriptPos.x - entityPos.x);
	int varY = (scriptPos.y - entityPos.y);
	int varZ = (scriptPos.z - entityPos.z);
	unsigned int distanceSquared = varX * varX + varY * varY + varZ * varZ;
	if (distanceSquared < blockSizeSquared) {
		return script;
	}

	return std::numeric_limits<unsigned int>().max();
}

ska::ScriptRefreshSystem::~ScriptRefreshSystem() {
	m_eventDispatcher.ska::Observable<InputKeyEvent>::removeObserver(*this);
}
