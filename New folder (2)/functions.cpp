#include "headers.h"

void MyCamera::step(Vector3 position) {
    cameraDist -= (GetMouseWheelMove() / sensitivity.z) * (cameraDist / 7.);
    cameraDist = Clamp(cameraDist, 1, 45);

    if (cameraDist > 3) {
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            Vector2 mouseDelta = GetMouseDelta();
            yaw -= mouseDelta.x / sensitivity.x;
            pitch += mouseDelta.y / sensitivity.y;

            pitch = Clamp(pitch, -PI / 2 - .1, PI / 2 - .1);

            dir = {
                cosf(pitch) * sinf(yaw),
                sinf(pitch),
                cosf(pitch) * cosf(yaw)
            };
        };

        rayCamera.position = Vector3Normalize(dir) * cameraDist + position;
        rayCamera.target = position;
    }
    else {
        Vector2 mouseDelta = GetMouseDelta();
        yaw -= mouseDelta.x / (sensitivity.x + .5);
        pitch += mouseDelta.y / (sensitivity.y + .5);

        pitch = Clamp(pitch, -PI / 2 - .1, PI / 2 - .1);

        dir = {
            cosf(pitch) * sinf(yaw),
            sinf(pitch),
            cosf(pitch) * cosf(yaw)
        };
        rayCamera.position = { position.x, position.y + 2.5f, position.z };
        rayCamera.target = Vector3Normalize(dir) * -5 + Vector3{ position.x, position.y + 2.5f, position.z };

        Vector2 mousePos = GetMousePosition();
        if (mousePos.x >= GetScreenWidth() - 1) SetMousePosition(1, mousePos.y);
        if (mousePos.x <= 0) SetMousePosition(GetScreenWidth() - 2, mousePos.y);
       
    };
};

float fract(float x) {
    return x - floor(x);
};
float random(float x) {
    return fract(sin(x) * 43758.5453f);
};
Vector2 randomVec(Vector2 pos) {
    float seed = pos.x * 12.9898f + pos.y * 78.233f;
    float angle = random(seed) * 2.0f * PI;

    return { cos(angle), sin(angle) };
};
float fade(float num) {
    return num * num * num * (num * (num * 6 - 15) + 10);
};
float perlin(float gridSize, Vector2 position) {
    Vector2 gridPos = { floor(position.x / gridSize), floor(position.y / gridSize) };
    Vector2 flooredPos = { gridPos.x * gridSize, gridPos.y * gridSize };

    Vector2 cornerOne = randomVec(flooredPos);
    Vector2 cornerTwo = randomVec({ flooredPos.x + gridSize, flooredPos.y });
    Vector2 cornerThree = randomVec({ flooredPos.x, flooredPos.y + gridSize });
    Vector2 cornerFour = randomVec({ flooredPos.x + gridSize, flooredPos.y + gridSize });

    Vector2 relPos = { (position.x - flooredPos.x) / gridSize,(position.y - flooredPos.y) / gridSize };

    Vector2 disOne = { relPos.x, relPos.y };
    Vector2 disTwo = { relPos.x - 1.0f, relPos.y };
    Vector2 disThree = { relPos.x, relPos.y - 1.0f };
    Vector2 disFour = { relPos.x - 1.0f, relPos.y - 1.0f };

    float dotOne = Vector2DotProduct(cornerOne, disOne);
    float dotTwo = Vector2DotProduct(cornerTwo, disTwo);
    float dotThree = Vector2DotProduct(cornerThree, disThree);
    float dotFour = Vector2DotProduct(cornerFour, disFour);

    float u = fade(relPos.x);
    float v = fade(relPos.y);

    float lerpOne = Lerp(dotOne, dotTwo, u);
    float lerpTwo = Lerp(dotThree, dotFour, u);
    return Lerp(lerpOne, lerpTwo, v);
};

float LengthDirX(double len, double dir) {
    return len * cos(dir * PI / 180);
};
float LengthDirY(double len, double dir) {
    return len * sin(dir * PI / 180);
};
Vector2 LDXY(double len, double dir) {
    float x = len * cos(dir * PI / 180);
    float y = len * sin(dir * PI / 180);
    return Vector2{ x, y };
};
Vector3 Movement(double spd, double dir) {
    float x = spd * cos(dir * PI / 180);
    float y = spd * sin(dir * PI / 180);
    return { x, 0, y };
};
Vector3 Movement(double spd, double dir, KeyboardKey key) {
    if (IsKeyDown(key)) {
        float x = spd * cos(dir * PI / 180);
        float y = spd * sin(dir * PI / 180);
        return { x, 0, y };
    }
    else {
        return { 0, 0, 0 };
    };
};
Vector3 Movement(double spd, double dir, KeyboardKey key1, KeyboardKey key2) {
    if (IsKeyDown(key1) && IsKeyDown(key2)) {
        float x = spd * cos(dir * PI / 180);
        float y = spd * sin(dir * PI / 180);
        return { x, 0, y };
    }
    else {
        return { 0, 0, 0 };
    };
};
bool CheckKeys(KeyboardKey key1, KeyboardKey key2) {
    if (IsKeyDown(key1) && IsKeyDown(key2)) {
        return true;
    }
    else {
        return false;
    };
};
bool checkVector2Vec(std::vector<Vector2> vecs, Vector2 vec) {
    for (int i = 0; i < vecs.size(); i++) {
        if (vecs.at(i).x == vec.x && vecs.at(i).y == vec.y) return true;
    };
    return false;
};
bool doPerSecond(int timesPerSecond, int FPS) {
    if (fmod(GetTime(), (1. / timesPerSecond)) <= (1. / FPS)) return true;
    return false;
};
void drawChunkPos(Vector3 position) {
    DrawSphere({ (floor(position.x / CHUNK_S) * CHUNK_S) + (CHUNK_S / 2), 10, (floor(position.z / CHUNK_S) * CHUNK_S) + (CHUNK_S / 2)}, 4, RED);
};
void drawRayOffset(Ray ray, Vector3 offset, Color color) {
    Ray* newRay = new Ray;
    newRay->position = ray.position + offset;
    newRay->direction = ray.direction;

    DrawRay(*newRay, color);
    delete newRay;
};
void coutRayCol(RayCollision rayCol, int timesPerSecond) {
    if (doPerSecond(timesPerSecond, FPS)) {
        std::cout << "RayCollision { " <<
            "\n   Distance: " << rayCol.distance <<
            "\n   Hit: " << rayCol.hit <<
            "\n   Normal: x:" << rayCol.normal.x << " y: " << rayCol.normal.y << " z: " << rayCol.normal.z <<
            "\n   Point: " << rayCol.point.x << " y: " << rayCol.point.y << " z: " << rayCol.point.z <<
            "\n} \n";
    };
};
int checkVector3Vec(std::vector<Vector3> vecs, Vector3 vec) {
    for (int i = 0; i < vecs.size(); i++) {
        if (vecs.at(i).x == vec.x && vecs.at(i).y == vec.y && vecs.at(i).z == vec.z) return i;
    };
    return -1;
};

Model initIce(Shader* shader) {
    Mesh mesh = GenMeshCube(1, 1, 2);
    Model model = LoadModelFromMesh(mesh);
    model.materials[0].shader = *shader;

    return model;
};

Vector4 calcWindowSize() {
    Vector2 ratio = { 16, 9 };
    float w = (float)GetRenderWidth() / ratio.x;
    float h = (float)GetRenderHeight() / ratio.y;
    
    Vector4 ret = { w * ratio.x, h * ratio.y, 0, 0 };

    if (w > h) {
        ret.x = w * ratio.x;
        ret.y = w * ratio.y;
    };

    if (w < h) {
        ret.x = h * ratio.x;
        ret.y = h * ratio.y;
    };

    ret.z = (ret.x - (w * ratio.x)) / 2.;
    ret.w = (ret.y - (h * ratio.y)) / 2.;

    return ret;
};

RenderTexture2D LoadShadowMapRenderTexture(int width, int height) {
    RenderTexture2D target = { 0 };

    target.id = rlLoadFramebuffer();
    target.texture.width = width;
    target.texture.height = height;

    if (target.id > 0) {
        rlEnableFramebuffer(target.id);

        target.depth.id = rlLoadTextureDepth(width, height, false);
        target.depth.width = width;
        target.depth.height = height;
        target.depth.format = 19;
        target.depth.mipmaps = 1;

        rlFramebufferAttach(target.id, target.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);
        rlDisableFramebuffer();
    };

    return target;
};
RenderTexture2D LoadColorRenderTexture(int width, int height) {
    RenderTexture2D target = { 0 };

    target.id = rlLoadFramebuffer();

    if (target.id > 0) {
        target.texture.id = rlLoadTexture(NULL, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);

        target.texture.width = width;
        target.texture.height = height;
        target.texture.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        target.texture.mipmaps = 1;

        rlEnableFramebuffer(target.id);
        rlFramebufferAttach(target.id, target.texture.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);

        rlDisableFramebuffer();
    };

    return target;
};

bool checkBBoxPoint(Vector3 point, BoundingBox BBox) {
    if (point.x > BBox.min.x && point.x < BBox.max.x && point.y > BBox.min.y && point.y < BBox.max.y && point.z > BBox.min.z && point.z < BBox.max.z) return true;
    return false;
};
Vector3 correctBoxCol(Vector3 point, Vector3 pos, BoundingBox BBox, Player* player) {
    Vector3 ret = pos + point;
    Vector3 mid = (BBox.min + BBox.max) / 2;

    //DrawCube(mid, BBox.min.x - BBox.max.x, BBox.min.y - BBox.max.y, BBox.min.z - BBox.max.z, GREEN);

    bool isCol = checkBBoxPoint(ret, BBox);

    if (isCol == true) {
        if (pos.z > BBox.min.z && pos.z < BBox.max.z && pos.y > BBox.min.y && pos.y < BBox.max.y) {
            if (ret.x > mid.x && checkBBoxPoint(ret, BBox)) ret.x -= ret.x - BBox.max.x;
            if (ret.x < mid.x && checkBBoxPoint(ret, BBox)) ret.x += BBox.min.x - ret.x;
        };
        if (pos.x > BBox.min.x && pos.x < BBox.max.x && pos.y > BBox.min.y && pos.y < BBox.max.y) {
            if (ret.z > mid.z && checkBBoxPoint(ret, BBox)) ret.z -= ret.z - BBox.max.z;
            if (ret.z < mid.z && checkBBoxPoint(ret, BBox)) ret.z += BBox.min.z - ret.z;
        };
    }

    return ret - pos;
};
Vector2 getPosInChunk(Vector3 pos) {
    return Vector2{ fract(pos.x / CHUNK_S) * CHUNK_S, fract(pos.z / CHUNK_S) * CHUNK_S };
};
Vector2 getPosInChunk(Vector2 pos) {
    return Vector2{ fract(pos.x / CHUNK_S) * CHUNK_S, fract(pos.y / CHUNK_S) * CHUNK_S };
}
TestCol testOffsetCol(Vector3 pos, Vector3 off, World* world, float dT) {
    TestCol testCol;
        testCol.X = 1;
        testCol.Y = 1;
        testCol.Z = 1;

    Vector2 testVec = { floor(pos.x / CHUNK_S), floor(pos.z / CHUNK_S)};
    int chunk = world->checkForChunk(testVec);

    if (chunk != -1) {
        Vector2 posInChunk = getPosInChunk(pos + off);

        float yOffset = .9;
        float distMod = 1.25;

        Ray rayX;
        rayX.position = Vector3{ posInChunk.x, pos.y + yOffset, posInChunk.y };
        rayX.direction = Vector3{ off.x, 0, 0 } / dT / distMod;
        Ray rayY;
        rayY.position = Vector3{ posInChunk.x, pos.y + (yOffset * 2), posInChunk.y};
        rayY.direction = Vector3{ 0, off.y, 0 } / dT / distMod;
        Ray rayZ;
        rayZ.position = Vector3{ posInChunk.x, pos.y + yOffset, posInChunk.y };
        rayZ.direction = Vector3{ 0, 0, off.z } / dT / distMod;

        Matrix m = MatrixIdentity();
        Chunk* chunkA = &world->grid->chunks.at(chunk);

        //DEBUG DRAWING
        /*Vector3 drawOffset = {testVec.x * CHUNK_S, 0, testVec.y * CHUNK_S};
        DrawModel(chunkA->model, drawOffset, 1, RED);
        drawRayOffset(rayX, drawOffset, RED);
        drawRayOffset(rayY, drawOffset, RED);
        drawRayOffset(rayZ, drawOffset, RED);

        DrawSphere(Vector3{pos.x, pos.y + yOffset, pos.z} + (rayX.direction * 1), .25, RED);
        DrawSphere(Vector3{ pos.x, pos.y + yOffset + yOffset, pos.z } + (rayY.direction * 1), .25, GREEN);
        DrawSphere(Vector3{ pos.x, pos.y + yOffset, pos.z } + (rayZ.direction * 1), .25, BLUE);*/

        RayCollision rayColX = GetRayCollisionMesh(rayX, chunkA->model.meshes[0], m);
        if (rayColX.hit == true) {
            //DrawSphere(rayColX.point + drawOffset, .25, RED);
            if (rayColX.distance <= abs(rayX.direction.x)) {
                testCol.X = -(abs(rayX.direction.x) - rayColX.distance) / 2.5;
            };
        };

        RayCollision rayColY = GetRayCollisionMesh(rayY, chunkA->model.meshes[0], m);
        if (rayColY.hit == true) {
            if (rayColY.distance <= abs(rayY.direction.y)) {
                testCol.Y = 0;
            };
        };

        RayCollision rayColZ = GetRayCollisionMesh(rayZ, chunkA->model.meshes[0], m);
        if (rayColZ.hit == true) {
            //DrawSphere(rayColZ.point + drawOffset, .25, BLUE);
            if (rayColZ.distance <= abs(rayZ.direction.z)) {
                testCol.Z = -(abs(rayZ.direction.z) - rayColZ.distance) / 2.5;
            };
        };
    };

    return testCol;
};

void SnowFlake::step(Mesh* mesh, Material* mat, Vector2 pos, float height, float dist) {
    float dT = GetFrameTime();
    rotation += rotDir * dT;

    float offset = sin(GetTime() + phase);
    position += Vector3{ dir.x * offset, dir.y, dir.z * (1 - offset) } * dT;

    //make DrawMeshInstanced later
    Matrix m = MatrixRotateXYZ(Vector3{ rotation.x, rotation.y, rotation.z });
    m *= MatrixTranslate(position.x, position.y, position.z);

    if (position.y <= 0) resetSnowFlake(this, pos, height, dist);

    DrawMesh(*mesh, *mat, m);
};
void resetSnowFlake(SnowFlake* flake, Vector2 position, float height, float dist) {
    float x = ((rand() % 1000) / 100.f) * (dist / 10) - (dist / 2);
    float y = ((rand() % 1000) / 100.f) * (dist / 10) - (dist / 2);

    Vector3 snowFlakePos = { position.x + x, height, position.y + y };

    flake->position = snowFlakePos;
};

SnowFlake* spawnSnowFlake(Vector2 position, float height, float dist, Vector2 direction) {
    float x = ((rand() % 100) / 10.f) * (dist / 10) - (dist / 2);
    float y = ((rand() % 100) / 10.f) * (dist / 10) - (dist / 2);

    Vector3 snowFlakePos = { position.x + x, height, position.y + y };

    SnowFlake* newFlake = new SnowFlake;
    newFlake->position = snowFlakePos;
    newFlake->dir = Vector3Normalize(Vector3{direction.x, (float)((rand() % 10) + 5) * -1, direction.y}) * 2.75;
    newFlake->rotation = { 0, 0, 0 };
    newFlake->rotDir = { x / dist, y / dist, random(position.y * GetTime() + height) };

    return newFlake;
}

Vector3 Player::WASD() {
    Vector3 movement = { 0, 0, 0 };
    float dSpd = spd * GetFrameTime();

    dir = atan2(camera.rayCamera.position.z - position.z, camera.rayCamera.position.x - position.x) * (180 / PI);
    if (camera.cameraDist < 3) {
        dir = atan2(camera.rayCamera.target.z - position.z, camera.rayCamera.target.x - position.x) * (180 / PI) + 180;
    };

    movement += Movement(dSpd, dir + 180, KEY_W);
    movement += Movement(dSpd, dir, KEY_S);
    movement += Movement(dSpd, dir + 90, KEY_A);
    movement += Movement(dSpd, dir + 270, KEY_D);

    return movement;
};
void Player::jump() {
    if (IsKeyPressed(KEY_SPACE) && velocity.y >= 0) {
        velocity.y -= jumpHeight;
        //std::cout << "jumped\n";
    };
};
float Player::getFloor(World* world, IceWorld* iceWorld) {
    Vector2 chunkVec = { floor(position.x / CHUNK_S), floor(position.z / CHUNK_S) };
    int i = world->checkForChunk(chunkVec);

    Vector3 playerPos = world->player->position;

    Ray ray;
    float tolerance = .75;
    ray.position = { 0, position.y + tolerance, 0 };
    ray.direction = { 0, -1, 0 };

    Vector3 translate = {(chunkVec.x * CHUNK_S) - position.x, 0, (chunkVec.y * CHUNK_S) - position.z };
    Matrix m = MatrixTranslate(translate.x, translate.y, translate.z);

    if (i != -1) {
        RayCollision hit = GetRayCollisionMesh(ray, world->grid->chunks.at(i).model.meshes[0], m);

        Vector2 chunkPos = { position.x, position.z };
        std::vector<BoundingBox> ices;
        getIcesFromChunk(chunkPos, iceWorld, &ices);

        float height = hit.point.y;

        for (int i = 0; i < ices.size(); i++) {
            if (position.x > ices.at(i).min.x && position.x < ices.at(i).max.x && position.z > ices.at(i).min.z && position.z < ices.at(i).max.z && ices.at(i).max.y > height && ices.at(i).min.y < position.y) height = ices.at(i).max.y;
        };

        return height;
    };

    DrawRay(ray, RED);
    return 12;
};
void Player::hitHead(World* world) {
    Vector2 chunkVec = { floor(position.x / CHUNK_S), floor(position.z / CHUNK_S) };
    int i = world->checkForChunk(chunkVec);

    Vector3 playerPos = world->player->position;

    Ray ray;
    float tolerance = .75;
    ray.position = { 0, position.y + tolerance, 0 };
    ray.direction = { 0, 1, 0 };

    Vector3 translate = { (chunkVec.x * CHUNK_S) - position.x, 0, (chunkVec.y * CHUNK_S) - position.z };
    Matrix m = MatrixTranslate(translate.x, translate.y, translate.z);

    if (i != -1) {
        RayCollision hit = GetRayCollisionMesh(ray, world->grid->chunks.at(i).model.meshes[0], m);
      
        if (hit.distance < .5 && hit.hit == true) {
            velocity.y *= -1.1;
        };
    };
}
void Player::setHeight(World* world, IceWorld* iceWorld, float dT, int delay) {
    if ((int)GetTime() > delay) {
        //if (doPerSecond(10, FPS)) std::cout << velocity.y << "\n";
        float testPos = getFloor(world, iceWorld);
        if (velocity.y >= .2) {
            if (testPos == 0) testPos = position.y + velocity.y;

            if (position.y - (velocity.y * dT) < testPos) {
                velocity.y = 0;
            }
            else position.y -= (velocity.y * dT);
        }
        else {
            position.y -= velocity.y * dT;
        }

    if (position.y < testPos) position.y = testPos;
    } else {
        velocity.y = 0;
        position.y = 12;
    };
}
Vector3 Player::iceCol(IceWorld* iceWorld, Vector3 movement) {
    Vector2 chunkPos = { position.x + movement.x, position.z + movement.z };
    std::vector<BoundingBox> ices;
    getIcesFromChunk(chunkPos, iceWorld, &ices);

    Vector3 ret = movement;

    for (int i = 0; i < ices.size(); i++) {
        ret = correctBoxCol(movement, position, ices.at(i), this);
        if (ret != movement) return ret;
    };
    return ret;
};
void Player::enablePlacing() {
    if (IsKeyPressed(KEY_Q)) {
        if (placing == false) {
            placing = true;
            return;
        };
        if (placing == true) {
            placing = false;
            return;
        };
    };
};
void Player::step(World* world, IceWorld* iceWorld) {
    if (IS_WEB_BUILD) jumpHeight = 9;

    velocity.y += GRAVITY;
    Vector3 movement = WASD();
    vecDir = movement;

    movement = iceCol(iceWorld, movement);

    float dT = GetFrameTime();
    float dSpd = spd * dT;
    TestCol col = testOffsetCol(position, vecDir, world, dSpd);
        movement.x *= col.X;
        movement.y *= col.Y;
        movement.z *= col.Z;

    position += movement;
    hitHead(world);
    setHeight(world, iceWorld, dT, 2);

    jump();

    //if (snow > 3) snow = 3;
    //if (snow < 0) snow = 0;

    if (camera.cameraDist > 3) DrawModel(model, position, 1.f, DARKBLUE);

    enablePlacing();

    if (placementHandler != nullptr) {
        if (placing == true) {
            clickData cd = getClickPosition(this, world, iceWorld);
            if (cd.hit == true) placementHandler->position = cd.point;

            placementHandler->step(world, iceWorld, this);
        };
    };
};


void Chunk::edit(Vector3 vec, bool a) {
    int i = (vec.x * (CHUNK_S + 1) + vec.z) * CHUNK_H + vec.y;
    grid[i] = a;
};
bool Chunk::access(Vector3 vec) const  {
    int i = (vec.x * (CHUNK_S + 1) + vec.z) * CHUNK_H + vec.y;
    return grid[i];
};
bool Chunk::access(float x, float y, float z) const  {
    int i = (x * (CHUNK_S + 1) + z) * CHUNK_H + y;
    return grid[i];
};
bool Chunk::access(int x, int y, int z) const  {
    return grid[(x * (CHUNK_S + 1) + z) * CHUNK_H + y];
};

void Chunk::init(Vector2 position, World* world) {

    std::fill(grid, grid + (CHUNK_S + 1) * (CHUNK_S + 1) * CHUNK_H, false);

    for (int i = 0; i < CHUNK_S; i++) {
        for (int q = 0; q < CHUNK_H - 1; q++) {
            for (int u = 0; u < CHUNK_S; u++) {
                if (q < WORLD_HEIGHT + 6) {
                    
                    float per = perlin(WORLD_GRID, Vector2{ (float)i, (float)u } + (position * (float)CHUNK_S)) * WORLD_HEIGHT + 9;
                    if (q < per) {
                        grid[(i * (CHUNK_S + 1) + u) * CHUNK_H + q] = true;
                        
                        float num = rand() % 10;
                        float num2 = rand() % 10;
                        float num3 = rand() % 10;
                        float num4 = rand() % 10;

                        if (num >= 9 && num2 >= 9 && num3 >= 9 && num4 >= 8) world->trees.push_back({ (position.x * (float)CHUNK_S) + u, (float)(floor(per) - .5), (position.y * (float)CHUNK_S) + i, (float)floor(rand() % 3) });
                    }
                    else {
                        grid[(i * (CHUNK_S + 1) + u) * CHUNK_H + q] = false;
                    };
                }
            };
        };
    };
};
int Chunk::getTriangleConfig(Vector3 vec, World* world, Vector2 position) {
	int bIndex = 0b00000000;
    Vector3 tVec = { vec.x + (position.x * CHUNK_S), vec.y, vec.z + (position.y * CHUNK_S) };

    if (vec.x > 0 && vec.z > 0 && vec.x < CHUNK_S - 1 && vec.z < CHUNK_S - 1) {
        if (access(vec) == true)									bIndex += 0b00000001;
        if (access({ vec.x, vec.y, vec.z + 1 }) == true)			bIndex += 0b00000010;
        if (access({ vec.x + 1, vec.y, vec.z + 1 }) == true)		bIndex += 0b00000100;
        if (access({ vec.x + 1, vec.y, vec.z }) == true)			bIndex += 0b00001000;
        if (access({ vec.x, vec.y + 1, vec.z }) == true)			bIndex += 0b00010000;
        if (access({ vec.x, vec.y + 1, vec.z + 1 }) == true)		bIndex += 0b00100000;
        if (access({ vec.x + 1, vec.y + 1, vec.z + 1 }) == true)	bIndex += 0b01000000;
        if (access({ vec.x + 1, vec.y + 1, vec.z }) == true)		bIndex += 0b10000000;
    }
    else {
        if (world->accessVoxel(tVec) == true)									bIndex += 0b00000001;
        if (world->accessVoxel({ tVec.x, tVec.y, tVec.z + 1 }) == true)			bIndex += 0b00000010;
        if (world->accessVoxel({ tVec.x + 1, tVec.y, tVec.z + 1 }) == true)		bIndex += 0b00000100;
        if (world->accessVoxel({ tVec.x + 1, tVec.y, tVec.z }) == true)			bIndex += 0b00001000;
        if (world->accessVoxel({ tVec.x, tVec.y + 1, tVec.z }) == true)			bIndex += 0b00010000;
        if (world->accessVoxel({ tVec.x, tVec.y + 1, tVec.z + 1 }) == true)		bIndex += 0b00100000;
        if (world->accessVoxel({ tVec.x + 1, tVec.y + 1, tVec.z + 1 }) == true)	bIndex += 0b01000000;
        if (world->accessVoxel({ tVec.x + 1, tVec.y + 1, tVec.z }) == true)		bIndex += 0b10000000;
    }
	
	return bIndex;
};
Vector3 Chunk::getSidePoint(int side, Vector3 pos) {
	Vector3 pointOne = {
		(float)points[EdgeVertexIndices[side][0]][0],
        (float)points[EdgeVertexIndices[side][0]][1],
        (float)points[EdgeVertexIndices[side][0]][2]
	};

	Vector3 pointTwo = {
        (float)points[EdgeVertexIndices[side][1]][0],
        (float)points[EdgeVertexIndices[side][1]][1],
        (float)points[EdgeVertexIndices[side][1]][2]
	};

	return (pointOne + pointTwo) / 2.f + pos;
}
void Chunk::addPointsForTriangleConfig(int config, std::vector<Vector3>* triangles, Vector3 pos) {
	for (int i = 0; i < 16;) {
		if (TriangleTable[config][i] == -1) break;
		
		triangles->push_back(getSidePoint(TriangleTable[config][i + 2], pos));
		triangles->push_back(getSidePoint(TriangleTable[config][i + 1], pos));
		triangles->push_back(getSidePoint(TriangleTable[config][i], pos));
        i += 3;
	};
};
Model Chunk::cubeMarch(World* world, Vector2 position) {
	Mesh mesh{ 0 };

	std::vector<Vector3> points;

	for (int i = -1; i < CHUNK_S + 1; i++) {
		for (int q = 0; q < CHUNK_H - 1; q++) {
			for (int u = -1; u < CHUNK_S + 1; u++) {
                Vector3 pos = {(float)i, (float)q, (float)u };

                int config = getTriangleConfig(pos, world, position);
                addPointsForTriangleConfig( config, &points, pos);
			};
		};
	};

	mesh.vertexCount = points.size();
	mesh.triangleCount = points.size() / 3;

	float* vertices = new float[points.size() * 3];

	for (int i = 0; i < points.size(); i++) {
		vertices[i * 3] = points.at(i).x;
		vertices[i * 3 + 1] = points.at(i).y;
		vertices[i * 3 + 2] = points.at(i).z;
	};

	mesh.vertices = vertices;
    mesh.normals = new float[mesh.vertexCount * 3];

    for (int i = 0; i < mesh.vertexCount * 3; i += 9) {
        Vector3 p1 = {
            mesh.vertices[i],
            mesh.vertices[i + 1],
            mesh.vertices[i + 2]
        };

        Vector3 p2 = {
            mesh.vertices[i + 3],
            mesh.vertices[i + 4],
            mesh.vertices[i + 5]
        };

        Vector3 p3 = {
            mesh.vertices[i + 6],
            mesh.vertices[i + 7],
            mesh.vertices[i + 8]
        };

        Vector3 normal = Vector3CrossProduct(p2 - p1, p3 - p1);

        normal = Vector3Normalize(normal);

        for (int j = 0; j < 3; j++) {
            int n = i + j * 3;

            mesh.normals[n] = normal.x;
            mesh.normals[n + 1] = normal.y;
            mesh.normals[n + 2] = normal.z;
        };
    };

	UploadMesh(&mesh, false);

	return LoadModelFromMesh(mesh);
};

Chunk createChunk(Vector2 position, Shader shader, World* world) {
    Chunk newChunk;
    newChunk.position = position;

    newChunk.init(position, world);
    newChunk.model = newChunk.cubeMarch(world, position);
    newChunk.model.materials[0].shader = shader;

    return newChunk;
}

int Grid::getQuad(Vector2 vec) {
    bool xSign = vec.x > 0;
    bool ySign = vec.y > 0;

    if (xSign == true && ySign == true) return 1;
    if (xSign == false && ySign == true) return 2;
    if (xSign == false && ySign == false) return 3;
    if (xSign == true && ySign == false) return 4;
};
void Grid::initQuadrant(std::vector<std::vector<int>>& q, int x, int y) {
    while ((int)q.size() <= x) {
        q.push_back({});
    };

    while ((int)q[x].size() <= y) {
        q[x].push_back(-1);
    };
};
int Grid::getChunkFromQuadrant( const std::vector<std::vector<int>>& q, int x, int y) {
    x = abs(x);
    y = abs(y);

    if (x < 0 || x >= (int)q.size()) return -1;
    if (y < 0 || y >= (int)q[x].size()) return -1;

    return q[x][y];
}

void Grid::initToPos(Vector2 vec) {
    int q = getQuad(vec);

    int x = (int)abs(vec.x);
    int y = (int)abs(vec.y);

    if (q == 1) initQuadrant(q1, x, y);
    if (q == 2) initQuadrant(q2, x, y);
    if (q == 3) initQuadrant(q3, x, y);
    if (q == 4) initQuadrant(q4, x, y);
}
void Grid::add(Chunk chunk) {
    chunks.push_back(chunk);
    int i = chunks.size() - 1;
    int q = getQuad(chunk.position);

    initToPos(chunk.position);
    Vector2 absPos = { abs(chunk.position.x), abs(chunk.position.y) };

    if (q == 1) q1.at(absPos.x).at(absPos.y) = i;
    if (q == 2) q2.at(absPos.x).at(absPos.y) = i;
    if (q == 3) q3.at(absPos.x).at(absPos.y) = i;
    if (q == 4) q4.at(absPos.x).at(absPos.y) = i;
};

void IceWorld::coutIce() {
    //if (doPerSecond(1, FPS)) {
        for (int i = 0; i < chunkIces.size(); i++) {
            std::cout << "CHUNK " << i << ": \n";
            std::cout << "   { " << chunkIces.at(i).chunkPos.x << ", " << chunkIces.at(i).chunkPos.y << " } \n";
            std::cout << "   ";
            for (int q = 0; q < chunkIces.at(i).indices.size(); q++) {
                std::cout << chunkIces.at(i).indices.at(q) << ", ";
            };
            std::cout << "\n";
        };
    //};
};
int IceWorld::checkForChunk(Vector2 chunkPos) {
    Vector2 pos = { floor(chunkPos.x / CHUNK_S), floor(chunkPos.y / CHUNK_S) };
    for (int i = 0; i < chunkIces.size(); i++) {
        if (chunkIces.at(i).chunkPos.x == pos.x && chunkIces.at(i).chunkPos.y == pos.y) return i;
    };

    return -1;
};
void IceWorld::draw(Shader* iceShader) {
    for (int i = 0; i < iceBlocks.size(); i++) {
        Material material = LoadMaterialDefault();
        material.shader = *iceShader;

        //if (doPerSecond(1, FPS)) std::cout << "X pos : " << iceBlocks.at(i).position.x << "   Y pos : " << iceBlocks.at(i).position.y << "   Z pos : " << iceBlocks.at(i).position.z << "\n";

        Matrix m = MatrixRotateX(iceBlocks.at(i).oriention.z);
        m *= MatrixRotateY(iceBlocks.at(i).oriention.y);
        m *= MatrixTranslate(iceBlocks.at(i).position.x, iceBlocks.at(i).position.y, iceBlocks.at(i).position.z);
        DrawMesh(iceModels.at(iceBlocks.at(i).model).meshes[0], material, m);
    };
};

void getIcesFromChunk(Vector2 pos, IceWorld* iceWorld, std::vector<Vector3>* list) {
    int chunkIndex = iceWorld->checkForChunk(pos);

    if (chunkIndex != -1) {
        for (int i = 0; i < iceWorld->chunkIces.at(chunkIndex).indices.size(); i++) {
            Vector3 iPos = iceWorld->iceBlocks.at(iceWorld->chunkIces.at(chunkIndex).indices.at(i)).position;

            if (fract(iPos.x) != 0) {
                list->push_back(Vector3{ floor(iPos.x), iPos.y, iPos.z });
                list->push_back(Vector3{ ceil(iPos.x), iPos.y, iPos.z });
            };

            if (fract(iPos.y) != 0) {
                list->push_back(Vector3{ iPos.x, floor(iPos.y), iPos.z });
                list->push_back(Vector3{ iPos.x, ceil(iPos.y), iPos.z });
            };

            if (fract(iPos.z) != 0) {
                list->push_back(Vector3{ iPos.x, iPos.y, floor(iPos.z) });
                list->push_back(Vector3{ iPos.x, iPos.y, ceil(iPos.z) });
            };
        };
    };
};
void getIcesFromChunk(Vector2 pos, IceWorld* iceWorld, std::vector<BoundingBox>* list) {
    int chunkIndex = iceWorld->checkForChunk(pos);

    float playerRadius = .6f;

    if (chunkIndex != -1) {
        for (int i = 0; i < iceWorld->chunkIces.at(chunkIndex).indices.size(); i++) {
            Vector3 iPos = iceWorld->iceBlocks.at(iceWorld->chunkIces.at(chunkIndex).indices.at(i)).position;

            if (fract(iPos.x) != 0) list->push_back({ 
                Vector3{ floor(iPos.x) - .5f - playerRadius, iPos.y - .5f - playerRadius, iPos.z - .5f - playerRadius }, 
                Vector3{ ceil(iPos.x) + .5f + playerRadius, iPos.y + .5f, iPos.z + .5f + playerRadius } });
            if (fract(iPos.y) != 0) list->push_back({ 
                Vector3{ iPos.x - .5f - playerRadius, floor(iPos.y) - .5f - playerRadius, iPos.z - .5f - playerRadius }, 
                Vector3{ iPos.x + .5f + playerRadius, ceil(iPos.y) + .5f, iPos.z + .5f + playerRadius } });
            if (fract(iPos.z) != 0) list->push_back({ 
                Vector3{ iPos.x - .5f - playerRadius, iPos.y - .5f - playerRadius, floor(iPos.z) - .5f - playerRadius }, 
                Vector3{ iPos.x + .5f + playerRadius, iPos.y + .5f, ceil(iPos.z) + .5f + playerRadius } });
        };
    };
};

int World::checkForChunk(Vector2 position) {
    int q = grid->getQuad(position);

    int x = (int)position.x;
    int y = (int)position.y;

    if (q == 1) return grid->getChunkFromQuadrant(grid->q1, x, y);
    if (q == 2) return grid->getChunkFromQuadrant(grid->q2, x, y);
    if (q == 3) return grid->getChunkFromQuadrant(grid->q3, x, y);
    if (q == 4) return grid->getChunkFromQuadrant(grid->q4, x, y);

    return -1;
};
bool World::accessVoxel(Vector3 position) {
    int chunkX = (int)floor(position.x / CHUNK_S);
    int chunkZ = (int)floor(position.z / CHUNK_S);

    int localX = (int)floor(position.x - chunkX * CHUNK_S);
    int localZ = (int)floor(position.z - chunkZ * CHUNK_S);

    Vector2 chunkPos = { (float)(int)chunkX, (float)(int)chunkZ };
    int i = checkForChunk(chunkPos);

    if (i == -1) {
        Vector2 worldPos = { position.x, position.z };
        return position.y < (perlin(WORLD_GRID, worldPos) * WORLD_HEIGHT) + 9;
    };

    return grid->chunks[i].access(localX, (int)position.y, localZ);
};
void World::setVoxel(Vector3 position, bool a) {
    int chunkX = (int)floor(position.x / CHUNK_S);
    int chunkZ = (int)floor(position.z / CHUNK_S);

    int localX = (int)floor(position.x - chunkX * CHUNK_S);
    int localZ = (int)floor(position.z - chunkZ * CHUNK_S);

    Vector2 chunkPos = { (float)(int)chunkX, (float)(int)chunkZ };
    int i = checkForChunk(chunkPos);

    if (i != -1) {
        grid->chunks[i].edit({ (float)localX, position.y, (float)localZ }, a);
    }
}
bool World::isChunkQueued(Vector2 pos) {
    for (int i = 0; i < queue.size(); i++) {
        if (queue.at(i).x == pos.x && queue.at(i).y == pos.y) return true;
    };

    return false;
}
void World::drawTrees(Model* t1, Model* t2, Model* t3) {
    for (int i = 0; i < trees.size(); i++) {
        if (trees.at(i).w == 1) DrawModel(*t1, Vector3{ trees.at(i).x, trees.at(i).y, trees.at(i).z }, 3, GREEN);
        if (trees.at(i).w == 2) DrawModel(*t2, Vector3{ trees.at(i).x, trees.at(i).y, trees.at(i).z }, 3, RED);
        if (trees.at(i).w == 3) DrawModel(*t3, Vector3{ trees.at(i).x, trees.at(i).y, trees.at(i).z }, 3, RED);
    };
};
void World::draw(int renderDistance) {
    Vector2 playerChunk = {
        floor(player->position.x / CHUNK_S),
        floor(player->position.z / CHUNK_S)
    };

    for (int x = -renderDistance; x <= renderDistance; x++) {
        for (int z = -renderDistance; z <= renderDistance; z++) {

            Vector2 testPos = { playerChunk.x + x, playerChunk.y + z };
            int testNumber = checkForChunk(testPos);

            if (testNumber != -1) {
                Chunk& chunk = grid->chunks.at(testNumber);

                Vector3 drawLocation = { chunk.position.x * CHUNK_S, 0, chunk.position.y * CHUNK_S };
                DrawModel(chunk.model, drawLocation, 1.0f, WHITE);
            };
        };
    };
};
void World::step() {
    int renderDistance = 4;

    Vector2 playerChunk = {
        floor(player->position.x / CHUNK_S),
        floor(player->position.z / CHUNK_S)
    };

    for (int x = -renderDistance; x <= renderDistance; x++) {
        for (int z = -renderDistance; z <= renderDistance; z++) {

            Vector2 testPos = { playerChunk.x + x, playerChunk.y + z };

            int testNumber = checkForChunk(testPos);

            if (testNumber == -1 && !isChunkQueued(testPos)) {
                queue.push_back(testPos);
            };
        }
    }

    if (!queue.empty() && doPerSecond(25, FPS)) {
        Vector2 current = queue.front();
        queue.erase(queue.begin());
        
        grid->add(createChunk(current, *shader, this));
    };
    draw(renderDistance);
};

void World::clean() {
    const float maxDistance = 200;
    const float maxDistanceSq = maxDistance * maxDistance;

    Vector2 playerPos = { player->position.x, player->position.z };

    for (size_t i = 0; i < grid->chunks.size(); ) {
        float dx = grid->chunks[i].position.x * CHUNK_S - playerPos.x;
        float dy = grid->chunks[i].position.y * CHUNK_S - playerPos.y;

        float distanceSq = dx * dx + dy * dy;

        if (distanceSq > maxDistanceSq) {
            grid->chunks[i] = std::move(grid->chunks.back());
            grid->chunks.pop_back();
        }
        else {
            ++i;
        }
    }
};

int nearestCol(std::vector<RayCollision>* cols, Vector3 pos) {
    float dist = 999999999;
    int closest = -1;
    for (int i = 0; i < cols->size(); i++) {
        if (cols->at(i).hit == true) {
            
            float curDist = Vector3DistanceSqr(cols->at(i).point, pos);
            //DrawSphere(cols->at(i).point, .2, Fade(BLUE, .25));
            if (curDist < dist) {
                dist = curDist;
                closest = i;
            };
        };
    };

    return closest;
}
BoundingBox getIceBBox(IceBlock ice) {
    Vector3 iPos = ice.position;
    BoundingBox BBox;

    if (fract(iPos.x) != 0) {
        BBox.min = Vector3{ floor(iPos.x), iPos.y - 1, iPos.z - 1 };
        BBox.max = Vector3{ ceil(iPos.x), iPos.y, iPos.z };
    };

    if (fract(iPos.y) != 0) {
        BBox.min = Vector3{ iPos.x - 1, floor(iPos.y), iPos.z - 1 };
        BBox.max = Vector3{ iPos.x, ceil(iPos.y), iPos.z };
    };

    if (fract(iPos.z) != 0) {
        BBox.min = Vector3{ iPos.x - 1, iPos.y - 1, floor(iPos.z) };
        BBox.max = Vector3{ iPos.x, iPos.y, ceil(iPos.z) };
    };

    return BBox;
}
clickData getClickPosition(Player* player, World* world, IceWorld* iceWorld) {
    //BeginBlendMode(BLEND_ALPHA);
    clickData rData;
    rData.hit = false;
    rData.point = { 0, 0, 0 };
    rData.chunkIndex = 0;
        
    Vector2 mousePos = GetMousePosition();
    if (player->camera.cameraDist < 3) mousePos = { (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2 };

    Ray ray = GetScreenToWorldRay(mousePos, player->camera.rayCamera);
    Vector3 cRay = ray.position;

    std::vector<Vector2> checkedChunks;
    std::vector<Vector3> closeIceBlocks;
    std::vector<RayCollision> cols;

    for (int i = 0; i < 20 + (player->camera.cameraDist / 1.5); i++) {
        cRay += ray.direction;

        Vector2 chunkTest = { floor(cRay.x / CHUNK_S), floor(cRay.z / CHUNK_S) };
        if (checkVector2Vec(checkedChunks, chunkTest) == false) checkedChunks.push_back(chunkTest);
    };

    for (int i = 0; i < checkedChunks.size(); i++) {
        Vector2 testVector = checkedChunks.at(i);
        int chunkIndex = world->checkForChunk(testVector);

        if (chunkIndex != -1) {
            Vector3 pos = { checkedChunks.at(i).x * CHUNK_S, 0, checkedChunks.at(i).y * CHUNK_S };
            getIcesFromChunk({ pos.x, pos.z }, iceWorld, &closeIceBlocks);

            Matrix m = MatrixTranslate(pos.x, pos.y, pos.z);
            cols.push_back(GetRayCollisionMesh(ray, world->grid->chunks.at(chunkIndex).model.meshes[0], m));
        };
    };

    for (int i = 0; i < closeIceBlocks.size(); i++) {
        BoundingBox BBox;
        BBox.min = { closeIceBlocks.at(i).x, closeIceBlocks.at(i).y, closeIceBlocks.at(i).z };
        BBox.max = { closeIceBlocks.at(i).x + 1, closeIceBlocks.at(i).y + 1, closeIceBlocks.at(i).z + 1 };

        //DrawCube(BBox.min, 1.01, 1.01, 1.01, Fade(RED, .5));

        cols.push_back(GetRayCollisionBox(ray, BBox));
    };

    int n = nearestCol(&cols, player->camera.rayCamera.position);
    if (n != -1) {
        rData.hit = true;
        rData.point = cols.at(n).point;
        rData.chunkIndex = world->checkForChunk({ cols.at(n).point.x, cols.at(n).point.z });

        Vector2 locPos = { fmodf(abs(cols.at(n).point.x), CHUNK_S), fmodf(abs(cols.at(n).point.z), CHUNK_S) };
        DrawSphere(cols.at(n).point, .21, RED);
    };

    return rData;
};
void rebuildChunk(Vector2 vector, World* world, Shader shader) {
    int i = world->checkForChunk(vector);
    if (i != -1) {
        world->grid->chunks.at(i).model = world->grid->chunks.at(i).cubeMarch(world, vector);
        world->grid->chunks.at(i).model.materials[0].shader = shader;
    }
};
void dig(Player* player, World* world, Shader shader, IceWorld* iceWorld) {
    clickData cData = getClickPosition(player, world, iceWorld);
    if (player->camera.cameraDist < 3) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            if (doPerSecond(10, FPS)) {
                if (cData.hit == true) {
                    bool change = false;
                    Vector3 dir = (player->camera.rayCamera.position - cData.point) / (player->camera.cameraDist + 10);
                    float add = -1;

                    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                        add = 1;
                        change = true;
                    };
                    dir *= add;

                    //if (add == -1 && player->snow == 3) return;
                    //if (add == 1 && player->snow == 0) return;
                    if (round(cData.point.y + dir.y) > 0 && round(cData.point.y + dir.y) < CHUNK_H - 1) {
                        bool cur = world->accessVoxel({ round(cData.point.x + dir.x), round(cData.point.y + dir.y), round(cData.point.z + dir.z) });
                        //if (add == -1 && cur == true) player->snow++;
                        //if (add == 1 && cur == false) player->snow--;

                        world->setVoxel({ round(cData.point.x + dir.x), round(cData.point.y + dir.y), round(cData.point.z + dir.z) }, change);

                        Vector2 accessPos = { floor(cData.point.x / CHUNK_S), floor(cData.point.z / CHUNK_S) };

                        //std::cout << player->snow << "\n";

                        rebuildChunk(accessPos, world, shader);

                        rebuildChunk({ accessPos.x + 1, accessPos.y }, world, shader);
                        rebuildChunk({ accessPos.x + 1, accessPos.y + 1 }, world, shader);
                        rebuildChunk({ accessPos.x, accessPos.y + 1 }, world, shader);
                        rebuildChunk({ accessPos.x + 1, accessPos.y - 1 }, world, shader);
                        rebuildChunk({ accessPos.x - 1, accessPos.y }, world, shader);
                        rebuildChunk({ accessPos.x - 1, accessPos.y - 1 }, world, shader);
                        rebuildChunk({ accessPos.x - 1, accessPos.y + 1 }, world, shader);
                        rebuildChunk({ accessPos.x, accessPos.y - 1 }, world, shader);
                    };
                };
            };
        };
    }
    else {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (doPerSecond(10, FPS)) {
                if (cData.hit == true) {
                    bool change = false;
                    Vector3 dir = (player->camera.rayCamera.position - cData.point) / (player->camera.cameraDist + 10);
                    float add = -1;

                    if (IsKeyDown(KEY_LEFT_CONTROL)) {
                        add = 1;
                        change = true;
                    };
                    dir *= add;

                    //if (add == -1 && player->snow == 3) return;
                    //if (add == 1 && player->snow == 0) return;
                    if (round(cData.point.y + dir.y) > 0 && round(cData.point.y + dir.y) < CHUNK_H - 1) {
                        bool cur = world->accessVoxel({ round(cData.point.x + dir.x), round(cData.point.y + dir.y), round(cData.point.z + dir.z) });
                        //if (add == -1 && cur == true) player->snow++;
                        //if (add == 1 && cur == false) player->snow--;

                        world->setVoxel({ round(cData.point.x + dir.x), round(cData.point.y + dir.y), round(cData.point.z + dir.z) }, change);

                        Vector2 accessPos = { floor(cData.point.x / CHUNK_S), floor(cData.point.z / CHUNK_S) };

                        //std::cout << player->snow << "\n";

                        rebuildChunk(accessPos, world, shader);

                        rebuildChunk({ accessPos.x + 1, accessPos.y }, world, shader);
                        rebuildChunk({ accessPos.x + 1, accessPos.y + 1 }, world, shader);
                        rebuildChunk({ accessPos.x, accessPos.y + 1 }, world, shader);
                        rebuildChunk({ accessPos.x + 1, accessPos.y - 1 }, world, shader);
                        rebuildChunk({ accessPos.x - 1, accessPos.y }, world, shader);
                        rebuildChunk({ accessPos.x - 1, accessPos.y - 1 }, world, shader);
                        rebuildChunk({ accessPos.x - 1, accessPos.y + 1 }, world, shader);
                        rebuildChunk({ accessPos.x, accessPos.y - 1 }, world, shader);
                    };
                };
            };
        };
    }
};

void PlacementHandler::rotate() {
    if (IsKeyPressed(KEY_R)) rotation.x++;
    rotation.x = (int)rotation.x % 4;

    if (IsKeyPressed(KEY_T)) rotation.y++;
    rotation.y = (int)rotation.y % 4;
};
bool PlacementHandler::checkValid(Vector3 pos, World* world, IceWorld* iceWorld) {
    Vector3 belowPos = { pos.x, pos.y - 1, pos.z };
    Vector3 abovePos = { pos.x, pos.y + 1, pos.z };
    
    bool inside = true;
    bool below = true;
    
    if (world->accessVoxel(pos) == true && world->accessVoxel(abovePos) == true) inside = false;
    if (world->accessVoxel(belowPos) == false) below = false;

    std::vector<Vector3> ices;
    getIcesFromChunk(Vector2{ pos.x, pos.z }, iceWorld, &ices);
    int bIce = checkVector3Vec(ices, belowPos);
    int iIce = checkVector3Vec(ices, pos);

    if (iIce != -1) inside = false;
    if (bIce != -1) below = true;

    if (inside == true and below == true) return true;
    return false;
};
void PlacementHandler::draw(World* world, IceWorld* iceWorld) {
    bool pOne = checkValid(vOne, world, iceWorld);
    bool pTwo = checkValid(vTwo, world, iceWorld);

    BeginBlendMode(BLEND_ALPHA);
    if (pOne == true) DrawModel(*highlightModel, vOne, 1, Fade(BLUE, .25f)); else DrawModel(*highlightModel, vOne, 1, Fade(RED, .25f));
    if (pTwo == true) DrawModel(*highlightModel, vTwo, 1, Fade(BLUE, .25f)); else DrawModel(*highlightModel, vTwo, 1, Fade(RED, .25f));
};
void PlacementHandler::click(IceWorld* iceWorld, Player* player, World* world) {
    if (player->placing == true && checkValid(vOne, world, iceWorld) == true && checkValid(vTwo, world, iceWorld) == true && IsKeyPressed(KEY_E)) {
        int isChunk = iceWorld->checkForChunk({ position.x, position.z });

        if (isChunk != -1) {
            int index = iceWorld->iceBlocks.size() - 1;
            IceBlock newIceBlock;
            newIceBlock.position = (vOne + vTwo) / 2;
            newIceBlock.oriention = { 0, rotation.x * 90 * (PI / 180), rotation.y * 90 * (PI / 180) };
            newIceBlock.model = 0;

            iceWorld->iceBlocks.push_back(newIceBlock);
            iceWorld->chunkIces.at(isChunk).indices.push_back(index + 1);
        };

        if (isChunk == -1) {
            int index = iceWorld->iceBlocks.size();
            IceBlock newIceBlock;
            newIceBlock.position = (vOne + vTwo) / 2;
            newIceBlock.oriention = { 0, rotation.x * 90 * (PI / 180), rotation.y * 90 * (PI / 180) };
            newIceBlock.model = 0;

            iceWorld->iceBlocks.push_back(newIceBlock);
            ChunkIceList newChunkIceList;
            newChunkIceList.chunkPos = Vector2{ floor(position.x / CHUNK_S), floor(position.z / CHUNK_S) };
            iceWorld->chunkIces.push_back(newChunkIceList);

            int chunkIndex = iceWorld->chunkIces.size() - 1;
            iceWorld->chunkIces.at(chunkIndex).indices.push_back(index);
        };

        //iceWorld->coutIce();
        player->placing = false;
    };
};
void PlacementHandler::step(World* world, IceWorld* iceWorld, Player* player) {
    rotate();

    vOne = { floor(position.x), (float)floor(position.y - .1) + 1, floor(position.z) };

    float yaw = rotation.x * 90 * (PI / 180);
    float pitch = rotation.y * 90 * (PI / 180);

    vTwo = { floor(position.x) + cosf(pitch) * sinf(yaw), (float)floor(position.y - .1) + sinf(pitch) + 1, floor(position.z) + cos(pitch) * cos(yaw) };

    draw(world, iceWorld);
    click(iceWorld, player, world);
};