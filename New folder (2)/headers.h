#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <cmath>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <array>

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include "data.cpp"

const int GAME_WIDTH = 480;
const int GAME_HEIGHT = 270;

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

const float FPS = 240;

const float WORLD_HEIGHT = 6;
const int WORLD_GRID = 32;
const int WORLD_SIZE = 16;

const int CHUNK_S = 14;
const int CHUNK_H = 32;

const float GRAVITY = .15;

const int SHADOWMAP_RESOLUTION = 1024;

const bool IS_WEB_BUILD = true;

class MyCamera {
    public:
        Camera rayCamera;
        Vector3 dir = { 1, 0, 0 };
        float pitch;
        float yaw;
        float cameraDist = 5;

        Vector3 sensitivity = { 100, 100, 1.2 };

        void step(Vector3 position);
};

float fract(float x);
float random(float x);
Vector2 randomVec(Vector2 pos);
float fade(float num);
float perlin(float gridSize, Vector2 position);

float LengthDirX(double len, double dir);
float LengthDirY(double len, double dir);
Vector2 LDXY(double len, double dir);
Vector3 Movement(double spd, double dir);
Vector3 Movement(double spd, double dir, KeyboardKey key);
Vector3 Movement(double spd, double dir, KeyboardKey key1, KeyboardKey key2);
bool CheckKeys(KeyboardKey key1, KeyboardKey key2);
bool checkVector2Vec(std::vector<Vector2> vecs, Vector2 vec);
bool doPerSecond(int timesPerSecond, int FPS);
void drawChunkPos(Vector3 position);
void drawRayOffset(Ray ray, Vector3 offset, Color color);
void coutRayCol(RayCollision rayCol, int timesPerSecond);
int checkVector3Vec(std::vector<Vector3> vecs, Vector3 vec);

Model initIce(Shader* shader);

Vector4 calcWindowSize();

RenderTexture2D LoadShadowMapRenderTexture(int width, int height);
RenderTexture2D LoadColorRenderTexture(int width, int height);

class IceWorld;
class World;
class Chunk;

struct ColData {
    Vector3 normal;
    Vector3 point;
    float depth;
};

struct TestCol {
    float X;
    float Y;
    float Z;
};

class Player;

bool checkBBoxPoint(Vector3 point, BoundingBox BBox);
Vector3 correctBoxCol(Vector3 point, Vector3 pos, BoundingBox BBox, Player* player);
Vector2 getPosInChunk(Vector3 pos);
Vector2 getPosInChunk(Vector2 pos);
TestCol testOffsetCol(Vector3 pos, Vector3 off, World* world, float dT);
bool AABBcol(Vector3 p1, Model* m1, Vector3 p2, Model* m2);
std::vector<Vector3> getClosePoints(Chunk* chunk, Vector3 pos, float dist);
ColData collision(Vector3 p1, Model* m1, Vector3 p2, Model* m2, Chunk* chunk);

class IceBlock {
    public:
        Vector3 position;
        Vector3 oriention;

        int model;
        Shader* shader;
};

class SnowFlake;
void resetSnowFlake(SnowFlake* flake, Vector2 position, float height, float dist);

class SnowFlake {
    public:
        Vector3 position;
        Vector3 rotation;

        Vector3 rotDir;
        Vector3 dir;

        float phase;

        void step(Mesh* mesh, Material* mat, Vector2 position, float height, float dist);
};

SnowFlake* spawnSnowFlake(Vector2 position, float height, float dist, Vector2 direction);

class PlacementHandler;

class Player {
    public:
        Vector3 position = {-6, 10, -6};
        Vector3 vecDir = { 0, 0, 0 };
        Vector3 velocity = { 0, 0, 0 };
        float spd = 7;
        float dir = 1;
        float jumpHeight = 12;

        int snow = 0;

        Model model;

        MyCamera camera;
        Vector3 WASD();
        void jump();
        float getFloor(World* world, IceWorld* iceWorld);
        void hitHead(World* world);
        void setHeight(World* world, IceWorld* iceWorld, float dT, int delay);
        Vector3 iceCol(IceWorld* iceWorld, Vector3 movement);

        void enablePlacing();
        void step(World* world, IceWorld* iceWorld);

        PlacementHandler* placementHandler = nullptr;
        bool placing = false;
};

class Chunk {
	public:
		bool grid[(CHUNK_S + 1) * (CHUNK_S + 1) * (CHUNK_H)]; // x, z, y
		Model model;
        Vector2 position;
		
        void edit(Vector3 vec, bool a);
		bool access(Vector3 vec) const;
		bool access(float x, float y, float z) const;
		bool access(int x, int y, int z) const;

		void init(Vector2 position, World* world);

		int getTriangleConfig(Vector3 index, World* world, Vector2 position);
		Vector3 getSidePoint(int side, Vector3 pos);
		void addPointsForTriangleConfig(int config, std::vector<Vector3>* triangles, Vector3 pos);
		Model cubeMarch(World* world, Vector2 position);
};

Chunk createChunk(Vector2 position, Shader shader, World* world);

class Grid {
    public:
        std::vector<Chunk> chunks;

        // q2 = -+, q1 = ++
        // q3 = --, q4 = +-

        std::vector<std::vector<int>> q1;
        std::vector<std::vector<int>> q2;
        std::vector<std::vector<int>> q3;
        std::vector<std::vector<int>> q4;

        int getQuad(Vector2 vec);
        void initQuadrant(std::vector<std::vector<int>>& q, int x, int y);
        int getChunkFromQuadrant(const std::vector<std::vector<int>>& q, int x, int y);
        void initToPos(Vector2 vec);
        void add(Chunk chunk);
};

struct ChunkIceList {
    Vector2 chunkPos;
    std::vector<int> indices;
};

class IceWorld {
    public:
        std::vector<Model> iceModels;
        std::vector<IceBlock> iceBlocks;
        std::vector<ChunkIceList> chunkIces;

        void coutIce();
        int checkForChunk(Vector2 chunkPos);
        void draw(Shader* iceShader);
};

void getIcesFromChunk(Vector2 pos, IceWorld* iceWorld, std::vector<Vector3>* list);
void getIcesFromChunk(Vector2 pos, IceWorld* iceWorld, std::vector<BoundingBox>* list);

class World {
    public:
        Player* player;
        float time;
        Shader* shader;

        std::vector<Vector2> queue;
        std::vector<IceBlock> iceBlocks;
        std::vector<Vector4> trees;

        int checkForChunk(Vector2 position);
        bool accessVoxel(Vector3 position);
        bool isChunkQueued(Vector2 pos);
        void drawTrees(Model* t1, Model* t2, Model* t3);
        void draw(int renderDistance);
        void step();
        void clean();

        void setVoxel(Vector3 position, bool a);

        Grid* grid;
};

struct clickData {
    bool hit;
    Vector3 point;
    int chunkIndex;
    int type; // 0 = snow, 1 = ice
};

int nearestCol(std::vector<RayCollision>* cols, Vector3 pos);
BoundingBox getIceBBox(IceBlock ice);
clickData getClickPosition(Player* player, World* world, IceWorld* iceWorld);
void rebuildChunk(Vector2 vector, World* world, Shader shader);
void dig(Player* player, World* world, Shader shader, IceWorld* iceWorld);

class PlacementHandler {
    public:
        Model* highlightModel;

        Vector2 rotation;
        Vector3 position;

        Vector3 vOne;
        Vector3 vTwo;

        int iceType;

        void rotate();
        bool checkValid(Vector3 pos, World* world, IceWorld* iceWorld);
        void draw(World* world, IceWorld* iceWorld);
        void click(IceWorld* iceWorld, Player* player, World* world);
        void step(World* world, IceWorld* iceWorld, Player* player);
};