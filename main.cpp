#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <thread>
#include "4DFunctions.h"
#include <atomic>
#include <memory>
#include <fstream>
#include <sstream>
#include <filesystem>

int WIDTH = 800;
int HEIGHT = 800;
std::vector<uint32_t> pixels(WIDTH*HEIGHT, 0xFF000000);
int frame = 0;

struct Cam { vec4 pos, fwd, up, right; };

const char* RECORD_PATH = "recording.bin";
const size_t PIXEL_BYTES  = (size_t)WIDTH * HEIGHT * sizeof(uint32_t);
const size_t RECORD_SIZE  = PIXEL_BYTES + sizeof(Cam) + sizeof(float);

static bool camStateIsSane(const Cam& c){
    const float* v = reinterpret_cast<const float*>(&c);
    for (int i = 0; i < 16; ++i) if (!std::isfinite(v[i])) return false;
    return std::abs(c.fwd.mag()-1) < 1e-3f && std::abs(c.up.mag()-1) < 1e-3f && std::abs(c.right.mag()-1) < 1e-3f;
}

enum mode{LIVE,RECORD,REPLAY,CONTINUE};
mode currentMode = REPLAY;

std::ofstream saveFile;
std::ifstream replayFile;



//straight down x axis




Cam cam = Cam{
    vec4(-3.0f, 1, 0, 0),
    vec4(1,0,0,0).normalize(),
    vec4(0,1,0,0).normalize(),
    vec4(0,0,1,0).normalize()
};
float camVel = 0.05f;

//diagonal down
/*
 v e*c4 camposInit = vec4(-1.0f, 2, 0, 0);
 vec4 forward = vec4(0.5f,-0.5f,0,0).normalize();
 vec4 up      = vec4(0.5f,0.5f,0,0).normalize();
 vec4 right   = vec4(0,0,1,0).normalize();
 */

void moveCamera(sdf& s, Cam& c, float dist){
    for(float moved = 0; moved < dist; ){
        float step = std::min(dist - moved, 0.01f);
        c.pos = c.pos + c.fwd * step;  moved += step;

        for(int i = 0; i < 3; ++i){
            float d = s.space_sdf(c.pos).dist;
            if(std::abs(d) < 1e-5f) break;
            vec4 n = s.spaceNorm(c.pos);
            if(n.mag() == 0.0f) break;
            c.pos = c.pos - n * d;
        }
        vec4 n = s.spaceNorm(c.pos);
        if(n.mag() > 0.0f){
            vec4 f = c.fwd   - n * c.fwd.dot(n);
            vec4 u = c.up    - n * c.up.dot(n);
            vec4 r = c.right - n * c.right.dot(n);
            if(f.mag() > 1e-6f) c.fwd = f;
            if(u.mag() > 1e-6f) c.up = u;
            if(r.mag() > 1e-6f) c.right = r;
        }
        c.fwd   = c.fwd.normalize();
        c.up    = (c.up - c.fwd * c.up.dot(c.fwd)).normalize();
        c.right = (c.right - c.fwd * c.right.dot(c.fwd) - c.up * c.right.dot(c.up)).normalize();
    }
}

void pixel_space(std::atomic<int>& rowCounter, std::atomic<int>& rowsDone, sdf& scene, std::atomic<vec4>& campos, std::atomic<bool>& quitFlag){
    while (!quitFlag.load()) {
        int y = rowCounter.fetch_add(1);
        if (y >= HEIGHT) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        } else{
            int x = 0;

            while(x < WIDTH){
                float screenX = ((float)x / (float)WIDTH) - 0.5f;
                float screenY = 0.5f - ((float)y / (float)HEIGHT);

                sdf_return dist = scene.raymarch_space(campos, (cam.fwd + (cam.right * screenX) + (cam.up * screenY)).normalize());

                if(dist.dist == -1){
                    pixels[y * WIDTH + x] = (uint32_t) 0x00000000;
                } else{
                    float brightness = 1.0f - (dist.dist / 10.0f);
                    if (brightness < 0.0f){brightness = 0.0f;}
                    if (brightness > 1.0f){brightness = 1.0f;}

                    uint8_t r = (dist.color >> 16) & 0xFF;
                    uint8_t g = (dist.color >> 8)  & 0xFF;
                    uint8_t b = (dist.color) & 0xFF;

                    uint8_t shadedR = static_cast<uint8_t>(r * brightness);
                    uint8_t shadedG = static_cast<uint8_t>(g * brightness);
                    uint8_t shadedB = static_cast<uint8_t>(b * brightness);

                    pixels[y * WIDTH + x] = (0xFF << 24) | (shadedR << 16) | (shadedG << 8) | shadedB;
                }
                x++;
            }
            rowsDone.fetch_add(1);
        }
    }
}

void pixel_real(std::atomic<int>& rowCounter, std::atomic<int>& rowsDone, sdf& scene, std::atomic<vec4>& campos, std::atomic<bool>& quitFlag){
    while (!quitFlag.load()) {
        int y = rowCounter.fetch_add(1);
        if (y >= HEIGHT) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        } else{
            int x = 0;

            while(x < WIDTH){
                float screenX = ((float)x / (float)WIDTH) - 0.5f;
                float screenY = 0.5f - ((float)y / (float)HEIGHT);

                sdf_return dist = scene.raymarch(campos, (cam.fwd + (cam.right * screenX) + (cam.up * screenY)).normalize());

                if(dist.dist == -1){
                    pixels[y * WIDTH + x] = (uint32_t) 0x00000000;
                } else{
                    float brightness = 1.0f - (dist.dist / 10.0f);
                    if (brightness < 0.0f){brightness = 0.0f;}
                    if (brightness > 1.0f){brightness = 1.0f;}

                    /*
                    uint8_t colorVal = static_cast<uint8_t>(brightness * 255.0f);

                    pixels[y * WIDTH + x] = (0xFF << 24) | (colorVal << 16) | (colorVal << 8) | colorVal;*/


                    uint8_t r = (dist.color >> 16) & 0xFF;
                    uint8_t g = (dist.color >> 8)  & 0xFF;
                    uint8_t b = (dist.color) & 0xFF;

                    uint8_t shadedR = static_cast<uint8_t>(r * brightness);
                    uint8_t shadedG = static_cast<uint8_t>(g * brightness);
                    uint8_t shadedB = static_cast<uint8_t>(b * brightness);

                    pixels[y * WIDTH + x] = (0xFF << 24) | (shadedR << 16) | (shadedG << 8) | shadedB;
                }
                //std::cout << "finished pixel: " << y * WIDTH + x << std::endl;
                x++;
            }
            rowsDone.fetch_add(1);
        }
    }
}

int main(int argc, char **argv) {

    auto lastTime = std::chrono::steady_clock::now();
    float deltaTime = 0;
    float totalTime = 0;

    //std::vector<float>* dists = new std::vector<float>();

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "SDL Failed to Initialize" << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Render", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    if(!window){
        std::cout << "window could not be created" << std::endl;
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    bool quit = false;
    SDL_Event e;

    unsigned int num_threads = std::thread::hardware_concurrency();
    if(num_threads == 0){num_threads = 1;}


    std::atomic<int> rowCounter(HEIGHT);
    std::atomic<int> rowsDone(HEIGHT);
    std::atomic<bool> workerQuit(false);
    std::vector<std::thread> pool;

    sdf scene1 = sdf();

    std::atomic<vec4> campos;
    campos.store(cam.pos);

    if (currentMode == RECORD) {
        saveFile.open(RECORD_PATH, std::ios::out | std::ios::binary | std::ios::trunc);
    }
    else if(currentMode == CONTINUE){
        uintmax_t fullRecords = 0;
        std::ifstream scanFile(RECORD_PATH, std::ios::binary | std::ios::ate);
        if (scanFile.is_open()) {
            uintmax_t fileSize = static_cast<uintmax_t>(scanFile.tellg());
            fullRecords = fileSize / RECORD_SIZE;

            if (fileSize % RECORD_SIZE != 0) {
                std::cout << "Dropping incomplete frame from end of file." << std::endl;
            }

            if (fullRecords > 0) {
                scanFile.seekg(static_cast<std::streamoff>((fullRecords - 1) * RECORD_SIZE + PIXEL_BYTES), std::ios::beg);
                Cam saved;
                scanFile.read(reinterpret_cast<char*>(&saved), sizeof(Cam));
                if (scanFile && camStateIsSane(saved)) {
                    cam = saved;
                } else {
                    std::cout << "Last record isn't a valid camera state (old format?) - starting fresh." << std::endl;
                    fullRecords = 0;
                }
                float vel;
                scanFile.read(reinterpret_cast<char*>(&vel), sizeof(float));
                camVel = vel;
            }
        } else {
            std::cout << "No existing recording file found. Creating new session." << std::endl;
        }

        { std::ofstream touch(RECORD_PATH, std::ios::binary | std::ios::app); }

        saveFile.open(RECORD_PATH, std::ios::in | std::ios::out | std::ios::binary);
        saveFile.seekp(static_cast<std::streamoff>(fullRecords * RECORD_SIZE), std::ios::beg);
    }
    else if (currentMode == REPLAY) {
        replayFile.open(RECORD_PATH, std::ios::in | std::ios::binary);
        if (!replayFile.is_open()) {
            std::cout << "Failed to open recording.txt" << std::endl;
            currentMode = LIVE;
        }
    }

    for (int i = 0; i < num_threads; ++i) {
        pool.emplace_back(pixel_real, std::ref(rowCounter), std::ref(rowsDone), std::ref(scene1), std::ref(campos), std::ref(workerQuit));
    }

    while(!quit){
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
        }
        auto currentTime = std::chrono::steady_clock::now();
        deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        totalTime += deltaTime;
        lastTime = currentTime;
        frame+=1;
        std::cout << "time for last frame: " << deltaTime << std::endl;


        if (currentMode == REPLAY) {
            Cam skipcam;
            float skipVel;
            if (replayFile.read(reinterpret_cast<char*>(pixels.data()), PIXEL_BYTES) &&
                replayFile.read(reinterpret_cast<char*>(&skipcam), sizeof(skipcam)) &&
                replayFile.read(reinterpret_cast<char*>(&skipVel), sizeof(skipVel))) {
            } else {
                std::cout << "Replay finished, looping" << std::endl;
                replayFile.clear();
                replayFile.seekg(0, std::ios::beg);
            }
        }
        else {
            moveCamera(scene1, cam, camVel);

            campos.store(cam.pos);
            std::cout << "(" << cam.pos.x << ", " << cam.pos.y << ", " << cam.pos.z << ", " << cam.pos.w << ")" << std::endl;

            rowsDone.store(0);
            rowCounter.store(0);
            while (rowsDone.load() < HEIGHT) {std::this_thread::yield();}

            if (currentMode == RECORD || currentMode == CONTINUE) {
                saveFile.write(reinterpret_cast<const char*>(pixels.data()), PIXEL_BYTES);
                saveFile.write(reinterpret_cast<const char*>(&cam), sizeof(Cam));
                saveFile.write(reinterpret_cast<const char*>(&camVel), sizeof(camVel));
                saveFile.flush();
            }
        }

        SDL_UpdateTexture(texture, NULL, pixels.data(), WIDTH * sizeof(uint32_t));
        if(currentMode == REPLAY){
            while(deltaTime < 0.02f){
                auto currentTime = std::chrono::steady_clock::now();
                deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    workerQuit.store(true);
    for (auto& t : pool) {
        if (t.joinable()) {
            t.join();
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
