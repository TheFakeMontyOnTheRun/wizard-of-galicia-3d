//
// Created by monty on 07/02/17.
//
#include "glm/glm.hpp"

#include <memory>
#include <iostream>
#include <functional>
#include <array>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "VBORenderingJob.h"
#include <vector>
#include <utility>
#include <unordered_map>
#include <map>
#include <vector>
#include "NativeBitmap.h"
#include "Material.h"
#include "Trig.h"
#include "TrigBatch.h"
#include "MeshObject.h"
#include "MaterialList.h"
#include "Scene.h"
#include "ETextures.h"
#include "VBORenderingJob.h"
#include "VBORegister.h"
#include "Vec2i.h"
#include "IMapElement.h"
#include "CTeam.h"
#include "CItem.h"
#include "CActor.h"
#include "CGameDelegate.h"
#include "CMap.h"
#include "CTile3DProperties.h"
#include "NoudarDungeonSnapshot.h"
#include "RenderingJobSnapshotAdapter.h"


namespace odb {
    const static glm::mat4 identity = glm::mat4(1.0f);
#ifdef OSMESA
    static int RenderingJobSnapshotAdapter::visibility;
#endif

	glm::mat4 RenderingJobSnapshotAdapter::getSkyTransform(long animationTime) {
#ifndef OSMESA
		long offset = animationTime;
		int integerPart = offset % ((kSkyTextureLength * 2) * 1000);
		float finalOffset = integerPart / 1000.0f;

		return glm::translate(identity, glm::vec3(finalOffset, 0.0f, 0.0f));
#else
        return glm::mat4(1.0f);
#endif
	}


	glm::mat4 RenderingJobSnapshotAdapter::getCubeTransform(glm::vec3 translation, float scale = 1.0f) {
		return glm::scale( glm::translate(identity, translation), glm::vec3( 1.0f, scale, 1.0f ) );
	}

	glm::mat4 RenderingJobSnapshotAdapter::getFloorTransform(glm::vec3 translation) {
		return glm::translate(identity, translation);
	}

	glm::vec2 RenderingJobSnapshotAdapter::easingAnimationCurveStep(glm::vec2 prevPosition,
	                                                                glm::vec2 destPosition,
	                                                                long animationTime,
	                                                                long timestamp) {
        float step = (((float) ((timestamp - animationTime))) /
                      ((float) kAnimationLength));


		float curve = 0.0f;

#ifdef OSMESA
        curve = step;
#else
		if (step < 0.5f) {
			curve = ((2.0f * step) * (2.0f * step)) / 2.0f;
		} else {
			curve = (sqrt((step * 2.0f) - 1.0f) / 2.0f) + 0.5f;
		}
#endif

		return {(curve * (destPosition.x - prevPosition.x)) + prevPosition.x,
		        (curve * (destPosition.y - prevPosition.y)) + prevPosition.y};

	}

	void RenderingJobSnapshotAdapter::readSnapshot(const NoudarDungeonSnapshot &snapshot,
	                                               std::unordered_map<ETextures, std::vector<VBORenderingJob>> &batches,
	                                               const CTilePropertyMap &tilePropertiesRegistry,
	                                               const std::unordered_map<VBORegisterId, VBORegister> &VBORegisters,
	                                               const std::unordered_map<std::string, ETextures> &textureRegistry
	) {

		glm::vec3 pos;
		const auto &floorVBO = VBORegisters.at("floor");

		batches.clear();

#ifdef OSMESA
        auto x0 = std::max( 0, snapshot.mCameraPosition.x - visibility );
        auto x1 = std::min( Knights::kMapSize, snapshot.mCameraPosition.x + visibility );
        auto z0 = std::max( 0, snapshot.mCameraPosition.y - visibility );
        auto z1 = std::min( Knights::kMapSize, snapshot.mCameraPosition.y + visibility );
#else
        const auto &skyVBO = VBORegisters.at("sky");
        batches[ETextures::Skybox].emplace_back(std::get<0>(skyVBO),
                                                std::get<1>(skyVBO),
                                                std::get<2>(skyVBO),
                                                getSkyTransform(snapshot.mTimestamp),
                                                1.0f, true);

        batches[ETextures::Skybox].emplace_back(std::get<0>(skyVBO),
                                                std::get<1>(skyVBO),
                                                std::get<2>(skyVBO),
                                                getSkyTransform(
                                                        snapshot.mTimestamp + kSkyTextureLength * 1000),
                                                1.0f, true);

        auto x0 = 0;
        auto x1 = Knights::kMapSize - 1;
        auto z0 = 0;
        auto z1 = Knights::kMapSize - 1;
#endif

		for (int z = z0; z <= z1; ++z) {
			for (int x = x0; x <= x1; ++x) {

				if (snapshot.mVisibilityMap[z][x] == EVisibility::kInvisible) {
					continue;
				}

				auto tile = snapshot.map[z][x];
#ifndef OSMESA
                Shade shade = ( snapshot.mLightMap[z][x] ) / 255.0f;
#else
                Shade shade = 1.0f;
#endif
				if (x == snapshot.mCursorPosition.x &&
				    z == snapshot.mCursorPosition.y) {
					shade = 1.5f;
				}

				if (tilePropertiesRegistry.count(tile) <= 0) {
					continue;
				}

				auto tileProperties = tilePropertiesRegistry.at(tile);


				if (tileProperties.mCeilingTexture != mNullTexture) {
					pos = glm::vec3(x * 2, -5 + (2.0f * tileProperties.mCeilingHeight), z * 2);
					batches[textureRegistry.at(tileProperties.mCeilingTexture)].emplace_back(std::get<0>(floorVBO),
					                                                                         std::get<1>(floorVBO),
					                                                                         std::get<2>(floorVBO),
					                                                                         getFloorTransform(pos),
					                                                                         shade, true);
				}

				if (tileProperties.mCeilingRepeatedWallTexture != mNullTexture) {

					const auto &tileVBO = VBORegisters.at(tileProperties.mVBOToRender);
                    auto vboId = std::get<0>(tileVBO);
                    auto vboIndicesId = std::get<1>(tileVBO);
                    auto amount = std::get<2>(tileVBO);
                    auto& repeatedBatches = batches[textureRegistry.at(tileProperties.mCeilingRepeatedWallTexture)];

#ifdef OSMESA
                    pos = glm::vec3(x * 2,
                                    -4 + (2.0f * tileProperties.mCeilingHeight) + (tileProperties.mCeilingRepetitions) - 1.0f,
                                    z * 2);

                    repeatedBatches.emplace_back(
                            vboId,
                            vboIndicesId,
                            amount,
                            getCubeTransform(pos, tileProperties.mCeilingRepetitions),
                            shade,
                            tileProperties.mNeedsAlphaTest);
#else
                    for (float y = 0; y < tileProperties.mCeilingRepetitions; ++y) {
                        pos = glm::vec3(x * 2,
                                        -4.0f + (2.0f * tileProperties.mCeilingHeight) + (2.0 * y),
                                        z * 2);

                        repeatedBatches.emplace_back(
                                vboId,
                                vboIndicesId,
                                amount,
                                getCubeTransform(pos),
                                shade,
                                tileProperties.mNeedsAlphaTest);
                    }
#endif
				}

				if (tileProperties.mMainWallTexture != mNullTexture) {
					const auto &tileVBO = VBORegisters.at(tileProperties.mVBOToRender);

					pos = glm::vec3(x * 2, -4.0f, z * 2);

					batches[textureRegistry.at(tileProperties.mMainWallTexture)].emplace_back(
							std::get<0>(tileVBO),
							std::get<1>(tileVBO),
							std::get<2>(tileVBO),
							getCubeTransform(pos), shade,
                            tileProperties.mNeedsAlphaTest);
				}

				if (tileProperties.mFloorRepeatedWallTexture != mNullTexture) {

					const auto &tileVBO = VBORegisters.at(tileProperties.mVBOToRender);
                    auto vboId = std::get<0>(tileVBO);
                    auto vboIndicesId = std::get<1>(tileVBO);
                    auto amount = std::get<2>(tileVBO);
                    auto& repeatedBatches = batches[textureRegistry.at(tileProperties.mFloorRepeatedWallTexture)];
#ifdef OSMESA
                    pos = glm::vec3(x * 2,
                                    -5.0f + (2.0f * tileProperties.mFloorHeight) - (tileProperties.mFloorRepetitions) , z * 2);

                    repeatedBatches.emplace_back(
                            vboId,
                            vboIndicesId,
                            amount,
                            getCubeTransform(pos, tileProperties.mFloorRepetitions),
                            shade,
                            tileProperties.mNeedsAlphaTest);
#else
                    for (float y = 0; y < tileProperties.mFloorRepetitions; ++y) {
                        //the final -1.0f in y is for accounting fore the block's length
                        pos = glm::vec3(x * 2,
                                        -5.0f + (2.0f * tileProperties.mFloorHeight) - (2.0 * y) -
                                        1.0f, z * 2);

                        repeatedBatches.emplace_back(
                                vboId,
                                vboIndicesId,
                                amount, getCubeTransform(pos),
                                shade,
                                tileProperties.mNeedsAlphaTest);
                    }
#endif
				}

				if (tileProperties.mFloorTexture != mNullTexture) {
					pos = glm::vec3(x * 2, -5.0f + (2.0f * tileProperties.mFloorHeight), z * 2);
					batches[textureRegistry.at(tileProperties.mFloorTexture)].emplace_back(std::get<0>(floorVBO),
					                                                                       std::get<1>(floorVBO),
					                                                                       std::get<2>(floorVBO),
					                                                                       getFloorTransform(pos),
					                                                                       shade, true);
				}
			}
		}
	}
}
