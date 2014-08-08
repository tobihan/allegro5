/*
 *    Example program for the Allegro library, by David Capello.
 *
 *    This program demonstrates how to create a window that can be
 *    1) closed with the escape key and the close window button,
 *    2) resized from the window borders,
 *    3) maximized/restored, and
 */


#include <allegro.h>



/* TRUE if the user has pressed Escape key or the close button in the
   window */
int closed = FALSE;

int resizing_enabled = FALSE;
int resized = FALSE;

RESIZE_DISPLAY_EVENT last_event;



/* This callback is used in set_close_button_callback() so we know
   when the user presses the close button of the window */
void close_hook(void)
{
  closed = TRUE;
}



void resize_hook(RESIZE_DISPLAY_EVENT *ev)
{
   resized = TRUE;
   last_event = *ev;
}



void switch_resizing()
{
   const char* title = "Resize Window (not supported)";

   resizing_enabled = !resizing_enabled;

   if (resizing_enabled) {
      if (set_resize_callback(resize_hook) == 0) {
         title = "Resize Window (enabled)";
      }
      else {
         resizing_enabled = FALSE;
      }
   }
   else {
      if (set_resize_callback(NULL) == 0) {
         title = "Resize Window (disabled)";
      }
   }

   set_window_title(title);
}



int main(int argc, char *argv[])
{
   BITMAP* buffer = NULL;

   if (allegro_init() != 0)
      return 1;

   install_keyboard();
   install_mouse();

   set_color_depth(desktop_color_depth());
   if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 400, 300, 0, 0) != 0) {
      allegro_message("Unable to set any graphic mode\n%s\n",
		      allegro_error);
      return 1;
   }

   set_mouse_sprite(NULL);

   /* Enable window close button */
   set_close_button_callback(close_hook);

   switch_resizing();

   while (!closed) {
      if (resized) {
         if (acknowledge_resize() == 0) {
            if (buffer) {
               show_mouse(NULL);

               destroy_bitmap(buffer);
               buffer = NULL;
            }
         }
      }

      /* regenerate the buffer */
      if (!buffer) {
         int c = makecol(255, 255, 255);

         buffer = create_bitmap(SCREEN_W, SCREEN_H);
         clear_to_color(buffer, 0);
         ellipse(buffer,
                 SCREEN_W/2, SCREEN_H/2,
                 SCREEN_W/2-1, SCREEN_H/2-1, c);
         rect(buffer, 0, 0,
              SCREEN_W-1, SCREEN_H-1, c);

         textout_ex(buffer, font, "Press R to enable/disable resizing", 8, 8, c, -1);

         if (resized) {
            textprintf_ex(buffer, font, 8, 16, c, -1, "w,h = %d,%d %s",
                          last_event.new_w,
                          last_event.new_h,
                          last_event.is_maximized ? " (maximized)":
                          (last_event.is_restored ? " (restored)": ""));

            resized = FALSE;
         }

         show_mouse(buffer);
      }

      blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

      if (keypressed()) {
         int k = readkey();
         switch (k >> 8) {
            case KEY_ESC:
               closed = TRUE;
               break;
            case KEY_R:
               switch_resizing();
               break;
         }
      }

      rest(1);
   }

   return 0;
}

END_OF_MAIN()
