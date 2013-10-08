
/* ---------------------------------------------------------------- */
/* Includes */
/* ---------------------------------------------------------------- */

/* ***** Standard C ***** */

#include <stdlib.h>
#include <stdio.h>

#ifndef SIZE_320_200
#define SIZE_640_480
#endif
#include "sizes.h"

#include "GFX_SDL.c"
#include "IO_SDL.c"
#include "LIB_SDL.c"
#include "SFX_SDL_mixer.c"
#include "MUS_SDL_mixer.c"

/* ---------------------------------------------------------------- */
/* Definitions */
/* ---------------------------------------------------------------- */

#undef TRUE

#undef FALSE

#ifndef NULL
#define NULL 0
#endif

#define XLode_debug(x) fprintf(stderr, x)
#define XLode_debug1(x,x1) fprintf(stderr, x,x1)
#define XLode_debug2(x,x1,x2) fprintf(stderr, x,x1,x2)
#define XLode_debug3(x,x1,x2,x3) fprintf(stderr, x,x1,x2,x3)
#define XLode_debug4(x,x1,x2,x3,x4) fprintf(stderr, x,x1,x2,x3,x4)

// #define randomNumber(x) ((int)(x * rand()/(RAND_MAX+1.0)))
#define randomNumber(x) (rand() % x)

/* ***** Game parameters ***** */

/**
 * Limitations
 */

#define Ci_arenaW 40
#define Ci_arenaH 20

#define Ci_stageCount 4

#define Ci_figureCount 11

#define Ci_monkCount 10

/**
 * Sound channels
 */

#define Ci_lodeChannel 0
#define Ci_brickChannel 1
#define Ci_treasureChannel 2
#define Ci_messageChannel 3

/**
 * Motion properties of a game element
 */

#define FROM_SIDES  1
#define FROM_UP     2
#define FROM_DOWN   4
#define TO_SIDES    8
#define TO_UP       16
#define TO_DOWN     32
#define FALL_FROM   64
#define FALL_TO     128

/**
 * Amount of time that the player can't move while breaking a brick
 */


#define Ci_breakingTime 5

/**
 * Number of songs used in the game
 */

#define Ci_songCount 5

/* --------------------------------------------------------------- */
/* Data structures */
/* --------------------------------------------------------------- */

/* ***** Enumerations ***** */

typedef enum {
   LODE = 0,
   MONK,
   BRICK,
   ROCK,
   GHOSTBRICK,
   EMPTY,
   STAIRS,
   TREASURE,
   ROPE,
   BROKENBRICK,
   HIDDENSTAIRS
} ElementType;

typedef enum {
   GET_TREASURE = 0,
   BREAK_BRICK,
   REACH_SKY,
   READY,
   GO,
   DEATH,
   YESSS
} SampleType;
#define Ci_sampleCount 7

typedef enum {
   NONE,
   UP,
   DOWN,
   LEFT,
   RIGHT
} Direction;

typedef enum {
   FALSE = 0,
   TRUE
} Boolean;

/* ***** Records ***** */

typedef struct {
   ElementType e_type;
   int i_moveSet;
   int i_status;
} Element;

typedef struct {
   int i_x;
   int i_y;
   int i_offsetX;
   int i_offsetY;
   Direction e_direction;
   Direction e_nextDirection;
   Direction e_lastDirection;
   Boolean b_falling;
   GFX_Image** papr_runningLeft;
   GFX_Image** papr_runningRight;
   GFX_Image* pr_defaultFigure;
   GFX_Image* pr_defaultFalling;
   int i_respawnX;
   int i_respawnY;
   Boolean b_isLode;
   Boolean b_locked; /* Monk is locked in a broken brick */
} Player;

typedef struct {
   int i_x;
   int i_y;
   int i_timer;
   int i_step;
} BrickEvent;

/* --------------------------------------------------------------- */
/* Game data */
/* --------------------------------------------------------------- */

/**
 * Generic game data
 */

Element aar_arena[Ci_arenaW][Ci_arenaH];

char aac_mapCache[Ci_stageCount+1][Ci_arenaW][Ci_arenaH];

Boolean b_retry;

Boolean b_gameOver;

int i_stage;

int i_score;

int i_lives;

Boolean ab_cachedStage[Ci_stageCount+1];

Player r_lode;

int i_monkCount;

Player ar_monks[Ci_monkCount];

int i_treasures;

int i_gameTimer;

Boolean b_playing;

Boolean b_victory;

int i_breaking;

Direction e_scheduledBreak;

Direction e_lastBreak;

/**
 * Broken bricks
 */

int i_brokenCount;

int i_brokenHead, i_brokenTail;

BrickEvent ar_broken[Ci_arenaW * Ci_arenaH];

/**
 * Frames for the "breaking bricks" animation
 */

// Concepts:
// The animation contains eight frames, called "steps", but only 5
// different images are used; hence the difference between the
// steps of the animation and the frames that are actually used.

#define Ci_brokenSteps 8
int ai_brokenTimes[Ci_brokenSteps]  = { 2, 4, 6, 8, 120, 130, 140, 150 };
int ai_brokenFrames[Ci_brokenSteps] = { 1, 2, 3, 0,   3,   2,   1,   4 };

/**
 * Monk logic
 */

Direction ae_directions[4] = {UP, DOWN, LEFT, RIGHT};

/** ***** Graphics ***** */

/**
 * Numbers
 */

GFX_Image* apr_numbers[10];

/**
 * Lode animations
 */

GFX_Image* apr_lodeRunningRight[8];

GFX_Image* apr_lodeRunningLeft[8];

GFX_Image* pr_lodeFalling;

/**
 * Monk animations
 */

GFX_Image* apr_monkRunningRight[8];

GFX_Image* apr_monkRunningLeft[8];

GFX_Image* pr_monkFalling;

/**
 * Broken bricks
 */

#define Ci_brokenFrames 5
GFX_Image* apr_brokenAnimation[Ci_brokenFrames];

GFX_Image* pr_lodeBreakingLeft;
GFX_Image* pr_lodeBreakingRight;

/**
 * Generic game graphics
 */

GFX_Image* apr_figures[Ci_figureCount];

GFX_Image* pr_backBuffer;

GFX_Image* pr_arenaBuffer;

/**
 * Graphics for the opening
 */

GFX_Image* pr_credits;
GFX_Image* pr_author;
GFX_Image* pr_title;
GFX_Image* pr_options;
GFX_Image* pr_lodeIcon;

GFX_Image* pr_background;

GFX_Image* pr_gameOver;
GFX_Image* pr_gameOverFlip;

/* ***** Sounds ***** */

int i_lodeSoundX;

int i_lodeSoundY;

SFX_Sample* pr_lodeVoice;

Boolean b_wasFalling;

MUS_Song* apr_songs[Ci_songCount];

SFX_Sample* apr_samples[Ci_sampleCount];

Boolean b_halfTime = FALSE;

/**
 * Lode on the rope
 */

#define Ci_ropeSampleCount 8
SFX_Sample* apr_ropeSamples[Ci_ropeSampleCount];

/**
 * Lode falling
 */

#define Ci_fallSampleCount 5
SFX_Sample* apr_fallSamples[Ci_fallSampleCount];

/**
 * End of a fall ("ouch")
 */

#define Ci_ouchSampleCount 3
SFX_Sample* apr_ouchSamples[Ci_ouchSampleCount];

/**
 * Lode on the stairs
 */

#define Ci_stairsSampleCount 4
SFX_Sample* apr_stairsSamples[Ci_stairsSampleCount];

/**
 * Lode running
 */

#define Ci_runningSampleCount 3
SFX_Sample* apr_runningSamples[Ci_runningSampleCount];

/* ---------------------------------------------------------------- */
/* Game specific routines */
/* ---------------------------------------------------------------- */

/* ***** Animation ***** */

/*
void Animation_play(char* sz_fileName_, JGMOD* pr_module) {
   GFX_Image* pr_centered;

   clear(screen);
   Allegro_setVideo(8);
   pr_centered = create_sub_bitmap(screen, Ci_resX/2-(320/2),Ci_resY/2-(200/2), 320, 200);
   set_mod_volume(250);
   play_mod(pr_module, FALSE);
   play_fli(sz_fileName_, pr_centered, 0, NULL);
   destroy_bitmap(pr_centered);
   Allegro_setVideo(16);
}
*/

/*
void Animation_play(char* sz_fileName_, JGMOD* pr_module) {
   GFX_Image* pr_centered;

   set_color_conversion(COLORCONV_EXPAND_256);

   clear(screen);
   pr_centered = create_sub_bitmap(screen, Ci_resX/2-(320/2),Ci_resY/2-(200/2), 320, 200);
   set_mod_volume(250);
   play_mod(pr_module, FALSE);
   open_fli(sz_fileName_);
   while (next_fli_frame(0) == FLI_OK) {
      set_palette(fli_palette);
      rest(30);
      GFX_Blit(fli_bitmap, pr_centered, 0, 0, 0, 0, 320, 200);
   }
   close_fli();
   destroy_bitmap(pr_centered);
}
*/

/* ***** Figures ***** */

/**
 * Draws the figure that's at the indicated position
 * directly at the back buffer.
 */

void Figure_draw(int i_x_, int i_y_) {
   Element* pr_this = &aar_arena[i_x_][i_y_];
   GFX_Blit(pr_background, pr_backBuffer,
            i_x_ * Ci_spriteW, i_y_ * Ci_spriteH,
            i_x_ * Ci_spriteW, i_y_ * Ci_spriteH,
            Ci_spriteW, Ci_spriteH);
   if (apr_figures[pr_this->e_type] != NULL) {
      GFX_Draw(pr_backBuffer, apr_figures[pr_this->e_type],
               i_x_ * Ci_spriteW, i_y_ * Ci_spriteH);
   }
}

/**
 * Draws the figure that's at the indicated position,
 * obtaining it from the arena's cache.
 */

void Figure_show(int i_x_, int i_y_) {
   GFX_Blit(pr_arenaBuffer, pr_backBuffer, i_x_ * Ci_spriteW, i_y_ * Ci_spriteH, i_x_ * Ci_spriteW, i_y_ * Ci_spriteH, Ci_spriteW, Ci_spriteH);
}

/**
 * Updates the arena's cache putting the given sprite at
 * the specified position.
 */

void Figure_update(int i_x_, int i_y_, GFX_Image* pr_sprite_) {
   Element* pr_this = &aar_arena[i_x_][i_y_];
   GFX_Blit(pr_background, pr_arenaBuffer, i_x_ * Ci_spriteW, i_y_ * Ci_spriteH, i_x_ * Ci_spriteW, i_y_ * Ci_spriteH, Ci_spriteW, Ci_spriteH);
   if (pr_sprite_ != NULL) {
      GFX_Draw(pr_arenaBuffer, pr_sprite_, i_x_ * Ci_spriteW, i_y_ * Ci_spriteH);
   }
}

void Number_displayAt(GFX_Image* pr_target, int i_x_, int i_y_, int i_number_, int i_digits_) {
   char sz_written[20];
   int i_length;
   int i_pad;
   int i;

   i_pad = 0;
   sprintf(sz_written, "%d", i_number_);
   i_length = strlen(sz_written);
   if (i_length < i_digits_) {
      for(; i_pad < i_digits_ - i_length; i_pad++) {
         GFX_Draw(pr_target, apr_numbers[0], i_x_+(i_pad*Ci_numberWidth), i_y_);
      }
   }
   for(i = 0; i < i_length; i++) {
      GFX_Draw(pr_target, apr_numbers[sz_written[i]-48], i_x_+(i_pad*Ci_numberWidth), i_y_);
      i_pad++;
   }
}

/* ***** Sounds ***** */

void Sound_play(SampleType e_id_, int i_xPosition_, int i_channel_) {
   SFX_Play(apr_samples[e_id_], 255, 255 * ((double)i_xPosition_ / (double)Ci_arenaW), i_channel_, FALSE);
}

/**
 * Play a sound base on Lode's current position
 */

void Sound_lodeSound() {
   int i_height;
   int i_yCount;
   int i_fall;
   int i_panning;
   SFX_Sample* pr_newLodeVoice;

   pr_newLodeVoice = NULL;

   i_panning = 255 * ((double)r_lode.i_x / (double)Ci_arenaW);

   if (i_lodeSoundX == r_lode.i_x && i_lodeSoundY == r_lode.i_y)
      return;

   i_lodeSoundX = r_lode.i_x;
   i_lodeSoundY = r_lode.i_y;

   if (aar_arena[r_lode.i_x][r_lode.i_y].e_type == ROPE) {
      if (b_halfTime = ~b_halfTime)
         pr_newLodeVoice = apr_ropeSamples[randomNumber(Ci_ropeSampleCount)];
      else
         pr_newLodeVoice = NULL;
   } else if (aar_arena[r_lode.i_x][r_lode.i_y].e_type == STAIRS) {
      if (b_halfTime = ~b_halfTime)
         pr_newLodeVoice = apr_stairsSamples[randomNumber(Ci_stairsSampleCount)];
      else
         pr_newLodeVoice = NULL;
   } else if (r_lode.b_falling && b_wasFalling) {
      // Check if done falling
      i_yCount = r_lode.i_y + 1;
      // TODO: this entire function sucks at 04:16am
      if (!(i_yCount <= Ci_arenaH && aar_arena[r_lode.i_x][i_yCount].i_moveSet & FALL_TO
          && i_yCount <= Ci_arenaH && aar_arena[r_lode.i_x][i_yCount].i_moveSet & FALL_FROM)) {
         pr_newLodeVoice = apr_ouchSamples[randomNumber(Ci_ouchSampleCount)];
         b_wasFalling = FALSE;
      }
   } else if (r_lode.b_falling && !b_wasFalling) {
      // Scream
      i_height = 0;
      i_yCount = r_lode.i_y;
      // TODO: fix this; it's 3:42am and I don't want to think about it
      while (i_yCount <= Ci_arenaH && aar_arena[r_lode.i_x][i_yCount].i_moveSet & FALL_TO
          && i_yCount <= Ci_arenaH && aar_arena[r_lode.i_x][i_yCount].i_moveSet & FALL_FROM) {
         i_height++;
         i_yCount++;
      }
      if (i_height <= 2) i_fall = 0;
      else if (i_height <= 5) i_fall = 1;
      else if (i_height <= 8) i_fall = 2;
      else if (i_height <= 11) i_fall = 3;
      else i_fall = 4;
      pr_newLodeVoice = apr_fallSamples[i_fall];
      b_wasFalling = TRUE;
   } else {
      if (b_halfTime = ~b_halfTime)
         pr_newLodeVoice = apr_runningSamples[randomNumber(Ci_runningSampleCount)];
      else
         pr_newLodeVoice = NULL;
   }
   if (pr_newLodeVoice != NULL) {
      SFX_Stop(Ci_lodeChannel);
      SFX_Play(pr_newLodeVoice, 255, i_panning, Ci_lodeChannel, 0);
      pr_lodeVoice = pr_newLodeVoice;
   }
}


/**
 * Check if there's a monk at a given point
 */

Player* Monk_anyAt(int i_x, int i_y) {
   int i;
   for (i = 0; i < i_monkCount; i++) {
      if (ar_monks[i].i_x == i_x && ar_monks[i].i_y == i_y) {
         return &ar_monks[i];
      }
   }
   return NULL;
}

/* ***** Bricks ***** */

/**
 * Break a brick: make the space empty and schedule the bricks reappearance.
 */

void Bricks_break(int i_x_, int i_y_) {
   BrickEvent* pr_brick = &(ar_broken[i_brokenTail]);

   Sound_play(BREAK_BRICK, i_x_, Ci_brickChannel);

   i_brokenCount++;
   pr_brick->i_x = i_x_;
   pr_brick->i_y = i_y_;
   pr_brick->i_timer = i_gameTimer;
   pr_brick->i_step = 0;
   i_brokenTail++;
   aar_arena[i_x_][i_y_].e_type = BROKENBRICK;
   aar_arena[i_x_][i_y_].i_moveSet = FROM_SIDES | FROM_UP | FROM_DOWN
                                   | TO_SIDES             | TO_DOWN
                                   | FALL_FROM  | FALL_TO;
}

void Bricks_update() {
   int i;
   int i_x, i_y;
   Player* pr_monk;

   if (i_brokenCount == 0)
      return;

   for (i = i_brokenHead; i < i_brokenTail; i++) {
      if (ar_broken[i].i_timer + ai_brokenTimes[ar_broken[i].i_step] == i_gameTimer) {
         Figure_update(ar_broken[i].i_x,
                       ar_broken[i].i_y,
                       apr_brokenAnimation[ai_brokenFrames[ar_broken[i].i_step]]);
         Figure_show(ar_broken[i].i_x, ar_broken[i].i_y);
         ar_broken[i].i_step++;
      }
   }

   if (ar_broken[i_brokenHead].i_step == Ci_brokenSteps) {
      i_x = ar_broken[i_brokenHead].i_x;
      i_y = ar_broken[i_brokenHead].i_y;
      aar_arena[i_x][i_y].e_type = BRICK;
      aar_arena[i_x][i_y].i_moveSet = 0;
      i_brokenHead++;
      i_brokenCount--;
      if (i_brokenCount == 0) {
         i_brokenHead = 0;
         i_brokenTail = 0;
      }
      if (r_lode.i_x == i_x && r_lode.i_y == i_y) {
         b_playing = FALSE;
      }
      while (pr_monk = Monk_anyAt(i_x, i_y)) {
         pr_monk->i_x = pr_monk->i_respawnX;
         pr_monk->i_y = pr_monk->i_respawnY;
         pr_monk->b_locked = FALSE;
      }
      if (r_lode.i_x == i_x && r_lode.i_y == i_y) {
         b_playing = FALSE;
      }
   }

}

/* ***** Player ***** */

/**
 * Verifies that the attempted move is valid in the arena.
 */

int Player_isValidMove(Player r_this, Direction e_direction) {

   int i_x, i_lx;
   int i_y, i_ly;

   if (e_direction == NONE)
      return TRUE;

   i_x = r_this.i_x;
   i_lx = i_x;
   i_y = r_this.i_y;
   i_ly = i_y;
   if (e_direction == UP) i_y--;
   if (e_direction == DOWN) i_y++;
   if (e_direction == LEFT) i_x--;
   if (e_direction == RIGHT) i_x++;

   if (!r_this.b_isLode && Monk_anyAt(i_x, i_y))
      return FALSE;

   if (i_x < 0 || i_x > Ci_arenaW - 1 || i_y < 0 || i_y > Ci_arenaH - 1)
      return FALSE;

   if (e_direction == LEFT || e_direction == RIGHT)
      return (aar_arena[i_lx][i_ly].i_moveSet & TO_SIDES
              && aar_arena[i_x][i_y].i_moveSet & FROM_SIDES);
   if (e_direction == UP)
      return (aar_arena[i_lx][i_ly].i_moveSet & TO_UP
              && aar_arena[i_x][i_y].i_moveSet & FROM_DOWN);
   if (e_direction == DOWN)
      return (aar_arena[i_lx][i_ly].i_moveSet & TO_DOWN
              && aar_arena[i_x][i_y].i_moveSet & FROM_UP);

   return FALSE;
}

/**
 * Erase the area surrounding the player, restoring
 * the arena. Notice that other characters (monks)
 * will be erased too.
 */

void Player_clear(Player r_this) {
   Figure_show(r_this.i_x, r_this.i_y);
   if (r_this.i_offsetX > 0)
      Figure_show(r_this.i_x+1, r_this.i_y);
   else if (r_this.i_offsetX < 0)
      Figure_show(r_this.i_x-1, r_this.i_y);
   else if (r_this.i_offsetY > 0)
      Figure_show(r_this.i_x, r_this.i_y+1);
   else if (r_this.i_offsetY < 0)
      Figure_show(r_this.i_x, r_this.i_y-1);
}

/**
 * Draw the player.
 */

void Player_draw(Player r_this) {
   int i_index;
   int i_rope;
   GFX_Image* pr_sprite;
   GFX_Image** apr_runningLeft;
   GFX_Image** apr_runningRight;

   apr_runningLeft = r_this.papr_runningLeft;
   apr_runningRight = r_this.papr_runningRight;

   // Remember that only one of those will be > 0
   i_index =
   ABS( r_this.i_offsetX / Ci_horizSprites + r_this.i_offsetY / Ci_vertSprites );

   if (aar_arena[r_this.i_x][r_this.i_y].e_type == ROPE)
      i_rope = 4;
   else
      i_rope = 0;

   // TODO: stairs
   if (r_this.e_lastDirection == LEFT)
      pr_sprite = apr_runningLeft[i_index + i_rope];
   else if (r_this.e_lastDirection == RIGHT)
      pr_sprite = apr_runningRight[i_index + i_rope];
   else if (r_this.e_lastDirection == UP)
      pr_sprite = r_this.pr_defaultFalling;
   else if (r_this.e_lastDirection == DOWN) {
      if (aar_arena[r_this.i_x][r_this.i_y].i_moveSet & FALL_FROM)
         pr_sprite = r_this.pr_defaultFalling;
      else
         pr_sprite = r_this.pr_defaultFalling;
   }

   if (r_this.b_isLode && i_breaking > 0) {
      if (e_lastBreak == LEFT)
         pr_sprite = pr_lodeBreakingLeft;
      else
         pr_sprite = pr_lodeBreakingRight;
   }

   GFX_Draw(pr_backBuffer,
               pr_sprite,
               Ci_spriteW * r_this.i_x + r_this.i_offsetX,
               Ci_spriteH * r_this.i_y + r_this.i_offsetY);
}

/**
 * Updates the player's position, based on current
 * direction coordinates.
 */

// Concepts:
// Stable - player is exactly in a position in the arena, with no offset.
// Unstable - player is moving between two positions, as indicated by the
// offset.

void Player_move(Player* pr_this) {
   Element r_here, r_under;
   int i_x, i_y;
   i_x = pr_this->i_x;
   i_y = pr_this->i_y;
   r_here = aar_arena[i_x][i_y];
   r_under = aar_arena[i_x][i_y+1];

   // If in a stable place, check if direction is valid and update it.
   if (pr_this->i_offsetX == 0 && pr_this->i_offsetY == 0) {
      if ((!pr_this->b_isLode) && r_here.e_type == BROKENBRICK) {
         // Lock monk in broken brick
         // TODO timer for monk escape
         pr_this->e_direction = NONE;
         pr_this->e_nextDirection = NONE;
      } else if (pr_this->i_y < Ci_arenaH
          && r_here.i_moveSet & FALL_FROM
          && r_under.i_moveSet & FALL_TO
          && !Monk_anyAt(i_x, i_y+1)
         ) {
         // Gravity has the priority :)
         pr_this->e_direction = DOWN;
         pr_this->e_nextDirection = DOWN;
         pr_this->b_falling = TRUE;
      } else {
         if (pr_this->b_falling) {
            pr_this->e_direction = NONE;
            pr_this->e_nextDirection = NONE;
         } else if (Player_isValidMove(*pr_this, pr_this->e_nextDirection)) {
            pr_this->e_direction = pr_this->e_nextDirection;
         } else {
            pr_this->e_direction = NONE;
            pr_this->e_nextDirection = NONE;
         }
         pr_this->b_falling = FALSE;
      }
   } else {
      // If opposite direction, allow changing direction even if unstable
      if (pr_this->e_direction == UP && pr_this->e_nextDirection == DOWN)
         pr_this->e_direction = DOWN;
      if (!pr_this->b_falling && pr_this->e_direction == DOWN && pr_this->e_nextDirection == UP)
         pr_this->e_direction = UP;
      if (pr_this->e_direction == LEFT && pr_this->e_nextDirection == RIGHT)
         pr_this->e_direction = RIGHT;
      if (pr_this->e_direction == RIGHT && pr_this->e_nextDirection == LEFT)
         pr_this->e_direction = LEFT;
   }
   // Move
   if (pr_this->e_direction == RIGHT)
      pr_this->i_offsetX += Ci_deltaX;
   if (pr_this->e_direction == LEFT)
      pr_this->i_offsetX -= Ci_deltaX;
   if (pr_this->e_direction == DOWN)
      pr_this->i_offsetY += Ci_deltaY;
   if (pr_this->e_direction == UP)
      pr_this->i_offsetY -= Ci_deltaY;
   // If full offset is reached, stabilize position
   if (pr_this->i_offsetX == Ci_spriteW) {
      pr_this->i_x++;
      pr_this->i_offsetX = 0;
   }
   if (pr_this->i_offsetX == -Ci_spriteW) {
      pr_this->i_x--;
      pr_this->i_offsetX = 0;
   }
   if (pr_this->i_offsetY == Ci_spriteH) {
      pr_this->i_y++;
      pr_this->i_offsetY = 0;
   }
   if (pr_this->i_offsetY == -Ci_spriteH) {
      pr_this->i_y--;
      pr_this->i_offsetY = 0;
   }
   if (pr_this->e_direction != NONE)
      pr_this->e_lastDirection = pr_this->e_direction;
}

/**
 * Returns TRUE if the player is perfectly centered in a position.
 */

int Player_isStable(Player r_this) {
   return (r_this.i_offsetX == 0 && r_this.i_offsetY == 0);
}

/**
 * Blink all players on the screen.
 */

void Player_blinkAll(int i_times_, int i_onTime_, int i_offTime_) {
   int i;
   int j;
   for (j = 0; j < i_times_; j++) {
      Player_clear(r_lode);
      for (i = 0; i < i_monkCount; i++)
         Player_clear(ar_monks[i]);
      GFX_DisplayBuffer(pr_backBuffer);
      IO_Wait(i_offTime_);
      Player_draw(r_lode);
      for (i = 0; i < i_monkCount; i++)
         Player_draw(ar_monks[i]);
      GFX_DisplayBuffer(pr_backBuffer);
      IO_Wait(i_onTime_);
   }
}

/* ***** Lode specific functions ***** */

/**
 * Attempt to break a brick in a specified direction, which may be
 * LEFT or RIGHT.
 */

void Lode_break(Direction e_direction) {
   int i_x;
   int i_y;

   i_x = r_lode.i_x;
   i_y = r_lode.i_y;

   if (i_y == Ci_arenaH)
      return;
   if (e_direction == LEFT) {
      if (i_x == 0)
         return;
      if (aar_arena[i_x-1][i_y].i_moveSet & TO_SIDES
         && aar_arena[i_x-1][i_y+1].e_type == BRICK
         && !Monk_anyAt(i_x-1, i_y)
      ) {
         Bricks_break(i_x-1, i_y+1);
      }
   } else if (e_direction == RIGHT) {
      if (i_x == Ci_arenaW - 1)
         return;
      if (aar_arena[i_x+1][i_y].i_moveSet & TO_SIDES
         && aar_arena[i_x+1][i_y+1].e_type == BRICK
         && !Monk_anyAt(i_x+1, i_y)
      ) {
         Bricks_break(i_x+1, i_y+1);
      }
   }
}

/* ***** Monk specific functions ***** */

Direction Monk_decideDirection(Player r_monk) {
   int i_xDistance;
   int i_yDistance;
   Direction e_choice;
   Boolean ab_isValid[4];
   int i;
   int i_limit;

   i_xDistance = r_monk.i_x - r_lode.i_x;
   i_yDistance = r_monk.i_y - r_lode.i_y;

   for(i=0; i<4; i++)
      ab_isValid[i] = Player_isValidMove(r_monk, ae_directions[i]);

   // Case 1: Lode is in sight
   if (ab_isValid[0] && i_xDistance == 0 && i_yDistance > 0)
      return UP;
   if (ab_isValid[1] && i_xDistance == 0 && i_yDistance < 0)
      return DOWN;
   if (ab_isValid[2] && i_yDistance == 0 && i_xDistance > 0)
      return LEFT;
   if (ab_isValid[3] && i_yDistance == 0 && i_xDistance < 0)
      return RIGHT;
   // Case 2: Know when Lode is close
   if (ab_isValid[0] && ABS(i_xDistance) < 5 && i_yDistance > 0)
      if (randomNumber(10))
         return UP;
   if (ab_isValid[1] && ABS(i_xDistance) < 5 && i_yDistance < 0)
      if (randomNumber(2))
         return DOWN;
   if (ab_isValid[2] && ABS(i_yDistance) < 5 && i_xDistance > 0)
      return LEFT;
   if (ab_isValid[3] && ABS(i_yDistance) < 5 && i_xDistance < 0)
      return RIGHT;
   i_limit = 0;
   do {
      if (randomNumber(3))
         e_choice = r_monk.e_lastDirection;
      else
         e_choice = ae_directions[randomNumber(4)];
      i_limit++;
   } while (i_limit < 16 && !Player_isValidMove(r_monk, e_choice));
   if (i_limit == 16) e_choice = NONE;

   return e_choice;
}

/* ***** Treasure ***** */

void Treasure_check() {
   int i, j;
   if (aar_arena[r_lode.i_x][r_lode.i_y].e_type == TREASURE) {
      i_treasures--;
      if (i_treasures == 0) {
         Sound_play(REACH_SKY, r_lode.i_x, Ci_messageChannel);
         for (j = 0; j < Ci_arenaH; j++)
            for (i = 0; i < Ci_arenaW; i++)
               if (aar_arena[i][j].e_type == HIDDENSTAIRS) {
                  aar_arena[i][j].e_type == STAIRS;
                  aar_arena[i][j].i_moveSet = FROM_SIDES | FROM_UP | FROM_DOWN
                                            | TO_SIDES   | TO_UP   | TO_DOWN;
                  Figure_update(i, j, apr_figures[STAIRS]);
                  Figure_show(i, j);
               }
      } else {
         Sound_play(GET_TREASURE, r_lode.i_x, Ci_treasureChannel);
      }
      i_score+=50;
      aar_arena[r_lode.i_x][r_lode.i_y].e_type = EMPTY;
      Figure_update(r_lode.i_x, r_lode.i_y,
                    apr_figures[EMPTY]);
      Figure_show(r_lode.i_x, r_lode.i_y);
   }
}

/* ***** Status bar ***** */

void Status_displayScore() {
   GFX_Blit(pr_arenaBuffer, pr_backBuffer, Ci_scoreX, Ci_statusY, Ci_scoreX, Ci_statusY, 8*Ci_numberWidth, Ci_numberHeight);
   Number_displayAt(pr_backBuffer, Ci_scoreX, Ci_statusY, i_score, 7);
}

/* ***** Gameplay ***** */

/**
 * Prepare for the start of a new round of gameplay:
 * clear everything and start at the first stage.
 */

void Play_init() {
   b_retry = FALSE;
   b_gameOver = FALSE;
   i_stage = 1;
   i_score = 0;
   i_lives = 3;
}

/**
 * Load stage file; construct the arena.
 */

void Play_loadStage() {
   char sz_fileName[20];
   int i, j;
   FILE* fd;

   i_brokenCount = 0;
   i_brokenHead = 0;
   i_brokenTail = 0;
   i_treasures = 0;

   sprintf(sz_fileName, "stage%d.map", i_stage);

   GFX_Clear(GFX_Screen());
   GFX_UpdateScreen();

   GFX_Clear(pr_backBuffer);
   GFX_Blit(pr_background, pr_backBuffer, 0, 0, 0, 0, Ci_resX, Ci_resY);

   i_monkCount = 0;

   if (!ab_cachedStage[i_stage]) {
      fd = fopen(sz_fileName, "r");
      for(j = 0; j < Ci_arenaH; j++)
         for(i = 0; i < Ci_arenaW; i++)
            do {
               fread(&aac_mapCache[i_stage][i][j], sizeof(char), 1, fd);
            } while (aac_mapCache[i_stage][i][j] == '\n');
      fclose(fd);
      ab_cachedStage[i_stage] = TRUE;
   }

   for(j = 0; j < Ci_arenaH; j++) {
      for(i = 0; i < Ci_arenaW; i++) {
         switch (aac_mapCache[i_stage][i][j]) {
         case 'L':
            aar_arena[i][j].e_type = EMPTY;
            aar_arena[i][j].i_moveSet = FROM_SIDES | FROM_UP | FROM_DOWN
                                      | TO_SIDES             | TO_DOWN
                                      | FALL_FROM  | FALL_TO;
            r_lode.i_x = i;
            i_lodeSoundX = r_lode.i_x;
            r_lode.i_y = j;
            i_lodeSoundY = r_lode.i_y;
            r_lode.i_offsetX = 0;
            r_lode.i_offsetY = 0;
            r_lode.b_falling = FALSE;
            b_wasFalling = FALSE;
            r_lode.e_direction = NONE;
            r_lode.e_nextDirection = NONE;
            r_lode.e_lastDirection = RIGHT;
            r_lode.papr_runningLeft = apr_lodeRunningLeft;
            r_lode.papr_runningRight = apr_lodeRunningRight;
            r_lode.pr_defaultFigure = apr_figures[LODE];
            r_lode.pr_defaultFalling = pr_lodeFalling;
            r_lode.b_isLode = TRUE;
            break;
         case 'M':
            aar_arena[i][j].e_type = EMPTY;
            aar_arena[i][j].i_moveSet = FROM_SIDES | FROM_UP | FROM_DOWN
                                      | TO_SIDES             | TO_DOWN
                                      | FALL_FROM  | FALL_TO;
            ar_monks[i_monkCount].i_x = i;
            ar_monks[i_monkCount].i_y = j;
            ar_monks[i_monkCount].i_respawnX = i;
            ar_monks[i_monkCount].i_respawnY = j;
            ar_monks[i_monkCount].i_offsetX = 0;
            ar_monks[i_monkCount].i_offsetY = 0;
            ar_monks[i_monkCount].b_falling = FALSE;
            ar_monks[i_monkCount].b_locked = FALSE;
            ar_monks[i_monkCount].e_direction = NONE;
            ar_monks[i_monkCount].e_nextDirection = NONE;
            ar_monks[i_monkCount].e_lastDirection = LEFT;
            ar_monks[i_monkCount].papr_runningLeft = apr_monkRunningLeft;
            ar_monks[i_monkCount].papr_runningRight = apr_monkRunningRight;
            ar_monks[i_monkCount].pr_defaultFigure = apr_figures[MONK];
            ar_monks[i_monkCount].pr_defaultFalling = pr_monkFalling;
            ar_monks[i_monkCount].b_isLode = FALSE;
            i_monkCount++;
            break;
         case 'B':
            aar_arena[i][j].e_type = BRICK;
            aar_arena[i][j].i_moveSet = 0;
            break;
         case 'R':
            aar_arena[i][j].e_type = ROCK;
            aar_arena[i][j].i_moveSet = 0;
            break;
         case 'G':
            aar_arena[i][j].e_type = GHOSTBRICK;
            aar_arena[i][j].i_moveSet =              FROM_UP | FROM_DOWN
                                                             | TO_DOWN
                                      | FALL_FROM  | FALL_TO;
            break;
         case ' ':
            aar_arena[i][j].e_type = EMPTY;
            aar_arena[i][j].i_moveSet = FROM_SIDES | FROM_UP | FROM_DOWN
                                      | TO_SIDES             | TO_DOWN
                                      | FALL_FROM  | FALL_TO;
            break;
         case 's':
            aar_arena[i][j].e_type = HIDDENSTAIRS;
            aar_arena[i][j].i_moveSet = FROM_SIDES | FROM_UP | FROM_DOWN
                                      | TO_SIDES             | TO_DOWN
                                      | FALL_FROM  | FALL_TO;
            break;
         case 'S':
            aar_arena[i][j].e_type = STAIRS;
            aar_arena[i][j].i_moveSet = FROM_SIDES | FROM_UP | FROM_DOWN
                                      | TO_SIDES   | TO_UP   | TO_DOWN;
            break;
         case 'T':
            aar_arena[i][j].e_type = TREASURE;
            aar_arena[i][j].i_moveSet = FROM_SIDES | FROM_UP | FROM_DOWN
                                      | TO_SIDES             | TO_DOWN
                                      | FALL_FROM  | FALL_TO;
            i_treasures++;
            break;
         case '-':
            aar_arena[i][j].e_type = ROPE;
            aar_arena[i][j].i_moveSet = FROM_SIDES | FROM_UP | FROM_DOWN
                                      | TO_SIDES             | TO_DOWN
                                                   | FALL_TO;
            break;
         default:
            // Invalid file!?
            aar_arena[i][j].e_type = EMPTY;
            aar_arena[i][j].i_moveSet = FROM_SIDES | FROM_UP | FROM_DOWN
                                      | TO_SIDES             | TO_DOWN
                                      | FALL_FROM  | FALL_TO;
            break;
         }
         Figure_draw(i, j);
      }
   }


   GFX_Blit(pr_backBuffer, pr_arenaBuffer, 0, 0, 0, 0, Ci_resX, Ci_resY);

   Number_displayAt(pr_backBuffer, Ci_scoreX, Ci_statusY, i_score, 7);

   Number_displayAt(pr_backBuffer, Ci_stageX, Ci_statusY, i_stage, 2);

   for (i = 0; i < MIN(i_lives, 6); i++) {
      GFX_Draw(pr_backBuffer, apr_lodeRunningRight[0], Ci_livesX + (i*Ci_spriteW), Ci_statusY + Ci_livesOffset);

   }

   GFX_DisplayBuffer(pr_backBuffer);
}

/**
 * This is the actual gameplay loop.
 */

void Play_timedLoop() {
   int i;
   IO_Key* keys;

   keys = IO_GetKeyboardState();

   if (keys[IO_KEY_F]) {
      GFX_ToggleFullScreen();
   }

   if (keys[IO_KEY_ESC]) {
      b_playing = FALSE;
   }

   if (keys[IO_KEY_S]) {
      b_playing = FALSE;
      i_lives = 0;
   }

   // cheat :-)
   if (keys[IO_KEY_V]) {
      b_playing = FALSE;
      b_victory = TRUE;
   }

   if (i_breaking > 0)
      i_breaking--;
   else {
      if (keys[IO_KEY_UP] || keys[IO_KEY_I] || keys[IO_KEY_C])
         r_lode.e_nextDirection = UP;
      if (keys[IO_KEY_DOWN] || keys[IO_KEY_K] || keys[IO_KEY_T])
         r_lode.e_nextDirection = DOWN;
      if (keys[IO_KEY_LEFT] || keys[IO_KEY_J] || keys[IO_KEY_H])
         r_lode.e_nextDirection = LEFT;
      if (keys[IO_KEY_RIGHT] || keys[IO_KEY_L] || keys[IO_KEY_N])
         r_lode.e_nextDirection = RIGHT;
      if (keys[IO_KEY_RSHIFT] || keys[IO_KEY_U] || keys[IO_KEY_G])
         e_scheduledBreak = LEFT;
      if (keys[IO_KEY_END] || keys[IO_KEY_O] || keys[IO_KEY_R])
         e_scheduledBreak = RIGHT;
   }

   IO_ClearKeyboardBuffer();

   Player_clear(r_lode);
   for (i = 0; i < i_monkCount; i++)
      Player_clear(ar_monks[i]);

   Player_move(&r_lode);

   Bricks_update();

   for (i = 0; i < i_monkCount; i++) {
      if (Player_isStable(ar_monks[i])) {
         ar_monks[i].e_nextDirection = Monk_decideDirection(ar_monks[i]);
      }
      if (ar_monks[i].b_falling || i_gameTimer % 3)
         Player_move(&ar_monks[i]);
      if (
            (ABS((ar_monks[i].i_x * Ci_spriteW + ar_monks[i].i_offsetX) - (r_lode.i_x * Ci_spriteW + r_lode.i_offsetX)) < Ci_spriteW/2)
         && (ABS((ar_monks[i].i_y * Ci_spriteH + ar_monks[i].i_offsetY) - (r_lode.i_y * Ci_spriteH + r_lode.i_offsetY)) < Ci_spriteH/2)
          ) {
         b_playing = FALSE;
      }
   }

   if (Player_isStable(r_lode)) {
      Sound_lodeSound();
      Treasure_check();
      if (i_treasures == 0 && r_lode.i_y == 0) {
         b_victory = TRUE;
         b_playing = FALSE;
      }
      if (e_scheduledBreak != NONE) {
         r_lode.e_nextDirection = NONE;
         Lode_break(e_scheduledBreak);
         i_breaking = Ci_breakingTime;
         e_lastBreak = e_scheduledBreak;
         e_scheduledBreak = NONE;
      }
   }

   Player_draw(r_lode);
   for (i = 0; i < i_monkCount; i++)
      Player_draw(ar_monks[i]);

   GFX_DisplayBuffer(pr_backBuffer);
}

/**
 * Inner loop: this is the main gameplay loop.
 * It controls other loops.
 */

int Play_loop() {
   int i;
   int i_savedScore;

   b_playing = TRUE;
   b_victory = FALSE;
   i_gameTimer = 0;
   i_breaking = 0;
   e_scheduledBreak = NONE;

   MUS_SetVolume(128);
   MUS_Play(apr_songs[2], TRUE);

   Sound_play(READY, 128, Ci_messageChannel);

   Player_blinkAll(3,300,300);

   Sound_play(GO, 128, Ci_messageChannel);
   IO_Wait(500);

   while (b_playing) {
      i_savedScore = i_score;
      Play_timedLoop();
      if (i_score != i_savedScore) {
         Status_displayScore();
      }
      i_gameTimer++;
      for (i = 0; i < 5; i++) {
         IO_PollKeyboard();
         IO_Wait(10);
      }
   }

   SFX_Stop(Ci_lodeChannel);
   MUS_Stop(apr_songs[2]);
   IO_ClearKeyboardBuffer();

   if (b_victory) {
      MUS_Play(apr_songs[4], FALSE);
      IO_Wait(5000);
      Sound_play(YESSS, r_lode.i_x, Ci_messageChannel);
      IO_Wait(1500);
      return TRUE;
   }

   Sound_play(DEATH, r_lode.i_x, Ci_lodeChannel);
   IO_Wait(1500);
   return FALSE;
}

void Play_nextStage() {
   i_stage++;
   i_lives++;
}

/* ***** Game functions ***** */

void Game_openingCredits() {
   // Initial credits
   GFX_Clear(pr_backBuffer);
   pr_credits = GFX_Load(RES_DIR"credits.bmp");
   GFX_Draw(pr_backBuffer, pr_credits, 0, 0);
   GFX_DisplayBuffer(pr_backBuffer);
   IO_Wait(1000);
}

/**
 * Game initializations; allocates the game data and
 * initializes the utility libraries.
 */

int Game_init() {
   char sz_fileName[20];
   int i;

   LIB_Init();
   IO_Init();
   SFX_Init();
   MUS_Init();
   GFX_Init(Ci_resX, Ci_resY, 16);

   pr_backBuffer = GFX_CreateBlank(Ci_resX, Ci_resY);
   pr_arenaBuffer = GFX_CreateBlank(Ci_resX, Ci_resY);

   Game_openingCredits();

XLode_debug("Will load images...\n");

   pr_author = GFX_Load(RES_DIR"author.bmp");
   pr_title = GFX_Load(RES_DIR"title.bmp");
   pr_options = GFX_Load(RES_DIR"options.bmp");
   pr_lodeIcon = GFX_Load(RES_DIR"lodeicon.bmp");
   pr_background = GFX_Load(RES_DIR"background.bmp");
   pr_gameOver = GFX_Load(RES_DIR"gameover.bmp");
   pr_gameOverFlip = GFX_Load(RES_DIR"gameoverflip.bmp");
   //pr_gameOverFlip = GFX_CreateBlank(240, 40);
   //draw_sprite_v_flip(pr_gameOverFlip, pr_gameOver, 0, 0);

   apr_figures[LODE] = GFX_Load(RES_DIR"lode.bmp");
   apr_figures[MONK] = GFX_Load(RES_DIR"monk.bmp");
   apr_figures[BRICK] = GFX_Load(RES_DIR"brick.bmp");
   apr_figures[ROCK] = GFX_Load(RES_DIR"rock.bmp");
   apr_figures[GHOSTBRICK] = GFX_Load(RES_DIR"brick.bmp");
   apr_figures[STAIRS] = GFX_Load(RES_DIR"stairs.bmp");
   apr_figures[TREASURE] = GFX_Load(RES_DIR"treasure.bmp");
   apr_figures[ROPE] = GFX_Load(RES_DIR"rope.bmp");
   apr_figures[BROKENBRICK] = NULL;
   apr_figures[HIDDENSTAIRS] = NULL;
   apr_figures[EMPTY] = NULL;

   apr_samples[GET_TREASURE] = SFX_Load("treasure.wav");
   apr_samples[BREAK_BRICK] = SFX_Load("dig.wav");
   apr_samples[REACH_SKY] = SFX_Load("reach.wav");
   apr_samples[READY] = SFX_Load("ready.wav");
   apr_samples[GO] = SFX_Load("go.wav");
   apr_samples[DEATH] = SFX_Load("death.wav");
   apr_samples[YESSS] = SFX_Load("yesss.wav");
   apr_songs[0] = MUS_Load("intro.xm");
   apr_songs[1] = MUS_Load("song.xm");
   apr_songs[2] = MUS_Load("groove.xm");
   apr_songs[3] = MUS_Load("gameover.xm");
   apr_songs[4] = MUS_Load("victory.xm");

   apr_brokenAnimation[0] = NULL;
   apr_brokenAnimation[1] = GFX_Load(RES_DIR"brick1.bmp");
   apr_brokenAnimation[2] = GFX_Load(RES_DIR"brick2.bmp");
   apr_brokenAnimation[3] = GFX_Load(RES_DIR"brick3.bmp");
   apr_brokenAnimation[4] = GFX_Load(RES_DIR"brick.bmp");

   pr_lodeBreakingLeft = GFX_Load(RES_DIR"breakl.bmp");
   pr_lodeBreakingRight = GFX_Load(RES_DIR"breakr.bmp");

   // Lode running to the right
   apr_lodeRunningRight[0] = GFX_Load(RES_DIR"lode1r.bmp");
   apr_lodeRunningRight[1] = GFX_Load(RES_DIR"lode2r.bmp");
   apr_lodeRunningRight[2] = GFX_Load(RES_DIR"lode3r.bmp");
   apr_lodeRunningRight[3] = GFX_Load(RES_DIR"lode4r.bmp");

   // Lode on the rope to the right
   apr_lodeRunningRight[4] = GFX_Load(RES_DIR"rlode1r.bmp");
   apr_lodeRunningRight[5] = GFX_Load(RES_DIR"rlode2r.bmp");
   apr_lodeRunningRight[6] = GFX_Load(RES_DIR"rlode3r.bmp");
   apr_lodeRunningRight[7] = GFX_Load(RES_DIR"rlode4r.bmp");

   // Lode running to the left
   apr_lodeRunningLeft[0] = GFX_Load(RES_DIR"lode1l.bmp");
   apr_lodeRunningLeft[1] = GFX_Load(RES_DIR"lode2l.bmp");
   apr_lodeRunningLeft[2] = GFX_Load(RES_DIR"lode3l.bmp");
   apr_lodeRunningLeft[3] = GFX_Load(RES_DIR"lode4l.bmp");

   // Lode on the rope to the left
   apr_lodeRunningLeft[4] = GFX_Load(RES_DIR"rlode1l.bmp");
   apr_lodeRunningLeft[5] = GFX_Load(RES_DIR"rlode2l.bmp");
   apr_lodeRunningLeft[6] = GFX_Load(RES_DIR"rlode3l.bmp");
   apr_lodeRunningLeft[7] = GFX_Load(RES_DIR"rlode4l.bmp");

   pr_lodeFalling = GFX_Load(RES_DIR"lodef.bmp");

   // Monk running to the right
   apr_monkRunningRight[0] = GFX_Load(RES_DIR"monk1r.bmp");
   apr_monkRunningRight[1] = GFX_Load(RES_DIR"monk2r.bmp");
   apr_monkRunningRight[2] = GFX_Load(RES_DIR"monk3r.bmp");
   apr_monkRunningRight[3] = GFX_Load(RES_DIR"monk4r.bmp");

   // Monk on the rope to the right
   apr_monkRunningRight[4] = GFX_Load(RES_DIR"rmonk1r.bmp");
   apr_monkRunningRight[5] = GFX_Load(RES_DIR"rmonk2r.bmp");
   apr_monkRunningRight[6] = GFX_Load(RES_DIR"rmonk3r.bmp");
   apr_monkRunningRight[7] = GFX_Load(RES_DIR"rmonk4r.bmp");

   // Monk running to the left
   apr_monkRunningLeft[0] = GFX_Load(RES_DIR"monk1l.bmp");
   apr_monkRunningLeft[1] = GFX_Load(RES_DIR"monk2l.bmp");
   apr_monkRunningLeft[2] = GFX_Load(RES_DIR"monk3l.bmp");
   apr_monkRunningLeft[3] = GFX_Load(RES_DIR"monk4l.bmp");

   // Monk on the rope to the left
   apr_monkRunningLeft[4] = GFX_Load(RES_DIR"rmonk1l.bmp");
   apr_monkRunningLeft[5] = GFX_Load(RES_DIR"rmonk2l.bmp");
   apr_monkRunningLeft[6] = GFX_Load(RES_DIR"rmonk3l.bmp");
   apr_monkRunningLeft[7] = GFX_Load(RES_DIR"rmonk4l.bmp");

   pr_monkFalling = GFX_Load(RES_DIR"monkf.bmp");

   // Numbers
   for (i = 0; i < 10; i++) {
      sprintf(sz_fileName, RES_DIR"number%d.bmp", i);
      apr_numbers[i] = GFX_Load(sz_fileName);
   }
   // Sounds: Lode on the rope
   for (i = 0; i < Ci_ropeSampleCount; i++) {
      sprintf(sz_fileName, "rope%d.wav", i+1);
      apr_ropeSamples[i] = SFX_Load(sz_fileName);
   }
   // Lode on the stairs
   for (i = 0; i < Ci_stairsSampleCount; i++) {
      sprintf(sz_fileName, "stairs%d.wav", i+1);
      apr_stairsSamples[i] = SFX_Load(sz_fileName);
   }
   // Lode falling
   for (i = 0; i < Ci_fallSampleCount; i++) {
      sprintf(sz_fileName, "fall%d.wav", i+1);
      apr_fallSamples[i] = SFX_Load(sz_fileName);
   }
   // End of a fall
   for (i = 0; i < Ci_ouchSampleCount; i++) {
      sprintf(sz_fileName, "ouch%d.wav", i+1);
      apr_ouchSamples[i] = SFX_Load(sz_fileName);
   }
   // Lode running
   for (i = 0; i < Ci_runningSampleCount; i++) {
      sprintf(sz_fileName, "run%d.wav", i+1);
      apr_runningSamples[i] = SFX_Load(sz_fileName);
   }
XLode_debug("Loaded all images...\n");

   return TRUE;
}

/**
 * Introduction: show the game's opening
 */

int Game_intro() {
   int i_key;
   int i_option;
   int i;
   i_option = 0;

   // Intro animation
   //Animation_play("intro.fli", apr_songs[0]);
   // Game splash
   GFX_Clear(pr_backBuffer);
   GFX_Draw(pr_backBuffer, pr_title, Ci_titleX, Ci_titleY);
   GFX_Draw(pr_backBuffer, pr_options, Ci_optionsX, Ci_optionsY);
   GFX_Draw(pr_backBuffer, pr_author, Ci_authorX, Ci_authorY);
   IO_ClearKeyboardBuffer();
   MUS_Play(apr_songs[1], TRUE);
   MUS_SetVolume(180);
   do {
      GFX_Blit(pr_lodeIcon, pr_backBuffer, 0, 0,
            Ci_menuIconX,
            Ci_menuIconY + (Ci_menuIconDelta * i_option),
            Ci_spriteW, Ci_spriteH);
      GFX_FillRectangle(pr_backBuffer,
            Ci_menuIconX,
            Ci_menuIconY + (Ci_menuIconDelta * (i_option == 0?1:0)),
            Ci_menuIconX+Ci_spriteW + 1,
            Ci_menuIconY + (Ci_menuIconDelta * (i_option == 0?1:0)) | Ci_spriteH + 3,
            0x000000);
      GFX_DisplayBuffer(pr_backBuffer);

      i_key = IO_ReadExtKey();

      if (i_key == IO_KEY_UP || i_key == IO_KEY_DOWN) {
         i_option = (i_option == 0?1:0);
         Sound_play(GET_TREASURE, 128, Ci_treasureChannel);
      }

   } while (!(i_key == IO_KEY_SPACE || i_key == IO_KEY_ENTER));
   for (i = 0; i < 36; i++) {
      MUS_SetVolume((36-i)*(180/5));
      GFX_FillRectangle(GFX_Screen(), 0, 0, Ci_resX, (Ci_resY/36)*i, 0x000000);
      GFX_UpdateScreen();
      IO_Wait(10+(10*i_option)); // slower fade for exit
   }
   MUS_Stop(apr_songs[1]);
   if (i_option == 1)
      return FALSE;
   return TRUE;
}

/**
 * Display "Game Over" sequence.
 */

void Game_gameOver() {
   int i;
   int j;

   MUS_Play(apr_songs[3], FALSE);

   GFX_UpdateScreen();
   for (i = 10; i <= 50; i+=10) {
      for (j = 0; j <= Ci_gameOverHeight / 2; j+=Ci_gameOverHeight/10) {
         GFX_Blit(pr_arenaBuffer, pr_backBuffer,
              Ci_gameOverX, Ci_gameOverY,
              Ci_gameOverX, Ci_gameOverY,
              Ci_gameOverWidth, Ci_gameOverHeight);
         GFX_Blit(pr_gameOver, pr_backBuffer,
              0, 0, Ci_gameOverX, Ci_gameOverY+(j/2),
              Ci_gameOverWidth, Ci_gameOverHeight-(j*2));
         GFX_Blit(pr_backBuffer, GFX_Screen(),
              Ci_gameOverX, Ci_gameOverY,
              Ci_gameOverX, Ci_gameOverY,
              Ci_gameOverWidth, Ci_gameOverHeight);
         GFX_UpdateScreen();
         IO_Wait(i/(j+1)*2);
      }
      for (j = Ci_gameOverHeight / 2; j >= 0; j-=Ci_gameOverHeight/10) {
         GFX_Blit(pr_arenaBuffer, pr_backBuffer,
              Ci_gameOverX, Ci_gameOverY,
              Ci_gameOverX, Ci_gameOverY,
              Ci_gameOverWidth, Ci_gameOverHeight);
         GFX_Blit(pr_gameOverFlip, pr_backBuffer,
              0, 0, Ci_gameOverX, Ci_gameOverY+(j/2),
              Ci_gameOverWidth, Ci_gameOverHeight-(j*2));
         GFX_Blit(pr_backBuffer, GFX_Screen(),
              Ci_gameOverX, Ci_gameOverY,
              Ci_gameOverX, Ci_gameOverY,
              Ci_gameOverWidth, Ci_gameOverHeight);
         GFX_UpdateScreen();
         IO_Wait(i/(j+1)*2);
      }
      for (j = 0; j <= Ci_gameOverHeight / 2; j+=Ci_gameOverHeight/10) {          GFX_Blit(pr_arenaBuffer, pr_backBuffer,
              Ci_gameOverX, Ci_gameOverY,
              Ci_gameOverX, Ci_gameOverY,
              Ci_gameOverWidth, Ci_gameOverHeight);
         GFX_Blit(pr_gameOverFlip, pr_backBuffer,
              0, 0, Ci_gameOverX, Ci_gameOverY+(j/2),
              Ci_gameOverWidth, Ci_gameOverHeight-(j*2));
         GFX_Blit(pr_backBuffer, GFX_Screen(),
              Ci_gameOverX, Ci_gameOverY,
              Ci_gameOverX, Ci_gameOverY,
              Ci_gameOverWidth, Ci_gameOverHeight);
         GFX_UpdateScreen();
         IO_Wait(i/(j+1)*2);
      }
      for (j = Ci_gameOverHeight / 2; j >= 0; j-=Ci_gameOverHeight/10) {          GFX_Blit(pr_arenaBuffer, pr_backBuffer,
              Ci_gameOverX, Ci_gameOverY,
              Ci_gameOverX, Ci_gameOverY,
              Ci_gameOverWidth, Ci_gameOverHeight);
         GFX_Blit(pr_gameOver, pr_backBuffer,
              0, 0, Ci_gameOverX, Ci_gameOverY+(j/2),
              Ci_gameOverWidth, Ci_gameOverHeight-(j*2));
         GFX_Blit(pr_backBuffer, GFX_Screen(),
              Ci_gameOverX, Ci_gameOverY,
              Ci_gameOverX, Ci_gameOverY,
              Ci_gameOverWidth, Ci_gameOverHeight);
         GFX_UpdateScreen();
         IO_Wait(i/(j+1)*2);
      }
   }
   IO_ClearKeyboardBuffer();
   IO_ReadKey();
   MUS_Stop(apr_songs[3]);
}

/**
 * Game outer loop: the loop between rounds of gameplay
 * (display init screen, load stages, call inner loop...).
 */

void Game_loop() {

   while (TRUE) {
      if (!Game_intro())
         break;
      Play_init();
      while (i_lives >= 0) {
         Play_loadStage();
         if (Play_loop()) {
            Play_nextStage();
            if (i_stage > Ci_stageCount) {
               // then you beat the game? ;-)
               break;
            }
         } else {
            i_lives--;
         }
      }
      Game_gameOver();
      b_gameOver = TRUE;
   }
}

/**
 * Game finalizations; finalizes the game data and
 * utility libraries.
 */

int Game_done() {
   int i;

   for (i = 0; i < Ci_songCount; i++)
      MUS_Free(apr_songs[i]);
   for (i = 0; i < Ci_sampleCount; i++)
      SFX_Free(apr_samples[i]);
   for (i = 0; i < Ci_ropeSampleCount; i++)
      SFX_Free(apr_ropeSamples[i]);
   for (i = 0; i < Ci_stairsSampleCount; i++)
      SFX_Free(apr_stairsSamples[i]);
   for (i = 0; i < Ci_runningSampleCount; i++)
      SFX_Free(apr_runningSamples[i]);
   for (i = 0; i < Ci_fallSampleCount; i++)
      SFX_Free(apr_fallSamples[i]);
   for (i = 0; i < Ci_ouchSampleCount; i++)
      SFX_Free(apr_ouchSamples[i]);

   GFX_Free(pr_title);
   GFX_Free(pr_background);
   GFX_Free(pr_backBuffer);

   for (i = 0; i < Ci_figureCount; i++)
      if (apr_figures[i] != NULL)
         GFX_Free(apr_figures[i]);

   for (i = 0; i < Ci_brokenFrames; i++)
      if (apr_brokenAnimation[i] != NULL)
         GFX_Free(apr_brokenAnimation[i]);

   for (i = 0; i < 8; i++) {
      GFX_Free(apr_lodeRunningLeft[i]);
      GFX_Free(apr_lodeRunningRight[i]);
      GFX_Free(apr_monkRunningLeft[i]);
      GFX_Free(apr_monkRunningRight[i]);
   }
   GFX_Free(pr_lodeFalling);
   GFX_Free(pr_monkFalling);

   GFX_Done();
}

/* ***** Main */

/**
 * Main function; does global initializations and finalizations
 * and calls the game's outer loop.
 */

int main(int i_argc_, char* asz_argv_) {
   int i;

   printf("\nXLode\n");
   printf("(C)2001 Hisham Muhammad\n");
   XLode_debug("Opening stages... ");

   for (i = 0; i < Ci_stageCount; i++) {
      ab_cachedStage[i] = FALSE;
   }

   XLode_debug("done.\n");
   XLode_debug("Initializing game... ");

   if (! Game_init() ) {
      printf("Failed initializing.");
      return 1;
   }

   XLode_debug("done.\n");
   XLode_debug("Starting game loop.\n");

   Game_loop();

   XLode_debug("Finished game loop.\n");
   XLode_debug("Closing game... ");

   Game_done();

   XLode_debug("done.\n");

   return 0;

}
LIB_END_MACRO
