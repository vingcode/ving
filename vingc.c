#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024
#define MAX_ELEMENTS 100
#define MAX_STYLES 50

typedef enum {
    ELEMENT_BUTTON,
    ELEMENT_TEXT,
    ELEMENT_IMG,
    ELEMENT_VIDEO,
    ELEMENT_AUDIO,
    ELEMENT_IFRAME,
    ELEMENT_UNKNOWN
} ElementType;

typedef struct {
    char name[256];
    char value[512];
} Style;

typedef struct {
    ElementType type;
    char params[256][64];
    int param_count;
    char content[1024];
    Style styles[10];
    int style_count;
} VingElement;

typedef struct {
    VingElement elements[MAX_ELEMENTS];
    int element_count;
    Style global_styles[MAX_STYLES];
    int global_style_count;
    char page_title[256];
    char page_style[64];
} VingProgram;

// Функция для удаления пробелов в начале и конце строки
void trim(char *str) {
    int start = 0;
    int end = strlen(str) - 1;
    
    while (isspace(str[start])) start++;
    while (end > start && isspace(str[end])) end--;
    
    str[end + 1] = '\0';
    if (start > 0) {
        memmove(str, str + start, end - start + 2);
    }
}

// Парсинг элемента button
int parse_button(const char *line, VingElement *element) {
    element->type = ELEMENT_BUTTON;
    element->param_count = 0;
    element->content[0] = '\0';
    element->style_count = 0;
    
    // Ищем открывающую скобку
    const char *open_brace = strchr(line, '(');
    if (!open_brace) {
        // button без параметров, но может быть текст после двоеточия
        const char *colon = strchr(line, ':');
        if (colon) {
            char content[1024];
            strncpy(content, colon + 1, 1023);
            content[1023] = '\0';
            trim(content);
            strncpy(element->content, content, 1023);
            element->content[1023] = '\0';
        }
        return 1;
    }
    
    // Ищем закрывающую скобку
    const char *close_brace = strchr(open_brace, ')');
    if (!close_brace) {
        return 0;
    }
    
    // Извлекаем параметры между скобками
    int len = close_brace - open_brace - 1;
    if (len > 0 && len < 256) {
        char params_str[256];
        strncpy(params_str, open_brace + 1, len);
        params_str[len] = '\0';
        trim(params_str);
        
        if (strlen(params_str) > 0) {
            // Разделяем параметры по запятой
            char *token = strtok(params_str, ",");
            
            while (token != NULL && element->param_count < 255) {
                trim(token);
                if (strlen(token) > 0) {
                    // Проверяем, является ли параметр ссылкой (link:URL)
                    if (strncmp(token, "link:", 5) == 0) {
                        // Извлекаем URL
                        char url[512];
                        strncpy(url, token + 5, 511);
                        url[511] = '\0';
                        trim(url);
                        strncpy(element->params[element->param_count], url, 63);
                        element->params[element->param_count][63] = '\0';
                        element->param_count++;
                        // Помечаем, что это ссылка
                        strncpy(element->params[element->param_count], "LINK_FLAG", 63);
                        element->param_count++;
                    } else {
                        // Обычный параметр (стиль)
                        strncpy(element->params[element->param_count], token, 63);
                        element->params[element->param_count][63] = '\0';
                        element->param_count++;
                    }
                }
                token = strtok(NULL, ",");
            }
        }
    }
    
    // Ищем двоеточие для текста кнопки (после закрывающей скобки)
    const char *colon = strchr(close_brace, ':');
    if (colon) {
        char content[1024];
        strncpy(content, colon + 1, 1023);
        content[1023] = '\0';
        trim(content);
        strncpy(element->content, content, 1023);
        element->content[1023] = '\0';
    }
    
    return 1;
}

// Парсинг элемента text
int parse_text(const char *line, VingElement *element) {
    element->type = ELEMENT_TEXT;
    element->param_count = 0;
    element->content[0] = '\0';
    element->style_count = 0;
    
    // Ищем открывающую скобку
    const char *open_brace = strchr(line, '(');
    if (!open_brace) {
        return 0;
    }
    
    // Ищем закрывающую скобку
    const char *close_brace = strchr(open_brace, ')');
    if (!close_brace) {
        return 0;
    }
    
    // Извлекаем параметры
    int len = close_brace - open_brace - 1;
    if (len > 0 && len < 256) {
        char params_str[256];
        strncpy(params_str, open_brace + 1, len);
        params_str[len] = '\0';
        trim(params_str);
        
        // Разделяем параметры по запятой
        char *token = strtok(params_str, ",");
        while (token != NULL && element->param_count < 256) {
            trim(token);
            if (strlen(token) > 0) {
                strncpy(element->params[element->param_count], token, 63);
                element->params[element->param_count][63] = '\0';
                element->param_count++;
            }
            token = strtok(NULL, ",");
        }
    }
    
    // Ищем двоеточие для контента
    const char *colon = strchr(close_brace, ':');
    if (colon) {
        char content[1024];
        strncpy(content, colon + 1, 1023);
        content[1023] = '\0';
        trim(content);
        strncpy(element->content, content, 1023);
        element->content[1023] = '\0';
    }
    
    return 1;
}

// Парсинг элемента img
int parse_img(const char *line, VingElement *element) {
    element->type = ELEMENT_IMG;
    element->param_count = 0;
    element->content[0] = '\0';
    element->style_count = 0;
    
    // Ищем открывающую скобку
    const char *open_brace = strchr(line, '(');
    if (!open_brace) {
        return 0;
    }
    
    // Ищем закрывающую скобку
    const char *close_brace = strchr(open_brace, ')');
    if (!close_brace) {
        return 0;
    }
    
    // Извлекаем параметры
    int len = close_brace - open_brace - 1;
    if (len > 0 && len < 256) {
        char params_str[256];
        strncpy(params_str, open_brace + 1, len);
        params_str[len] = '\0';
        trim(params_str);
        
        if (strlen(params_str) > 0) {
            strncpy(element->params[0], params_str, 63);
            element->params[0][63] = '\0';
            element->param_count = 1;
        }
    }
    
    // Ищем двоеточие для пути к изображению
    const char *colon = strchr(close_brace, ':');
    if (colon) {
        char content[1024];
        strncpy(content, colon + 1, 1023);
        content[1023] = '\0';
        trim(content);
        strncpy(element->content, content, 1023);
        element->content[1023] = '\0';
    }
    
    return 1;
}

// Парсинг элемента video
int parse_video(const char *line, VingElement *element) {
    element->type = ELEMENT_VIDEO;
    element->param_count = 0;
    element->content[0] = '\0';
    element->style_count = 0;
    
    // Ищем открывающую скобку
    const char *open_brace = strchr(line, '(');
    if (!open_brace) {
        return 0;
    }
    
    // Ищем закрывающую скобку
    const char *close_brace = strchr(open_brace, ')');
    if (!close_brace) {
        return 0;
    }
    
    // Извлекаем параметры
    int len = close_brace - open_brace - 1;
    if (len > 0 && len < 256) {
        char params_str[256];
        strncpy(params_str, open_brace + 1, len);
        params_str[len] = '\0';
        trim(params_str);
        
        // Разделяем параметры по запятой
        char *token = strtok(params_str, ",");
        while (token != NULL && element->param_count < 255) {
            trim(token);
            if (strlen(token) > 0) {
                strncpy(element->params[element->param_count], token, 63);
                element->params[element->param_count][63] = '\0';
                element->param_count++;
            }
            token = strtok(NULL, ",");
        }
    }
    
    // Ищем двоеточие для пути к видео
    const char *colon = strchr(close_brace, ':');
    if (colon) {
        char content[1024];
        strncpy(content, colon + 1, 1023);
        content[1023] = '\0';
        trim(content);
        strncpy(element->content, content, 1023);
        element->content[1023] = '\0';
    }
    
    return 1;
}

// Парсинг элемента audio
int parse_audio(const char *line, VingElement *element) {
    element->type = ELEMENT_AUDIO;
    element->param_count = 0;
    element->content[0] = '\0';
    element->style_count = 0;
    
    // Ищем открывающую скобку
    const char *open_brace = strchr(line, '(');
    if (!open_brace) {
        return 0;
    }
    
    // Ищем закрывающую скобку
    const char *close_brace = strchr(open_brace, ')');
    if (!close_brace) {
        return 0;
    }
    
    // Извлекаем параметры
    int len = close_brace - open_brace - 1;
    if (len > 0 && len < 256) {
        char params_str[256];
        strncpy(params_str, open_brace + 1, len);
        params_str[len] = '\0';
        trim(params_str);
        
        // Разделяем параметры по запятой
        char *token = strtok(params_str, ",");
        while (token != NULL && element->param_count < 255) {
            trim(token);
            if (strlen(token) > 0) {
                strncpy(element->params[element->param_count], token, 63);
                element->params[element->param_count][63] = '\0';
                element->param_count++;
            }
            token = strtok(NULL, ",");
        }
    }
    
    // Ищем двоеточие для пути к аудио
    const char *colon = strchr(close_brace, ':');
    if (colon) {
        char content[1024];
        strncpy(content, colon + 1, 1023);
        content[1023] = '\0';
        trim(content);
        strncpy(element->content, content, 1023);
        element->content[1023] = '\0';
    }
    
    return 1;
}

// Парсинг элемента iframe
int parse_iframe(const char *line, VingElement *element) {
    element->type = ELEMENT_IFRAME;
    element->param_count = 0;
    element->content[0] = '\0';
    element->style_count = 0;
    
    // Ищем открывающую скобку
    const char *open_brace = strchr(line, '(');
    if (!open_brace) {
        return 0;
    }
    
    // Ищем закрывающую скобку
    const char *close_brace = strchr(open_brace, ')');
    if (!close_brace) {
        return 0;
    }
    
    // Извлекаем параметры
    int len = close_brace - open_brace - 1;
    if (len > 0 && len < 256) {
        char params_str[256];
        strncpy(params_str, open_brace + 1, len);
        params_str[len] = '\0';
        trim(params_str);
        
        // Разделяем параметры по запятой
        char *token = strtok(params_str, ",");
        while (token != NULL && element->param_count < 255) {
            trim(token);
            if (strlen(token) > 0) {
                strncpy(element->params[element->param_count], token, 63);
                element->params[element->param_count][63] = '\0';
                element->param_count++;
            }
            token = strtok(NULL, ",");
        }
    }
    
    // Ищем двоеточие для URL
    const char *colon = strchr(close_brace, ':');
    if (colon) {
        char content[1024];
        strncpy(content, colon + 1, 1023);
        content[1023] = '\0';
        trim(content);
        strncpy(element->content, content, 1023);
        element->content[1023] = '\0';
    }
    
    return 1;
}

// Парсинг директив (style:, title:)
int parse_directive(const char *line, VingProgram *program) {
    char line_copy[MAX_LINE_LENGTH];
    strncpy(line_copy, line, MAX_LINE_LENGTH - 1);
    line_copy[MAX_LINE_LENGTH - 1] = '\0';
    trim(line_copy);
    
    // Пропускаем пустые строки и комментарии
    if (line_copy[0] == '\0' || line_copy[0] == '#') {
        return 0;
    }
    
    // Проверяем директиву title:
    if (strncmp(line_copy, "title:", 6) == 0) {
        char title[256];
        strncpy(title, line_copy + 6, 255);
        title[255] = '\0';
        trim(title);
        
        // Удаляем комментарий, если есть
        char *comment = strchr(title, '#');
        if (comment) {
            *comment = '\0';
            trim(title);
        }
        
        if (strlen(title) > 0) {
            strncpy(program->page_title, title, 255);
            program->page_title[255] = '\0';
        }
        return 1;
    }
    
    // Проверяем директиву style:
    if (strncmp(line_copy, "style:", 6) == 0) {
        char style[64];
        strncpy(style, line_copy + 6, 63);
        style[63] = '\0';
        trim(style);
        
        // Удаляем комментарий, если есть
        char *comment = strchr(style, '#');
        if (comment) {
            *comment = '\0';
            trim(style);
        }
        
        if (strlen(style) > 0) {
            strncpy(program->page_style, style, 63);
            program->page_style[63] = '\0';
        }
        return 1;
    }
    
    return 0;
}

// Парсинг строки кода Ving
int parse_ving_line(const char *line, VingElement *element) {
    char line_copy[MAX_LINE_LENGTH];
    strncpy(line_copy, line, MAX_LINE_LENGTH - 1);
    line_copy[MAX_LINE_LENGTH - 1] = '\0';
    trim(line_copy);
    
    // Пропускаем пустые строки и комментарии
    if (line_copy[0] == '\0' || line_copy[0] == '#') {
        return 0;
    }
    
    // Пропускаем директивы (они обрабатываются отдельно)
    if (strncmp(line_copy, "title:", 6) == 0 || strncmp(line_copy, "style:", 6) == 0) {
        return 0;
    }
    
    // Определяем тип элемента
    if (strncmp(line_copy, "button", 6) == 0) {
        return parse_button(line_copy, element);
    } else if (strncmp(line_copy, "text", 4) == 0) {
        return parse_text(line_copy, element);
    } else if (strncmp(line_copy, "img", 3) == 0) {
        return parse_img(line_copy, element);
    } else if (strncmp(line_copy, "video", 5) == 0) {
        return parse_video(line_copy, element);
    } else if (strncmp(line_copy, "audio", 5) == 0) {
        return parse_audio(line_copy, element);
    } else if (strncmp(line_copy, "iframe", 6) == 0) {
        return parse_iframe(line_copy, element);
    }
    
    return 0;
}

// Генерация CSS стилей
void generate_css(FILE *out, VingProgram *program) {
    fprintf(out, "<style>\n");
    
    // Определяем цветовую схему на основе стиля
    const char *bg_color = "#f5f5f5";
    const char *container_bg = "#ffffff";
    const char *text_color = "#333333";
    const char *container_shadow = "0 2px 10px rgba(0,0,0,0.1)";
    
    if (strlen(program->page_style) > 0) {
        if (strcmp(program->page_style, "black") == 0 || strcmp(program->page_style, "dark") == 0) {
            bg_color = "#1a1a1a";
            container_bg = "#2d2d2d";
            text_color = "#e0e0e0";
            container_shadow = "0 2px 10px rgba(0,0,0,0.5)";
        } else if (strcmp(program->page_style, "light") == 0 || strcmp(program->page_style, "white") == 0) {
            bg_color = "#ffffff";
            container_bg = "#f9f9f9";
            text_color = "#000000";
            container_shadow = "0 2px 10px rgba(0,0,0,0.05)";
        }
    }
    
    // Базовые стили для контейнера
    fprintf(out, "  body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background-color: %s; color: %s; }\n", bg_color, text_color);
    fprintf(out, "  .container { max-width: 1200px; margin: 0 auto; background-color: %s; padding: 30px; border-radius: 10px; box-shadow: %s; }\n", container_bg, container_shadow);
    
    // Базовые стили для элементов
    fprintf(out, "  .ving-button { padding: 10px 20px; background-color: #007bff; color: white; border: none; border-radius: 5px; cursor: pointer; font-size: 16px; margin: 10px 5px; transition: background-color 0.3s; text-decoration: none; display: inline-block; }\n");
    fprintf(out, "  .ving-button:hover { background-color: #0056b3; }\n");
    fprintf(out, "  a .ving-button { text-decoration: none; }\n");
    fprintf(out, "  a { text-decoration: none; }\n");
    fprintf(out, "  .ving-text { font-size: 16px; line-height: 1.5; margin: 15px 0; color: %s; }\n", text_color);
    fprintf(out, "  .ving-img { max-width: 100%%; height: auto; display: block; margin: 20px 0; border-radius: 5px; }\n");
    fprintf(out, "  .ving-video { max-width: 100%%; height: auto; display: block; margin: 20px 0; border-radius: 5px; }\n");
    fprintf(out, "  .ving-audio { width: 100%%; max-width: 500px; margin: 20px 0; display: block; }\n");
    fprintf(out, "  .ving-iframe { width: 100%%; max-width: 800px; height: 450px; border: none; border-radius: 5px; margin: 20px 0; display: block; }\n");
    
    // Стили для параметров
    fprintf(out, "  .bold { font-weight: bold; }\n");
    fprintf(out, "  .italic { font-style: italic; }\n");
    fprintf(out, "  .big { font-size: 24px; }\n");
    fprintf(out, "  .img-big { width: 100%%; max-width: 800px; }\n");
    
    // Пользовательские стили (st1, st2 и т.д.)
    fprintf(out, "  .st1 { background-color: #28a745 !important; }\n");
    fprintf(out, "  .st1:hover { background-color: #218838 !important; }\n");
    
    // Пользовательские стили
    for (int i = 0; i < program->global_style_count; i++) {
        fprintf(out, "  .%s { %s }\n", program->global_styles[i].name, program->global_styles[i].value);
    }
    
    fprintf(out, "</style>\n");
}

// Генерация HTML элемента
void generate_html_element(FILE *out, VingElement *element) {
    switch (element->type) {
        case ELEMENT_BUTTON: {
            // Проверяем, есть ли ссылка
            int has_link = 0;
            char link_url[512] = "";
            char button_text[1024] = "Button";
            char button_class[256] = "ving-button";
            
            // Проверяем параметры на наличие ссылки и собираем стили
            for (int i = 0; i < element->param_count; i++) {
                if (strcmp(element->params[i], "LINK_FLAG") == 0 && i > 0) {
                    has_link = 1;
                    strncpy(link_url, element->params[i-1], 511);
                    link_url[511] = '\0';
                } else if (strcmp(element->params[i], "LINK_FLAG") != 0) {
                    // Это не флаг ссылки, проверяем, не является ли это URL
                    // (URL будет перед LINK_FLAG, так что пропускаем его)
                    int is_url = 0;
                    if (i + 1 < element->param_count && 
                        strcmp(element->params[i + 1], "LINK_FLAG") == 0) {
                        is_url = 1;
                    }
                    if (!is_url) {
                        // Обычный параметр (стиль)
                        strncat(button_class, " ", 255 - strlen(button_class));
                        strncat(button_class, element->params[i], 255 - strlen(button_class));
                    }
                }
            }
            
            // Используем текст из content, если он есть
            if (strlen(element->content) > 0) {
                strncpy(button_text, element->content, 1023);
                button_text[1023] = '\0';
            }
            
            if (has_link) {
                // Кнопка со ссылкой - оборачиваем в <a>
                fprintf(out, "    <a href=\"%s\"><button class=\"%s\">%s</button></a>\n", 
                        link_url, button_class, button_text);
            } else {
                // Обычная кнопка
                fprintf(out, "    <button class=\"%s\">%s</button>\n", button_class, button_text);
            }
            break;
        }
        case ELEMENT_TEXT: {
            fprintf(out, "    <p class=\"ving-text");
            for (int i = 0; i < element->param_count; i++) {
                fprintf(out, " %s", element->params[i]);
            }
            fprintf(out, "\">%s</p>\n", element->content);
            break;
        }
        case ELEMENT_IMG: {
            fprintf(out, "    <img class=\"ving-img");
            if (element->param_count > 0) {
                if (strcmp(element->params[0], "big") == 0) {
                    fprintf(out, " img-big");
                } else {
                    fprintf(out, " %s", element->params[0]);
                }
            }
            fprintf(out, "\" src=\"%s\" alt=\"Image\">\n", element->content);
            break;
        }
        case ELEMENT_VIDEO: {
            fprintf(out, "    <video class=\"ving-video\"");
            
            // Обрабатываем параметры
            int has_controls = 0;
            int has_autoplay = 0;
            int has_loop = 0;
            int has_muted = 0;
            char width[64] = "";
            char height[64] = "";
            
            for (int i = 0; i < element->param_count; i++) {
                if (strncmp(element->params[i], "controls:", 9) == 0) {
                    if (strcmp(element->params[i] + 9, "true") == 0) {
                        has_controls = 1;
                    }
                } else if (strncmp(element->params[i], "controls", 8) == 0) {
                    has_controls = 1;
                } else if (strncmp(element->params[i], "autoplay:", 9) == 0) {
                    if (strcmp(element->params[i] + 9, "true") == 0) {
                        has_autoplay = 1;
                    }
                } else if (strncmp(element->params[i], "autoplay", 8) == 0) {
                    has_autoplay = 1;
                } else if (strncmp(element->params[i], "loop:", 5) == 0) {
                    if (strcmp(element->params[i] + 5, "true") == 0) {
                        has_loop = 1;
                    }
                } else if (strncmp(element->params[i], "loop", 4) == 0) {
                    has_loop = 1;
                } else if (strncmp(element->params[i], "muted:", 6) == 0) {
                    if (strcmp(element->params[i] + 6, "true") == 0) {
                        has_muted = 1;
                    }
                } else if (strncmp(element->params[i], "muted", 5) == 0) {
                    has_muted = 1;
                } else if (strncmp(element->params[i], "width:", 6) == 0) {
                    strncpy(width, element->params[i] + 6, 63);
                    width[63] = '\0';
                } else if (strncmp(element->params[i], "height:", 7) == 0) {
                    strncpy(height, element->params[i] + 7, 63);
                    height[63] = '\0';
                }
            }
            
            if (has_controls) fprintf(out, " controls");
            if (has_autoplay) fprintf(out, " autoplay");
            if (has_loop) fprintf(out, " loop");
            if (has_muted) fprintf(out, " muted");
            if (strlen(width) > 0) fprintf(out, " width=\"%s\"", width);
            if (strlen(height) > 0) fprintf(out, " height=\"%s\"", height);
            
            fprintf(out, ">\n");
            fprintf(out, "      <source src=\"%s\" type=\"video/mp4\">\n", element->content);
            fprintf(out, "      Your browser does not support video.\n");
            fprintf(out, "    </video>\n");
            break;
        }
        case ELEMENT_AUDIO: {
            fprintf(out, "    <audio class=\"ving-audio\"");
            
            // Обрабатываем параметры
            int has_controls = 0;
            int has_autoplay = 0;
            int has_loop = 0;
            int has_muted = 0;
            
            for (int i = 0; i < element->param_count; i++) {
                if (strncmp(element->params[i], "controls:", 9) == 0) {
                    if (strcmp(element->params[i] + 9, "true") == 0) {
                        has_controls = 1;
                    }
                } else if (strncmp(element->params[i], "controls", 8) == 0) {
                    has_controls = 1;
                } else if (strncmp(element->params[i], "autoplay:", 9) == 0) {
                    if (strcmp(element->params[i] + 9, "true") == 0) {
                        has_autoplay = 1;
                    }
                } else if (strncmp(element->params[i], "autoplay", 8) == 0) {
                    has_autoplay = 1;
                } else if (strncmp(element->params[i], "loop:", 5) == 0) {
                    if (strcmp(element->params[i] + 5, "true") == 0) {
                        has_loop = 1;
                    }
                } else if (strncmp(element->params[i], "loop", 4) == 0) {
                    has_loop = 1;
                } else if (strncmp(element->params[i], "muted:", 6) == 0) {
                    if (strcmp(element->params[i] + 6, "true") == 0) {
                        has_muted = 1;
                    }
                } else if (strncmp(element->params[i], "muted", 5) == 0) {
                    has_muted = 1;
                }
            }
            
            if (has_controls) fprintf(out, " controls");
            if (has_autoplay) fprintf(out, " autoplay");
            if (has_loop) fprintf(out, " loop");
            if (has_muted) fprintf(out, " muted");
            
            fprintf(out, ">\n");
            fprintf(out, "      <source src=\"%s\" type=\"audio/mpeg\">\n", element->content);
            fprintf(out, "      Your browser does not support audio.\n");
            fprintf(out, "    </audio>\n");
            break;
        }
        case ELEMENT_IFRAME: {
            fprintf(out, "    <iframe class=\"ving-iframe\"");
            
            // Обрабатываем параметры
            char width[64] = "";
            char height[64] = "";
            int has_allowfullscreen = 0;
            
            for (int i = 0; i < element->param_count; i++) {
                if (strncmp(element->params[i], "width:", 6) == 0) {
                    strncpy(width, element->params[i] + 6, 63);
                    width[63] = '\0';
                } else if (strncmp(element->params[i], "height:", 7) == 0) {
                    strncpy(height, element->params[i] + 7, 63);
                    height[63] = '\0';
                } else if (strncmp(element->params[i], "allowfullscreen:", 16) == 0) {
                    if (strcmp(element->params[i] + 16, "true") == 0) {
                        has_allowfullscreen = 1;
                    }
                } else if (strncmp(element->params[i], "allowfullscreen", 15) == 0) {
                    has_allowfullscreen = 1;
                }
            }
            
            if (strlen(width) > 0) fprintf(out, " width=\"%s\"", width);
            if (strlen(height) > 0) fprintf(out, " height=\"%s\"", height);
            if (has_allowfullscreen) fprintf(out, " allowfullscreen");
            
            fprintf(out, " src=\"%s\"></iframe>\n", element->content);
            break;
        }
        default:
            break;
    }
}

// Генерация полного HTML документа
void generate_html(FILE *out, VingProgram *program) {
    fprintf(out, "<!DOCTYPE html>\n");
    fprintf(out, "<html lang=\"en\">\n");
    fprintf(out, "<head>\n");
    fprintf(out, "  <meta charset=\"UTF-8\">\n");
    fprintf(out, "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
    
    // Используем title из программы или значение по умолчанию
    if (strlen(program->page_title) > 0) {
        fprintf(out, "  <title>%s</title>\n", program->page_title);
    } else {
        fprintf(out, "  <title>Ving Application</title>\n");
    }
    
    generate_css(out, program);
    fprintf(out, "</head>\n");
    fprintf(out, "<body>\n");
    fprintf(out, "  <div class=\"container\">\n");
    
    for (int i = 0; i < program->element_count; i++) {
        generate_html_element(out, &program->elements[i]);
    }
    
    fprintf(out, "  </div>\n");
    fprintf(out, "</body>\n");
    fprintf(out, "</html>\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input.ving> [output.html]\n", argv[0]);
        return 1;
    }
    
    FILE *input = fopen(argv[1], "r");
    if (!input) {
        printf("Error: failed to open file %s\n", argv[1]);
        return 1;
    }
    
    VingProgram program;
    program.element_count = 0;
    program.global_style_count = 0;
    program.page_title[0] = '\0';
    program.page_style[0] = '\0';
    
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, input)) {
        // Сначала обрабатываем директивы (style:, title:)
        if (parse_directive(line, &program)) {
            continue;
        }
        
        // Затем обрабатываем элементы
        VingElement element;
        if (parse_ving_line(line, &element)) {
            if (program.element_count < MAX_ELEMENTS) {
                program.elements[program.element_count] = element;
                program.element_count++;
            }
        }
    }
    
    fclose(input);
    
    const char *output_file = (argc >= 3) ? argv[2] : "output.html";
    FILE *output = fopen(output_file, "w");
    if (!output) {
        printf("Error: failed to create file %s\n", output_file);
        return 1;
    }
    
    generate_html(output, &program);
    fclose(output);
    
    printf("Compilation completed successfully! Created file: %s\n", output_file);
    return 0;
}

