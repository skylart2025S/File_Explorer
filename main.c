#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL_video.h>
#include <SDL_events.h>
#include <SDL2/SDL_ttf.h>
#include <unistd.h>

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    
    TTF_Font *font = TTF_OpenFont("JapaneseText.ttf", 24);
    if(!font)
    {
        printf("Failed to load font: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Color text_color = {255, 255, 255, 255};
    int x, y;
    int mouse_clicked = 0;
    int current_font_size = 24;  // Move outside loop
    
    SDL_Window *pwindow = SDL_CreateWindow("File Explorer",
                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 900,
                          600, SDL_WINDOW_SHOWN);
    SDL_Surface *psurface = SDL_GetWindowSurface(pwindow);
    
    int running = 1;
    while(running)
    {
        SDL_Event event;
        mouse_clicked = 0;  // Reset at start of frame
        
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                running = 0;
            }
            else if(event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(event.button.button == SDL_BUTTON_LEFT)
                {
                    mouse_clicked = 1;
                }
            }
        }

        SDL_GetMouseState(&x, &y);
        SDL_FillRect(psurface, NULL, SDL_MapRGB(psurface->format, 0, 0, 0));

        // Mouse position text
        char text[100];
        snprintf(text, sizeof(text), "Mouse X: %d, Mouse Y: %d", x, y);
        SDL_Surface *text_surface = TTF_RenderText_Solid(font, text, text_color);
        if(text_surface)
        {
            SDL_Rect text_rect = {10, 10, text_surface->w, text_surface->h};
            SDL_BlitSurface(text_surface, NULL, psurface, &text_rect);
            SDL_FreeSurface(text_surface);
        }

        // Rectangle
        int rect_pos_x = 110;
        int rect_pos_y = 150;
        int rect_width = 600;
        int rect_height = 350;
        uint32_t rectColor = 0xFF00FF;
        SDL_Rect rect = {rect_pos_x, rect_pos_y, rect_width, rect_height};
        SDL_FillRect(psurface, &rect, rectColor);

        // Count files
        FILE *fp = popen("ls | wc -l", "r");
        int file_count = 0;
        if(fp)
        {
            fscanf(fp, "%d", &file_count);
            pclose(fp);
        }

        // Calculate font size
        int font_size = 24;
        int max_height = rect_height - 50;
        int line_spacing = 30;
        int required_height = file_count * line_spacing;

        while(required_height > max_height && font_size > 8)
        {
            font_size -= 2;
            line_spacing = font_size + 6;
            required_height = file_count * line_spacing;
        }

        // Only reload font if size changed
        if(font_size != current_font_size)
        {
            TTF_Font *new_font = TTF_OpenFont("JapaneseText.ttf", font_size);
            if(new_font)
            {
                TTF_CloseFont(font);
                font = new_font;
                current_font_size = font_size;
            }
        }

        // List files
        fp = popen("ls", "r");
        if(fp == NULL)
        {
            printf("Failed to run command\n");
            return 1;
        }

        char line[256];
        int y_offset = 200;

        while(fgets(line, sizeof(line), fp) != NULL)
        {
            line[strcspn(line, "\n")] = 0;
            
            SDL_Surface *temp_surface = TTF_RenderText_Solid(font, line, text_color);
            if(!temp_surface)
            {
                continue;
            }
            
            int text_width = temp_surface->w;
            int text_height = temp_surface->h;
            SDL_FreeSurface(temp_surface);
            
            SDL_Color color;
            if(x > 150 && x < 150 + text_width && y > y_offset && y < y_offset + text_height)
            {
                color = (SDL_Color){0, 255, 0, 255};
                if(mouse_clicked)
                {
                    printf("Click on file: %s\n", line);
		    if(chdir(line) == 0)
		    {
			    printf("changed directory\n");
		    }
		    else
		    {
			    printf("Can't access directory\n");
		    }
                }
            }
            else
            {
                color = (SDL_Color){255, 255, 255, 255};
            }
            
            SDL_Surface *file_text_surface = TTF_RenderText_Solid(font, line, color);
            if(file_text_surface)
            {
                SDL_Rect file_text_rect = {150, y_offset, file_text_surface->w, file_text_surface->h};
                SDL_BlitSurface(file_text_surface, NULL, psurface, &file_text_rect);
                SDL_FreeSurface(file_text_surface);
            }
            
            y_offset += line_spacing;
        }
        pclose(fp);

        // Back button
        char back_text[] = "back";
        SDL_Surface *back_temp = TTF_RenderText_Solid(font, back_text, text_color);
        if(back_temp)
        {
            int back_width = back_temp->w;
            int back_height = back_temp->h;
            SDL_FreeSurface(back_temp);

            SDL_Color back_color;
            if(x > 150 && x < 150 + back_width &&
               y > y_offset && y < y_offset + back_height)
            {
                back_color = (SDL_Color){0, 255, 0, 255};
                if(mouse_clicked)
                {
                    printf("Going back...\n");
                    chdir("..");
                }
            }
            else
            {
                back_color = (SDL_Color){255, 255, 255, 255};
            }

            SDL_Surface *back_surface = TTF_RenderText_Solid(font, back_text, back_color);
            if(back_surface)
            {
                SDL_Rect back_rect = {150, y_offset, back_surface->w, back_surface->h};
                SDL_BlitSurface(back_surface, NULL, psurface, &back_rect);
                SDL_FreeSurface(back_surface);
            }
        }`

        SDL_UpdateWindowSurface(pwindow);
        SDL_Delay(16);
    }
    
    TTF_CloseFont(font);
    SDL_DestroyWindow(pwindow);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
