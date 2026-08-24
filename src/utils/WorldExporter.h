#pragma once

#include "VoxelEngine/Managers/EntityManagerMeta.h"
#include "VoxelEngine/CMP/ChunkCMP.h"
#include "Config/thirdParties/json.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

using json = nlohmann::json;

class WorldExporter
{
public:
    static void ExportWorldToUE5(EntityManagerMeta &em, const std::vector<EntityMeta> &chunks, const std::string &filepath)
    {
        json worldJson;
        worldJson["world_chunks"] = WORLD_CHUNKS;

        json chunksArray = json::array();
        for (auto ent : chunks)
        {
            ChunkCMP *chunkData = em.getComponent<ChunkCMP>(ent.getNextId());
            if (!chunkData)
            {
                continue;
            }

            json chunkJson;
            chunkJson["x"] = chunkData->chunkX;
            chunkJson["z"] = chunkData->chunkZ;

            json rleData = json::array();
            if (!chunkData->voxels.empty())
            {

                auto currentType = chunkData->voxels[0];
                int cont = 1;

                for (int i = 1; i < chunkData->voxels.size(); i++)
                {
                    if (chunkData->voxels[i] == currentType)
                    {
                        cont++;
                    }
                    else
                    {
                        rleData.push_back({static_cast<int>(currentType), cont});
                        currentType = chunkData->voxels[i];
                        cont = 1;
                    }
                }
                rleData.push_back({static_cast<int>(currentType), cont});
            }
            chunkJson["rle_voxels"] = rleData;
            chunksArray.push_back(chunkJson);
        }

        worldJson["chunks"] = chunksArray;

        std::ofstream file(filepath);
        if (file.is_open())
        {
            file << worldJson.dump(4);
            file.close();
        }
    }
};
