#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
// #include <SDL3_mixer/SDL_mixer.h>
#include <iostream>

// https : // youtu.be/1NLGd32iVY0?si=tVI055A-N2LR5YMG

int main(int argc, char *argv[])
{

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cout << "Erreur lors de l'initialisation de la SDL : " << SDL_GetError();
        return 1;
    }

    int winW = 800, winH = 600;

    // CHargement de la fenetre
    SDL_Window *window = SDL_CreateWindow(
        "SOKOBAN - GAME",
        winW,
        winH,
        SDL_WINDOW_RESIZABLE);

    // Chargement du rendu
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

    // CHargement de la texture
    SDL_Texture *playtexture = IMG_LoadTexture(renderer, "assests/logo.png");

    // Image de fond
    SDL_Texture *bgtexture = IMG_LoadTexture(renderer, "assests/gazon.jpg");

    // Image du block osbtacle
    SDL_Texture *boxTexture = IMG_LoadTexture(renderer, "assests/brique.jpg");

    // Si l'image ne charge pas, on affichera quand même un rectangle
    if (!playtexture)
        SDL_Log("Impossible de charger l'image : %s", SDL_GetError());

    SDL_Event event;
    bool run = true;

    // Creation du personnage (Rectangle de destination)
    SDL_FRect play{300.0f, 200.0f, 70.0f, 70.0f};

    // Ajout de l'obstacle (Caisse)
    SDL_FRect box{450.0f, 400.0f, 70.0f, 30.0f};

    // Ajout de l'obstacle (Mur)
    SDL_FRect wall{600.0f, 200.0f, 50.0f, 400.0f};

    float speed = 40.0f;      // Vitesse de deplacement
    bool Isfollowing = false; // Vérifit si le rect suit la souris

    float oldX,
        oldY,
        oldBoxX,
        oldBoxY;

    while (run)
    {
        // Gestion des éléments
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                run = false; // Sortie de la fenêtre
            if (event.type == SDL_WINDOW_RESIZABLE)
            {
                // Ajustement de la taille de l'écran à la fenêtre
                winW = event.window.data1;
                winH = event.window.data2;
            }

            // Gestion des mouvements du rectangle
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                // On sauvegarde les anciennes positions pour gérer les collisions
                oldX = play.x,
                oldY = play.y,
                oldBoxX = box.x,
                oldBoxY = box.y;

                switch (event.key.scancode)
                {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                {
                    play.y -= speed;
                    break;
                }
                case SDL_SCANCODE_Z:
                case SDL_SCANCODE_DOWN:
                {
                    play.y += speed;
                    break;
                }
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                {
                    play.x -= speed;
                    break;
                }
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                {
                    play.x += speed;
                    break;
                }
                case SDL_SCANCODE_ESCAPE:
                {
                    run = false;
                    std::cout << "Sortie par la touche 'ESCAPE' du clavier.";
                    break;
                }
                default:
                    break;
                }
            }

            // COLLISION perso -> caisse (Pousser)
            if (SDL_HasRectIntersectionFloat(&play, &box))
            {
                // On deplace la caisse dans la même direction
                box.x += (play.x - oldX);
                box.y += (play.y - oldY);

                // COllisin CAISSE -> MUR (On bloque la caisse)
                if (SDL_HasRectIntersectionFloat(&box, &wall))
                {
                    box.x = oldBoxX;
                    box.y = oldBoxY;
                    play.x = oldX; // Le person est blloqué car la caisse ne peut pas bouger
                    play.y = oldY;
                }
            }
            // COLLISION PERSO -> MUR (On bloque le perso)
            if (SDL_HasRectIntersectionFloat(&play, &wall))
            {
                play.x = oldX;
                play.y = oldY;
            }

            // Gestion du rectangle par le motion du curseur (Glisser le rectangle)
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                if (event.button.button == SDL_BUTTON_RIGHT)
                    Isfollowing = !Isfollowing; // Alternes entre true et false
                if (event.button.button == SDL_BUTTON_LEFT)
                    Isfollowing = true; // Boutton gauche active le suivie
            }

            if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                    Isfollowing = false; // Boutton droit desactive le suivi
            }

            // Deplacer le rectangle si la touche gauche est active if (event.type == SDL_EVENT_MOUSE_MOTION && Isfollowing)
            if (event.type == SDL_EVENT_MOUSE_MOTION && Isfollowing)
            {
                // laisser le curseur au milieu du rect
                play.x = event.motion.x - (play.w / 2);
                play.y = event.motion.y - (play.h / 2);
            }
        }

        // Rendu Graphique
        SDL_SetRenderDrawColor(renderer, 100, 200, 90, 255);
        SDL_RenderClear(renderer);

        if (playtexture)
        {
            // SDL3 : Dessin de la texture dans le rectangle
            // NULL : On dessine toute l'image source
            SDL_RenderTexture(renderer, playtexture, NULL, &play);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 255, 129, 200, 255);
            SDL_RenderFillRect(renderer, &play);
        }

        // Arrière plan
        if (bgtexture)
        {
            // NULL : En destination de toute la fenêtre
            SDL_RenderTexture(renderer, bgtexture, NULL, NULL);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 100, 200, 90, 255);
            SDL_RenderClear(renderer);
        }

        // Rendu du mur
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &wall);

        // Rendu de la caisse
        if (boxTexture)
        {
            SDL_RenderTexture(renderer, boxTexture, NULL, &box);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
            SDL_RenderFillRect(renderer, &box);
        }

        // // Rendu du rectangle
        // SDL_SetRenderDrawColor(renderer, 200, 100, 99, 255);
        // SDL_RenderFillRect(renderer, &play);

        // Rendu du personnage
        if (playtexture)
        {
            SDL_RenderTexture(renderer, playtexture, NULL, &play);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 200, 109, 99, 255);
            SDL_RenderFillRect(renderer, &play);
        }

        // Présentation du graphique
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(bgtexture);
    SDL_DestroyTexture(boxTexture);
    SDL_DestroyTexture(playtexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}