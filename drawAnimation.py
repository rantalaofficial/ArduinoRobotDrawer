import pygame
import math
import pyperclip

GRID_SIZE = 34
WINDOW_SIZE = 800
CELL_SIZE = WINDOW_SIZE // GRID_SIZE
RESET_BUTTON_SIZE = 100
FONT_SIZE = 30

pygame.init()

window = pygame.display.set_mode((WINDOW_SIZE, WINDOW_SIZE))
pygame.display.set_caption("Pixel Drawing App")

grid = [[False for _ in range(GRID_SIZE)] for _ in range(GRID_SIZE)]

font = pygame.font.Font(None, FONT_SIZE)

shapeX = []
shapeY = []

def draw_grid():
    for y in range(GRID_SIZE):
        for x in range(GRID_SIZE):
            color = (255, 0, 0) if grid[y][x] else (0, 0, 0)
            distance = math.sqrt((GRID_SIZE - 1 - x) ** 2 + (GRID_SIZE - 1 - y) ** 2)
            if distance > 34:
                color = pygame.Color("gray")
            pygame.draw.rect(window, color, (x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE))

def draw_reset_button():
    button_rect = pygame.Rect(0, 0, RESET_BUTTON_SIZE, RESET_BUTTON_SIZE)
    pygame.draw.rect(window, (255, 0, 0), button_rect)

    reset_text = font.render("RESET", True, (255, 255, 255))
    text_rect = reset_text.get_rect(center=button_rect.center)
    window.blit(reset_text, text_rect)

def getShapeCode(arrayName, array):
    return "int " + arrayName + "[200] = {" + ",".join(str(num) for num in array) + "};"

def reset_grid():
    global shapeX
    global shapeY

    for y in range(GRID_SIZE):
        for x in range(GRID_SIZE):
            grid[y][x] = False

    shapeX.append(-1)
    shapeY.append(-1)

    print("Shape size: " + str(len(shapeX)) + "\n")
    print("Copied to clipboard:\n")
    codeX = getShapeCode("shapeArrayX", shapeX)
    codeY = getShapeCode("shapeArrayY", shapeY)
    print(codeX)
    print(codeY)
    pyperclip.copy(codeX + "\n" + codeY)

    shapeX = []
    shapeY = []

def is_reset_button_clicked(pos):
    button_x, button_y, button_width, button_height = 0, 0, RESET_BUTTON_SIZE, RESET_BUTTON_SIZE
    return button_x <= pos[0] <= button_x + button_width and button_y <= pos[1] <= button_y + button_height

running = True
drawing = False
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.MOUSEBUTTONDOWN:
            if event.button == 1:
                if is_reset_button_clicked(event.pos):
                    reset_grid()
                else:
                    drawing = True
        elif event.type == pygame.MOUSEBUTTONUP:
            drawing = False

    if drawing:
        mouse_x, mouse_y = pygame.mouse.get_pos()
        cell_x = mouse_x // CELL_SIZE
        cell_y = mouse_y // CELL_SIZE
        if 0 <= cell_x < GRID_SIZE and 0 <= cell_y < GRID_SIZE:
            if not grid[cell_y][cell_x]:
                grid[cell_y][cell_x] = True
                shapeX.insert(0, cell_x)
                shapeY.insert(0, cell_y)

    window.fill((0, 0, 0))
    draw_grid()
    draw_reset_button()

    pygame.display.flip()

pygame.quit()
