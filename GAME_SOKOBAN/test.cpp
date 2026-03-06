#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_gamepad.h> // <‑‑ définit SDL_Gamepad
#include <iostream>

// Affiche toi
 bool stat = true;
 
bool PointInRect(int x, int y, SDL_FRect &rect)
{
    return x >= rect.x &&
           x <= rect.x + rect.w &&
           y >= rect.y &&
           y <= rect.y + rect.h;
}

int main(int argc, char *argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cout << "Erreur lors de l'initialisation de la fenêtre : " << SDL_GetError();
        return 1;
    }

    int winW = 800, winH = 600;
    SDL_Window *window = SDL_CreateWindow("SDL - JOUR 6", winW, winH, SDL_WINDOW_RESIZABLE); // Le pointeur vers la fenêtre du jeu
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);                               // La variable du rendu
    SDL_Gamepad *gamepad = nullptr;                                                          // La manette de jeu
    SDL_Event event;                                                                         // La variable chargée des évènements
    bool run = true;                                                                         // L'unique condition qui fait tourner le jeu

    // Il fallait seulement changer "SDL_Rect" en "SDL_FRect"
    SDL_FRect play{300.0f, 200.0f, 60.0f, 60.0f};

    // Variables temporelles
    Uint64 lastime = SDL_GetTicks();
    float speed = 400.0f;
    bool clicked = false, click = false, clock = false;

    bool isDragging = false;
    float offsteX = 0.0f, offsteY = 0.0f;

    while (run)
    {
        // On quitte la fenêtre avec la croix
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                run = false;

            if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE)
                run = false;

            // Si la fenêtre change de dimensions
            if (event.type == SDL_EVENT_WINDOW_RESIZED)
            {
                winW = event.window.data1; // Nouvelle longeur
                winH = event.window.data2; // Nouvelle hauteur
            }

            // Adaptation du rect en fonction du clique
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                float mx, my;
                mx = event.button.x;
                my = event.button.y;

                if (PointInRect((int)mx, (int)my, play))
                    clock = !clock;

                // On divise en deux afin de toujours rester au centre
                play.x = mx - (play.w / 2.0f);
                play.y = my - (play.h / 2.0f);
                // play.x = event.motion.x - offsteX;
                // play.y = event.motion.y - offsteY;
            }

            // Evènement sur la souris (Clique gauche)
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                float mx, my;
                SDL_GetMouseState(&mx, &my); // On récupère la position de la souris
                if (PointInRect((int)mx, (int)my, play))
                    clicked = !clicked;
            }

            // Evènement sur la souris (Clique droit)
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_RIGHT)
            {
                float mx, my;
                SDL_GetMouseState(&mx, &my); // On récupère la position de la souris

                std::cout << "\n\n====================================="
                          << "\nPosition X (au clique droit) : " << mx
                          << "\nPosition Y (au clique droit) : " << my
                          << "\n=====================================";

                if (PointInRect((int)mx, (int)my, play))
                    click = !click;

                // Permet de glisser après un click gauche
                if (PointInRect((int)event.button.x, (int)event.button.y, play))
                {
                    isDragging = true;
                    offsteX = event.button.x - play.x;
                    offsteY = event.button.y - play.y;
                }
            }

            // Ce qui me permettra de faire glisser mon rect
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                if (PointInRect((int)event.button.x, (int)event.button.y, play))
                {
                    isDragging = true;
                    offsteX = event.button.x - play.x;
                    offsteY = event.button.y - play.y;
                }
            }

            // On arrete de glisser
            if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT)
                isDragging = false;

            if (event.type == SDL_EVENT_MOUSE_MOTION && isDragging)
            {
                play.x = event.motion.x - offsteX;
                play.y = event.motion.y - offsteY;
            }

            // ======================== LA GAMEPAD ==========================
            if (event.type == SDL_EVENT_GAMEPAD_ADDED)
            {
                gamepad = SDL_OpenGamepad(event.gdevice.which);
            }

            if (event.type == SDL_EVENT_GAMEPAD_REMOVED)
            {
                SDL_CloseGamepad(gamepad);
                gamepad = nullptr;
            }

            if (gamepad)
            {
                float x = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
                float y = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);

                play.x += static_cast<int>(x * 5);
                play.y += static_cast<int>(y * 5);

                if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_START))
                    run = false;
            }
        }

        // Variables temporelles actuelles
        Uint64 tempsCourant = SDL_GetTicks();
        float deltatime = (tempsCourant - lastime) / 1000.0f;
        lastime = tempsCourant;

        // Etat du clavier;
        const bool *keyBoardState = SDL_GetKeyboardState(NULL);

        if (keyBoardState[SDL_SCANCODE_W] || keyBoardState[SDL_SCANCODE_UP])
            play.y -= speed * deltatime;
        if (keyBoardState[SDL_SCANCODE_S] || keyBoardState[SDL_SCANCODE_DOWN])
            play.y += speed * deltatime;
        if (keyBoardState[SDL_SCANCODE_A] || keyBoardState[SDL_SCANCODE_LEFT])
            play.x -= speed * deltatime;
        if (keyBoardState[SDL_SCANCODE_D] || keyBoardState[SDL_SCANCODE_RIGHT])
            play.x += speed * deltatime;

        // Limites de parcours
        if (play.x < 0)
            play.x = 0;
        if (play.y < 0)
            play.y = 0;
        if (play.x + play.w > winW)
            play.x = winW - play.w;
        if (play.y + play.h > winH)
            play.y = winH - play.h;

        SDL_SetRenderDrawColor(renderer, 30, 90, 20, 255);
        SDL_RenderClear(renderer);

        if (clicked)
        {
            SDL_SetRenderDrawColor(renderer, 200, 100, 209, 255);
        }
        else if (click)
        {
            SDL_SetRenderDrawColor(renderer, 10, 50, 39, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 8, 10, 9, 255);
        }

        SDL_RenderFillRect(renderer, &play);

        SDL_RenderPresent(renderer);
    }

    if (gamepad)
        SDL_CloseGamepad(gamepad);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}