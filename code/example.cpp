#include <stdio.h>
#include <cfloat> // for FLT_MAX
#include <chrono>
#include <thread>
#include <cstdlib>
#include "graphics.h"

// Callbacks for event-driven window handling.
void delay (long milliseconds);
void drawscreen (void);
void act_on_new_button_func (void (*drawscreen_ptr) (void));
void act_on_button_press (float x, float y, t_event_buttonPressed event);
void act_on_mouse_move (float x, float y);
void act_on_key_press (char c);

// For example showing entering lines and rubber banding
static bool rubber_band_on = false;
static bool have_entered_line, have_rubber_line;
static bool line_entering_demo = false;
static float x1, y1, x2, y2;  
static int num_new_button_clicks = 0;

// const t_bound_box initial_coords = t_bound_box(0,1000,1000,0); // use this one for a conventional graphics coordinate system (ie. with inverted y; origin in top left)
const t_bound_box initial_coords = t_bound_box(0,0,1000,1000); // use this for the one you probably have seen in math (ie. with origin in bottom left)
// Note, that text orientation is not affected. Zero degrees will always be the normal, readable, orientation.

int main() {

	printf ("About to start graphics.\n");

	/* initialize display */
	
	// Set the name of the window (in UTF-8), with white background.
	init_graphics("Some Example Graphic😇s", WHITE); // you could pass a t_color RGB triplet instead

	// Set-up coordinates
	init_world(initial_coords);

	// Set the message at the bottom (also UTF-8)
	update_message("Interactive graphics example.");

	// Pass control to the window handling routine.  It will watch for user input
	// and redraw the screen / pan / zoom / etc. the graphics in response to user
	// input or windows being moved around the screen.  This is done by calling the
	// four callbacks below.  mouse movement and key press (keyboard) events aren't
	// enabled by default, so they aren't on yet. This function will return if and when
	// the user presses the proceed button.
	event_loop(act_on_button_press, NULL, NULL, drawscreen);   
	t_bound_box old_coords = get_visible_world(); // save the current view for later;

	/* animation section */
	init_world(initial_coords);
	clearscreen();
	update_message("Non-interactive (animation) graphics example.");
	setcolor (RED);
	setlinewidth(1);
	setlinestyle (DASHED);

	t_point start_point = t_point(100,0);
	for (int i = 0; i < 50; i++) {
		drawline (start_point, start_point + t_point(500, 10));
		flushinput();
		delay(50);
		start_point += t_point(5,10);
	}

	/* Draw an interactive still picture again.  I'm also creating one new button. */

	update_message("Interactive graphics #2 Click in graphics area to rubber band line.");
	create_button ("Window", "0 Clicks", act_on_new_button_func); // name is UTF-8

	// Enable mouse movement (not just button presses) and key board input.
	// The appropriate callbacks will be called by event_loop.
	set_keypress_input (true);
	set_mouse_move_input (true);
	line_entering_demo = true;

	// draw the screen once before calling event loop, so the picture is correct 
	// before we get user input.
	init_world(old_coords); // restore saved coords.
	drawscreen();
	event_loop(act_on_button_press, act_on_mouse_move, act_on_key_press, drawscreen);

	close_graphics ();
	printf ("Graphics closed down.\n");

	return (0);
}


void drawscreen (void) {

	/* redrawing routine for still pictures.  Graphics package calls  *
	 * this routine to do redrawing after the user changes the window *
	 * in any way.                                                    */
 
	set_draw_mode (DRAW_NORMAL);  // Should set this if your program does any XOR drawing in callbacks.
	clearscreen();  /* Should precede drawing for all drawscreens */

	setfontsize (10);
	setlinestyle (SOLID);
	setlinewidth (1);
	setcolor (PINK);
	
	{
		/**************
		 * Draw some rectangles using the indexed colours
		 **************/

		color_types color_indicies[] = {
			LIGHTGREY,
			DARKGREY,
			WHITE,
			BLACK,
			BLUE,
			GREEN,
			YELLOW,
			CYAN,
			RED,
			DARKGREEN,
			MAGENTA
		};

		const float rectangle_width = 50;
		const float rectangle_height = rectangle_width;
		const t_point start_point = t_point(150,30);
		t_bound_box color_rectangle = t_bound_box(start_point, rectangle_width, rectangle_height);
		
		// text is UTF-8
		drawtext (110, color_rectangle.get_ycenter(), "colors", 2*(start_point.x - 110), rectangle_height);

		for(size_t i = 0; i < sizeof(color_indicies)/sizeof(color_indicies[0]); ++i) {
			setcolor(color_indicies[i]);
			fillrect(color_rectangle);
			color_rectangle += t_point(rectangle_width,0);
		}

		drawtext (400, color_rectangle.get_ycenter(), "fillrect", FLT_MAX, rectangle_height);

		/********
		 * Draw some rectangles with RGB triplet colours
		 ********/

		std::srand(std::time(0)); // hack to make the colors change once per second.

		for(size_t i = 0; i < 3; ++i) {
			color_rectangle += t_point(rectangle_width,0);
			setcolor(std::rand(), std::rand(), std::rand()); // note that setcolor(..) also takes a t_color object.
			fillrect(color_rectangle);
		}

		setcolor(BLACK);
		drawrect(start_point, color_rectangle.top_right());

	}

	/********
	 * Draw some example lines, shapes, and arcs
	 ********/

	{
		float radius = 50;

		setcolor (BLACK);
		drawtext (250,150,"drawline",150.0, FLT_MAX);
		setlinestyle (SOLID);
		drawline (200,120,200,200);
		setlinestyle (DASHED);
		drawline (300,120,300,200);

		setcolor (MAGENTA);
		drawtext (450, 160, "drawellipticarc", 150, FLT_MAX);
		drawellipticarc (550, 160, 30, 60, 90, 270);
		drawtext (720, 160, "fillellipticarc", 150, FLT_MAX);
		fillellipticarc (800, 160, 30, 60, 90, 270);

		setcolor (BLUE);
		drawtext (190,300,"drawarc", radius*2, 150);
		drawarc (190,300,radius,0,270);
		drawarc (300,300,radius,0,-180);
		fillarc (410,300,radius,90, -90);
		fillarc (520,300,radius,0,360);
		setcolor (BLACK);
		drawtext (520,300,"fillarc", radius*2, 150);
		setcolor (BLUE);
		fillarc (630,300,radius,90,180);
		fillarc (740,300,radius,90,270);
		fillarc (850,300,radius,90,30);
	}
	
	/********
	 * Draw some rotated text
	 ********/

	{
		const float textsquare_width = 200;

		t_bound_box textsquare = t_bound_box(t_point(100,400), textsquare_width, textsquare_width);

		setcolor(BLUE);
		drawrect(textsquare);

		setcolor(GREEN);
		drawrect(textsquare.get_center(), textsquare.top_right());
		drawrect(textsquare.bottom_left(), textsquare.get_center());

		setcolor(RED);
		drawline(textsquare.bottom_left(), textsquare.top_right());
		drawline(textsquare.left(), textsquare.top(), textsquare.right(), textsquare.bottom());

		setcolor(BLACK);
		settextattrs(14,0); // set a reasonable font size, and zero rotation.
		drawtext(textsquare.get_xcenter(), textsquare.bottom(),        "0 degrees", textsquare.get_width(), textsquare.get_height());
		settextrotation(90);
		drawtext(textsquare.right(),       textsquare.get_ycenter(),  "90 degrees", textsquare.get_width(), textsquare.get_height());
		settextrotation(180);
		drawtext(textsquare.get_xcenter(), textsquare.top(),         "180 degrees", textsquare.get_width(), textsquare.get_height());
		settextrotation(270);
		drawtext(textsquare.left(),        textsquare.get_ycenter(), "270 degrees", textsquare.get_width(), textsquare.get_height());
	
		settextrotation(45);
		drawtext(textsquare.get_center(), "45 degrees", textsquare.get_width(), textsquare.get_height());
		settextrotation(135);
		drawtext(textsquare.get_center(), "135 degrees", textsquare.get_width(), textsquare.get_height());

		// it is probably a good idea to set text rotation back to zero,
		// as this is the way most text will be drawn, and you dont't want
		// to have to set it to zero every time you draw text. However,
		// there are checks that make in a noop to set the text size, or
		// rotation, to the current value.
		settextrotation(0);
	}

	/*******
	 * Draw some polygons
	 *******/
	
	{
		t_point polypts[3] = {{500,400},{450,480},{550,480}};
		t_point polypts2[4] = {{700,400},{650,480},{750,480}, {800,400}};

		setfontsize(10);
		setcolor (RED);
		fillpoly(polypts,3);
		fillpoly(polypts2,4);
		setcolor (BLACK);
		drawtext (500,450,"fillpoly1",66.7, FLT_MAX);
		drawtext (725,440,"fillpoly2",100.0, FLT_MAX);
		setcolor (DARKGREEN);
		setlinestyle(SOLID);
		t_bound_box rect = t_bound_box(350,550,650,670);
		drawtext_in(rect,"drawrect");
		drawrect (rect); 
	}

	/*********
	 * Draw some example text, with the bounding box functions
	 *********/

	{
		const float text_example_width = 800;

		const int num_lines = 2;
		const int max_strings_per_line = 3;

		const int num_strings_per_line[num_lines] = {3,2};

		// Text is UTF-8, so you can use special characters, as long as your font supports them.
		const char* const line_text[num_lines][max_strings_per_line] = {
			{
				"8 Point Text",
				"12 Point Text",
				"18 Point Text"
			},
			{
				"24 Point Text",
				"32 Point Text"
			}
		};

		const int text_sizes[num_lines][max_strings_per_line] = {
			{8,12,15},
			{24,32}
		};

		setcolor (BLACK);
		setlinestyle(DASHED);

		for(int i = 0; i < num_lines; ++i) {
			t_bound_box text_bbox = t_bound_box(t_point(100,710 + i*60), text_example_width/num_strings_per_line[i], 60);
			for(int j = 0; j < num_strings_per_line[i]; ++j) {
				setfontsize (text_sizes[i][j]);
				drawtext_in(text_bbox, line_text[i][j]);
				drawrect(text_bbox);

				text_bbox += t_point(text_example_width/num_strings_per_line[i],0);
			}
		}
	}

	/********
	 * Draw some lines of various thickness
	 ********/
	{
		setfontsize (10);
		
		setlinestyle (SOLID);
		drawtext (200,900,"Thin line (width 1)", 200.0, FLT_MAX);
		setlinewidth (1);
		drawline (100,920,300,920);
		drawtext (500,900,"Width 3 Line", 200.0, FLT_MAX);
		setlinewidth (3);
		drawline (400,920,600,920);
		drawtext (800,900,"Width 6 Line", 200.0, FLT_MAX);
		setlinewidth (6);
		drawline (700,920,900,920);
	}

	/********
	 * Draw the rubber-band line, if it's there
	 ********/

	setlinewidth (1);
	setcolor (GREEN);
	
	if (have_entered_line) 
		drawline (x1, y1, x2, y2);

	// Screen redraw will get rid of a rubber line.  
	have_rubber_line = false;
}


void delay (long milliseconds) {
	// if you would like to use this function in your
	// own code you will need to #include <chrono> and
	// <thread>
	std::chrono::milliseconds duration(milliseconds);
	std::this_thread::sleep_for(duration);
}


void act_on_new_button_func (void (*drawscreen_ptr) (void)) {

	char old_button_name[200], new_button_name[200];
	printf ("You pressed the new button!\n");
	setcolor (MAGENTA);
	setfontsize (12);
	drawtext (500, 500, "You pressed the new button!", 10000.0, FLT_MAX);
	sprintf (old_button_name, "%d Clicks", num_new_button_clicks);
	num_new_button_clicks++;
	sprintf (new_button_name, "%d Clicks", num_new_button_clicks);
	change_button_text (old_button_name, new_button_name);
}


void act_on_button_press (float x, float y, t_event_buttonPressed event) {

/* Called whenever event_loop gets a button press in the graphics *
 * area.  Allows the user to do whatever he/she wants with button *
 * clicks.                                                        */

	printf("User clicked a mouse button at coordinates (%f, %f)", x, y);
	if (event.shift_pressed || event.ctrl_pressed) {
		printf(" with %s%s%s pressed",
			event.shift_pressed ? "shift" : "",
			(event.shift_pressed && event.ctrl_pressed) ? " and " : "",
			event.ctrl_pressed ? "control" : ""
		);
	}
	putchar('\n');

	if (line_entering_demo) {
		if (rubber_band_on) {
			rubber_band_on = false;
			x2 = x;
			y2 = y;
			have_entered_line = true;  // both endpoints clicked on --> consider entered.

			// Redraw screen to show the new line.  Could do incrementally, but this is easier.
			drawscreen ();  
		}
		else {
			rubber_band_on = true;
			x1 = x;
			y1 = y;
			have_entered_line = false;
			have_rubber_line = false;
		}
	}

}



void act_on_mouse_move (float x, float y) {
	// function to handle mouse move event, the current mouse position in the current world coordinate
	// as defined as MAX_X and MAX_Y in init_world is returned

	printf ("Mouse move at (%f,%f)\n", x, y);
	if (rubber_band_on) {
		// Go into XOR mode.  Make sure we set the linestyle etc. for xor mode, since it is 
		// stored in different state than normal mode.
		set_draw_mode (DRAW_XOR); 
		setlinestyle (SOLID);
		setcolor (WHITE);
		setlinewidth (1);

		if (have_rubber_line) {
			// Erase old line.  
			drawline (x1, y1, x2, y2); 
		}
		have_rubber_line = true;
		x2 = x;
		y2 = y;
		drawline (x1, y1, x2, y2);  // Draw new line
	}
}


void act_on_key_press (char c) {
	// function to handle keyboard press event, the ASCII character is returned
	printf ("Key press: %c\n", c);
}
