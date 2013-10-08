
#ifdef SIZE_640_480

   #define Ci_resX 640
   #define Ci_resY 480

   #define Ci_spriteW 16
   #define Ci_spriteH 20

   #define Ci_statusY 430
   #define Ci_scoreX 165
   #define Ci_livesX 320
   #define Ci_livesOffset 0
   #define Ci_stageX 505

   #define Ci_titleWidth 500
   #define Ci_optionsWidth 100
   #define Ci_authorWidth 400
   #define Ci_gameOverWidth 240
   #define Ci_gameOverHeight 40

   #define Ci_horizSprites 4
   /* unused for now */
   #define Ci_vertSprites 5

   #define Ci_deltaX 4
   #define Ci_deltaY 5

   #define Ci_numberWidth 13
   #define Ci_numberHeight 20

   #define Ci_menuIconX 260
   #define Ci_menuIconY 298
   #define Ci_menuIconDelta 34

   #define RES_DIR "640/"

#endif
#ifdef SIZE_320_200

   #define Ci_resX 320
   #define Ci_resY 200

   #define Ci_spriteW 8
   #define Ci_spriteH 8

   #define Ci_statusY 175
   #define Ci_scoreX 80
   #define Ci_livesX 160
   #define Ci_livesOffset 3
   #define Ci_stageX 251

   #define Ci_titleWidth 250
   #define Ci_optionsWidth 50
   #define Ci_authorWidth 200
   #define Ci_gameOverWidth 120
   #define Ci_gameOverHeight 20

   #define Ci_horizSprites 4
   /* unused for now */
   #define Ci_vertSprites 5

   #define Ci_deltaX 2
   #define Ci_deltaY 2

   #define Ci_numberWidth 8
   #define Ci_numberHeight 8

   #define Ci_menuIconX 130
   #define Ci_menuIconY 128
   #define Ci_menuIconDelta 20

   #define RES_DIR "320/"

#endif

#define Ci_titleX (Ci_resX / 2 - Ci_titleWidth / 2)
#define Ci_titleY (Ci_resY * 0.1)

#define Ci_optionsX (Ci_resX / 2 - Ci_optionsWidth / 2)
#define Ci_optionsY (Ci_resY * 0.583)

#define Ci_gameOverX (Ci_resX / 2 - Ci_gameOverWidth / 2)
#define Ci_gameOverY (Ci_resY / 2 - Ci_gameOverHeight / 2)

#define Ci_authorX (Ci_resX / 2 - Ci_authorWidth / 2)
#define Ci_authorY (Ci_resY * 0.83)

