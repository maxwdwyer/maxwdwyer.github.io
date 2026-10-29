#include "headers.h"

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "TESTING");
    #define GLSL_VERSION 100
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(FPS);

    HideCursor();

    RenderTexture2D target = LoadRenderTexture(GAME_WIDTH, GAME_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);


    Shader shader;
    Shader iceShader;
    Shader treeShader;
    Shader flakeShader;

    if (IS_WEB_BUILD == false) {
        shader = LoadShader("ground.vs", "basic.fs");
        iceShader = LoadShader("basic.vs", "ice.fs");
        treeShader = LoadShader("basic.vs", "tree.fs");
        flakeShader = LoadShader("basic.vs", "flake.fs");
    };

    if (IS_WEB_BUILD == true) {
        shader = LoadShader("webGround.vs", "webBasic.fs");
        iceShader = LoadShader("webBasic.vs", "webIce.fs");
        treeShader = LoadShader("webBasic.vs", "webTree.fs");
        flakeShader = LoadShader("webBasic.vs", "webFlake.fs");
    };
    

    int lightPosLoc = GetShaderLocation(shader, "playerLoc");
    int lightDirLoc = GetShaderLocation(shader, "lightDir");
    int camDirLoc = GetShaderLocation(shader, "camDir");
    Vector3 tplayerpos = { 0, 0, 0 };
    Vector3 tlightdir = { .25, -.5, .25 };
    Vector3 tcamDir = { 1., 0, 0 };
    SetShaderValue(shader, lightPosLoc, &tplayerpos, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, lightDirLoc, &tlightdir, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, camDirLoc, &tcamDir, SHADER_UNIFORM_VEC3);


    
    int iceLightPosLoc = GetShaderLocation(iceShader, "playerLoc");
    int iceLightDirLoc = GetShaderLocation(iceShader, "lightDir");
    int iceBackgroundLoc = GetShaderLocation(iceShader, "background");
    int iceWindowSize = GetShaderLocation(iceShader, "windowSize");
    Vector3 tIcePlayerpos = { 0, 0, 0 };
    Vector3 tIceLightdir = { .25, -.5, .25 };
    Vector4 tScreenSize = calcWindowSize();
    Vector2 tIceWindowSize = { tScreenSize.x, tScreenSize.y };
    SetShaderValue(iceShader, iceLightPosLoc, &tIcePlayerpos, SHADER_UNIFORM_VEC3);
    SetShaderValue(iceShader, iceLightDirLoc, &tIceLightdir, SHADER_UNIFORM_VEC3);
    SetShaderValue(iceShader, iceWindowSize, &tIceWindowSize, SHADER_UNIFORM_VEC2);

    
    int treeLightPosLoc = GetShaderLocation(treeShader, "playerLoc");
    int treeLightDirLoc = GetShaderLocation(treeShader, "lightDir");
    Vector3 tTreePlayerpos = { 0, 0, 0 };
    Vector3 tTreeLightdir = { .25, -.5, .25 };
    SetShaderValue(treeShader, treeLightPosLoc, &tTreePlayerpos, SHADER_UNIFORM_VEC3);
    SetShaderValue(treeShader, treeLightDirLoc, &tTreeLightdir, SHADER_UNIFORM_VEC3);

    
    int flakeLightPosLoc = GetShaderLocation(flakeShader, "playerLoc");
    int flakeLightDirLoc = GetShaderLocation(flakeShader, "lightDir");
    Vector3 tFlakePlayerpos = { 0, 0, 0 };
    Vector3 tFlakeLightdir = { .25, -.5, .25 };
    SetShaderValue(flakeShader, flakeLightPosLoc, &tFlakePlayerpos, SHADER_UNIFORM_VEC3);
    SetShaderValue(flakeShader, flakeLightDirLoc, &tFlakeLightdir, SHADER_UNIFORM_VEC3);

    IceWorld iceWorld;
    iceWorld.iceModels.push_back(initIce(&iceShader));

    Camera camera = { 0 };
    camera.position = { 3, 3, 3 };
    camera.target = { 0, 0, 0 };
    camera.up = { 0, 1, 0 };
    camera.fovy = 45.f;
    camera.projection = CAMERA_PERSPECTIVE;

    MyCamera myCamera;
    myCamera.rayCamera = camera;
    myCamera.pitch = .5;
    myCamera.yaw = .5;
    myCamera.cameraDist = 7.5;

    Player player;
    player.camera = myCamera;
    player.position = { 0, 0, 0 };
    player.model = LoadModelFromMesh(GenMeshCylinder(.6, 1.5, 12));

    PlacementHandler ph = {};
    Model phModel = LoadModelFromMesh(GenMeshCube(.85, .85, .85));
    ph.highlightModel = &phModel;
    player.placementHandler = &ph;

    World world;
    world.grid = new Grid;
    world.player = &player;
    world.time = 0;
    world.shader = &shader;

    Model treeOne = LoadModel("treeOne.obj");
    treeOne.materials[0].shader = treeShader;

    Model treeTwo = LoadModel("treeTwo.obj");
    treeTwo.materials[0].shader = treeShader;

    Model treeThree = LoadModel("treeThree.obj");
    treeThree.materials[0].shader = treeShader;

    
   
    RenderTexture2D shadowMap = LoadColorRenderTexture(SHADOWMAP_RESOLUTION, SHADOWMAP_RESOLUTION);

    Matrix lightView = { 0 };
    Matrix lightProj = { 0 };
    Matrix lightViewProj = { 0 };
    int textureActiveSlot = 10;

    SetShaderValue(iceShader, iceBackgroundLoc, &textureActiveSlot, SHADER_UNIFORM_INT);

    Mesh snowFlakeMesh = GenMeshCube(.25, .025, .25);
    Material snowFlakeMaterial = LoadMaterialDefault();
    snowFlakeMaterial.shader = flakeShader;

    std::vector<SnowFlake*> flakes;

    int snowflakeNum = 100;
    int snowflakeDur = 20;
    if (IS_WEB_BUILD) snowflakeNum = 80;
    if (IS_WEB_BUILD) snowflakeDur = 18;

    while (!WindowShouldClose()) {
        if (doPerSecond(3, FPS)) std::cout << "hi \n";
        player.camera.step(player.position);

        tplayerpos = player.position;
        tcamDir = player.camera.dir;

        SetShaderValue(shader, camDirLoc, &tcamDir, SHADER_UNIFORM_VEC3);
        SetShaderValue(shader, lightPosLoc, &tplayerpos, SHADER_UNIFORM_VEC3);

        tIcePlayerpos = player.position;
        SetShaderValue(iceShader, iceLightPosLoc, &tIcePlayerpos, SHADER_UNIFORM_VEC3);

        tTreePlayerpos = player.position;
        SetShaderValue(treeShader, treeLightPosLoc, &tTreePlayerpos, SHADER_UNIFORM_VEC3);

        tFlakePlayerpos = player.position;
        SetShaderValue(flakeShader, flakeLightPosLoc, &tFlakePlayerpos, SHADER_UNIFORM_VEC3);

        Vector4 tScreenSize = calcWindowSize();
        tIceWindowSize = { GAME_WIDTH, GAME_HEIGHT };
        SetShaderValue(iceShader, iceWindowSize, &tIceWindowSize, SHADER_UNIFORM_VEC2);


        if (GetTime() < snowflakeDur && doPerSecond(snowflakeNum, FPS)) {
            flakes.push_back(spawnSnowFlake(Vector2{ player.position.x, player.position.z }, 30, 150, Vector2{ 2, 2 }));
        };


        // -- ICE PASS --
        BeginTextureMode(shadowMap);
        ClearBackground(WHITE);

        BeginMode3D(player.camera.rayCamera);
            world.draw(3);
            world.drawTrees(&treeOne, &treeTwo, &treeThree);
            DrawModel(player.model, player.position, 1.f, DARKBLUE);
        EndMode3D();

        EndTextureMode();

        // -- NORMAL DRAW --

        BeginDrawing();
        BeginTextureMode(target);
        ClearBackground(WHITE);

        rlActiveTextureSlot(textureActiveSlot);
        rlEnableTexture(shadowMap.texture.id);
        

        BeginMode3D(player.camera.rayCamera);
            world.step();
            world.drawTrees(&treeOne, &treeTwo, &treeThree);

            for (int i = 0; i < flakes.size(); i++) flakes.at(i)->step(&snowFlakeMesh, &snowFlakeMaterial, Vector2{ player.position.x, player.position.z }, 30, 150);

            dig(&player, &world, shader, &iceWorld);
            player.step(&world, &iceWorld);
            iceWorld.draw(&iceShader);
        EndMode3D();
        EndTextureMode();
        

        DrawTexturePro(target.texture,
            Rectangle{ 0, 0, (float)target.texture.width, -(float)target.texture.height },
            Rectangle{ 0, 0, (float)tScreenSize.x, (float)tScreenSize.y },
            Vector2{ tScreenSize.z, tScreenSize.w }, 0.0f, WHITE);

        DrawText(" Click to dig \n In third person click + ctrl to place \n In first person right click to build\n Q to enable ice placing, R and T to rotate ice, E to place ice", 5, 40, 30, BLUE);

        EndDrawing();
    };

    CloseWindow();

    return 0;
}