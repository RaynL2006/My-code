from tkinter import CENTER
import pygame
import random
import math
from pygame.locals import(
    RLEACCEL,
    K_ESCAPE,
    KEYDOWN,
    QUIT,
)

SCREEN_WIDTH = 1275
SCREEN_HEIGHT = 650

class Player(pygame.sprite.Sprite):
    def __init__(self):
        super(Player, self).__init__()
        self.surf = pygame.image.load(r'C:\Users\RaynL\OneDrive\Pictures\Saved Pictures\jet.png').convert()
        self.surf_clean = self.surf.copy()
        self.rect = self.surf_clean.get_rect()

    def fly(self, px, py, mox, moy, speed):
        dx = abs(mox-px)
        dy = abs(moy-py)
        if dx > 0:
            if dy > 0: 
                hyp = math.hypot(dy, dx)
                cosAngle = (dx**2 + hyp**2 - dy**2) / (2*dx*hyp)
                AngleR = math.acos(cosAngle)
                AngleD = math.degrees(AngleR)
                sinAngle = math.sin(AngleR)
                ndy = speed * sinAngle
                ndx = speed * cosAngle
                if mox < px:
                    ndx = ndx * (-1)
                if moy < py:
                    ndy = ndy * (-1)
                
                if mox < px and moy < py:
                    self.surf = pygame.transform.rotate(self.surf_clean, 180 - AngleD)
                    self.rect = self.surf.get_rect(center = self.rect.center)
                elif mox < px and moy > py:
                    self.surf = pygame.transform.rotate(self.surf_clean, 180 + AngleD)
                    self.rect = self.surf.get_rect(center = self.rect.center)
                elif mox > px and moy > py:
                    self.surf = pygame.transform.rotate(self.surf_clean, -AngleD)
                    self.rect = self.surf.get_rect(center = self.rect.center)
                elif mox > px and moy < py:
                    self.surf = pygame.transform.rotate(self.surf_clean, AngleD)
                    self.rect = self.surf.get_rect(center = self.rect.center)
                
                self.surf.set_colorkey((255, 255, 255), RLEACCEL)
                self.rect.move_ip(ndx, ndy)

                if self.rect.left < 0:
                    self.rect.left = SCREEN_WIDTH - self.rect.width
                if self.rect.right > SCREEN_WIDTH:
                    self.rect.right = self.rect.width
                if self.rect.top < 0:
                    self.rect.top = SCREEN_HEIGHT - self.rect.height
                if self.rect.bottom > SCREEN_HEIGHT:
                    self.rect.bottom = self.rect.height
                
        
class Enemy(pygame.sprite.Sprite):
    def __init__(self):
        super(Enemy, self).__init__()
        self.surf = pygame.image.load(r'C:\Users\RaynL\OneDrive\Pictures\Saved Pictures\missile.png').convert()
        self.surf_clean = self.surf.copy()
        self.rect = self.surf_clean.get_rect(
            center=(
                random.randint(0, SCREEN_WIDTH),
                random.randint(SCREEN_HEIGHT + 20, SCREEN_HEIGHT + 100),
            )
        )
        self.speed = random.randint(5, 10)
        self.frames_since_birth = 0

    def update(self, px, py):
        self.frames_since_birth = self.frames_since_birth + 1
        self.secs_since_birth = self.frames_since_birth / FPS
        if self.secs_since_birth == e_lifetime:
            self.kill()

        ex, ey = self.rect.center
        dx = abs(px-ex)
        dy = abs(py-ey)
        if dx > 0:
            if dy > 0:
                speed = self.speed 
                hyp = math.hypot(dy, dx)
                cosAngle = (dx**2 + hyp**2 - dy**2) / (2*dx*hyp)
                AngleR = math.acos(cosAngle)
                AngleD = math.degrees(AngleR)
                sinAngle = math.sin(AngleR)
                ndy = speed * sinAngle
                ndx = speed * cosAngle
                if px < ex:
                    ndx = ndx * (-1)
                if py < ey:
                    ndy = ndy * (-1)
                
                if px < ex and py < ey:
                    self.surf = pygame.transform.rotate(self.surf_clean, 180 - AngleD)
                    self.rect = self.surf.get_rect(center = self.rect.center)
                elif px < ex and py > ey:
                    self.surf = pygame.transform.rotate(self.surf_clean, 180 + AngleD)
                    self.rect = self.surf.get_rect(center = self.rect.center)
                elif px > ex and py > ey:
                    self.surf = pygame.transform.rotate(self.surf_clean, -AngleD)
                    self.rect = self.surf.get_rect(center = self.rect.center)
                elif px > ex and py < ey:
                    self.surf = pygame.transform.rotate(self.surf_clean, AngleD)
                    self.rect = self.surf.get_rect(center = self.rect.center)
                
                self.surf.set_colorkey((255, 255, 255), RLEACCEL)
                self.rect.move_ip(ndx, ndy)
        


pygame.mixer.init()
pygame.init()

FPS = 30
seconds_passed = 0

e_lifetime = 10

clock = pygame.time.Clock()

screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))

pygame.display.set_caption('Plane game')
font = pygame.font.Font('freesansbold.ttf', 14)


ADDENEMY = pygame.USEREVENT + 1
pygame.time.set_timer(ADDENEMY, 1500)

player = Player()

enemies = pygame.sprite.Group()
other_enemies = pygame.sprite.Group()
all_sprites = pygame.sprite.Group()
all_sprites.add(player)

pygame.mixer.music.load(r'C:\Users\RaynL\OneDrive\Desktop\Python\Game_music\background_music.ogg')
pygame.mixer.music.play(loops=-1)

missile_crash_sound = pygame.mixer.Sound(r'C:\Users\RaynL\OneDrive\Desktop\Python\Game_music\missile_on_missile.wav')
missile_crash_sound.set_volume(0.5)
plane_crash_sound = pygame.mixer.Sound(r'C:\Users\RaynL\OneDrive\Desktop\Python\Game_music\plane_explosion.wav')
plane_crash_sound.set_volume(0.75)

running = True

while running:
    mox, moy = pygame.mouse.get_pos()
    px, py = player.rect.center
    
    text = font.render('Score: %d' % (seconds_passed), True, (255, 255, 255))
    textRect = text.get_rect()
    textRect.center = (100, 50)
    

    for event in pygame.event.get():
        
        if event.type == KEYDOWN:
            
            if event.key == K_ESCAPE:
                running = False
        
        elif event.type == QUIT:
            running = False

        elif event.type == ADDENEMY:
            new_enemy = Enemy()
            enemies.add(new_enemy)
            other_enemies.add(new_enemy)
            all_sprites.add(new_enemy)

    player.fly(px, py, mox, moy, 10)
    enemies.update(px, py)

    screen.fill((135, 206, 250))

    for entity in all_sprites:
        screen.blit(entity.surf, entity.rect)
    screen.blit(text, textRect)
    
    if pygame.sprite.spritecollideany(player, enemies):
        player.kill()
        plane_crash_sound.play()
        end_font = pygame.font.Font('freesansbold.ttf', 32)
        end_message = end_font.render('You got shot down! Your score is: %d' % (seconds_passed), True, (255, 255, 255))
        end_messageRect = end_message.get_rect()
        end_messageRect.center = (SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2)
        screen.blit(end_message, end_messageRect)
        pygame.display.flip()
        pygame.time.wait(3500)
        running = False
    
    for entity in enemies:
        other_enemies.remove(entity)
        if pygame.sprite.spritecollide(entity, other_enemies, False):
            collided_missiles = pygame.sprite.spritecollide(entity, other_enemies, False)
            entity.kill()
            missile_crash_sound.play()
            for missile in collided_missiles:
                missile.kill()
                missile_crash_sound.play()
        else: other_enemies.add(entity)

    # Update the display
    pygame.display.flip()

    seconds_passed = round(pygame.time.get_ticks() / 1000, 0)
  
    clock.tick(FPS)

pygame.mixer.music.stop()
pygame.mixer.quit()