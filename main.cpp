/*

> TODO

1. Make a constructor in Color32 that takes in a string value that represents a HEX color code (#RRGGBBAA), and converts it into RGBA, finally storing the values in the relevant members.

*/

#include <iostream>
#include <vector>

// Drawing Libraries
#include <gtkmm.h>
#include <cairomm/cairomm.h>

using namespace std;

class windowData {

    public:

        static int width;
        static int height;

        static void updateWindowSizeData (int w, int h) {
            width = w;
            height = h;
        }

};

int windowData::width (0);
int windowData::height (0);

// The drawing area widget
// An instance of 'Canvas' is created in the mainWindow class as a member which in-turn is set as a child of the main window.
class Canvas : public Gtk::DrawingArea {
    public:
        Canvas () {

            set_draw_func(sigc::mem_fun(*this, &Canvas::onDraw));

            auto controllerMotion = Gtk::EventControllerMotion::create ();
            controllerMotion->signal_motion ().connect (sigc::mem_fun (*this, &Canvas::onMouseMove));
            add_controller (controllerMotion);

            auto controllerGesture = Gtk::GestureClick::create ();
            controllerGesture->signal_pressed ().connect (sigc::mem_fun (*this, &Canvas::onMouseDown));
            controllerGesture->signal_released ().connect (sigc::mem_fun (*this, &Canvas::onMouseUp));
            add_controller (controllerGesture);

        }
        virtual ~Canvas () { /* Empty deconstructor for some reason. This needs to be empty otherwise it doesn't comple ! Need to find out why but too lazy. */ }

    protected:
        void onDraw (const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

        // Events
        void onMouseMove (double x, double y); 
        void onMouseDown (int n_press, double x, double y);
        void onMouseUp (int n_press, double x, double y);
        
};

// The main window widget
class mainWindow : public Gtk::Window {
    public:

        mainWindow ();

    protected:
    
        Canvas canvas;

        // Events
        bool onKeyPress (guint keyval, guint keycode, Gdk::ModifierType state);
        void onKeyRelease (guint keyval, guint keycode, Gdk::ModifierType state);
};

mainWindow::mainWindow () {

    set_title ("C++ UI Generator Project");
    set_child (canvas);

    auto controllerKey = Gtk::EventControllerKey::create ();
    controllerKey->signal_key_pressed ().connect (sigc::mem_fun (*this, &mainWindow::onKeyPress), false);
    controllerKey->signal_key_released ().connect (sigc::mem_fun (*this, &mainWindow::onKeyRelease));
    add_controller (controllerKey);

}

// 32-Bit Color (#RRGGBBAA in HEX form)
class Color32 {

    private:

        float red, green, blue, alpha;

    public:

        Color32 () {
            this->red = 0;
            this->green = 0;
            this->blue = 0;
            this->alpha = 0;
        }

        Color32 (float red, float green, float blue, float alpha) {
            this->red = red;
            this->green = green;
            this->blue = blue;
            this->alpha = alpha;
        }

        // To make them read-only.
        float r () { return red;   }
        float g () { return green; }
        float b () { return blue;  }
        float a () { return alpha; }

};

class StrokeData {

    private:
        
        int width;
        Color32 color;

    public:
        
        StrokeData () {
            this->width = 0;
        }

        StrokeData (int width, Color32 color) {
            this->width = width;
            this->color = color;
        }

        bool isNull () {
            return color.a () == 0.0;
        }

        int getWidth () {
            return width;
        }

        Color32 getColor () {
            return color;
        }

};

class FillData {

    private:

        // More members might be added.
        Color32 color;

    public:

        FillData () { }

        FillData (Color32 color) {
            this->color = color;
        }

        bool isNull () {
            return color.a () == 0.0;
        }

        Color32 getColor () {
            return color;
        }

};

class Coordinate {
    
    private:
        int x, y;

    public:

        Coordinate () {
            this->x = 0;
            this->y = 0;
        }

        Coordinate (int x, int y) {
            this->x = x;
            this->y = y;
        }

        int getX () { return x; }
        int getY () { return y; }

};

class Line {

    protected:

        Coordinate start;
        Coordinate end;

        StrokeData stroke;

    public:

        Line (Coordinate start, Coordinate end, StrokeData stroke) {
            this->start = start;
            this->end = end;
            this->stroke = stroke;
        }

        void draw (const Cairo::RefPtr<Cairo::Context>& context) {
            if (!stroke.isNull ()) {

                context->move_to (start.getX (), start.getY ());
                context->line_to (end.getX (), end.getY ());

                context->set_source_rgba (
                    stroke.getColor ().r (),
                    stroke.getColor ().g (),
                    stroke.getColor ().b (),
                    stroke.getColor ().a ()
                );

                context->set_line_width (stroke.getWidth ());

                context->stroke ();
            }
        }

        Coordinate getStart () {
            return start;
        }

        Coordinate getEnd () {
            return end;
        }

        void setStart (Coordinate newStart) {
            this->start = newStart;
        }

        void setEnd (Coordinate newEnd) {
            this->end = newEnd;
        }

};

class Path {

    private:

        vector<Coordinate> points;
        StrokeData stroke;

    public:

        Path (vector<Coordinate> points, StrokeData stroke) {
            this->points = points;
            this->stroke = stroke;
        }

        void draw (const Cairo::RefPtr<Cairo::Context>& context) {

            context->move_to (points[0].getX (), points[0].getY ());

            for (int i = 1; i < points.size (); i++) {
                context->line_to (points[i].getX (), points[i].getY ());
            }

            context->set_source_rgba (
                stroke.getColor ().r (),
                stroke.getColor ().g (),
                stroke.getColor ().b (),
                stroke.getColor ().a ()
            );

            context->set_line_width (stroke.getWidth ());

            context->stroke ();

        }

        vector<Coordinate> getPoints () {
            return points;
        }

        void addPoint (Coordinate newPoint) {
            points.push_back (newPoint);
        }

};

class Shape {

    protected:
    
        float top,
              left,

              width,
              height,

              originX,
              originY;

        StrokeData stroke;
        FillData fill;

    public:

        Shape (

            float top, 
            float left, 

            float width, 
            float height, 

            float originX, 
            float originY, 
            
            StrokeData stroke = StrokeData (),
            FillData fill = FillData ()

        ) {

            this->top = top;
            this->left = left;

            this->width = width;
            this->height = height;

            this->originX = originX;
            this->originY = originY;

            this->stroke = stroke;
            this->fill = fill;

        }

};


class Rectangle : Shape {

    public:

        Rectangle (

            float top, 
            float left, 

            float width, 
            float height, 

            float originX, 
            float originY, 
            
            StrokeData stroke = StrokeData (),
            FillData fill = FillData ()

        ) : Shape (top, left, width, height, originX, originY, stroke, fill) {
            cout << "Rectangle Created" << endl;
        } 

        void draw (const Cairo::RefPtr<Cairo::Context>& context) {

            float positionX, positionY;
            float sizeX, sizeY;

            if (width <= 1) {
                sizeX = width * windowData::width;
            } else {
                sizeX = width;
            }

            if (height <= 1) {
                sizeY = height * windowData::height;
            } else {
                sizeY = height;
            }

            if (top <= 1) {
                positionY = (top * windowData::height) - (originY * sizeY);
            } else {
                positionY = top - (originY * sizeY);
            }

            if (left <= 1) {
                positionX = (left * windowData::width) - (originX * sizeX);
            } else {
                positionX = left - (originX * sizeX);
            }

            context->rectangle (positionX, positionY, sizeX, sizeY);            

            if (!fill.isNull ()) {
                context->set_source_rgba (
                    fill.getColor ().r (),
                    fill.getColor ().g (),
                    fill.getColor ().b (),
                    fill.getColor ().a ()
                );
                context->fill_preserve ();
            }

            if (!stroke.isNull ()) {
                context->set_source_rgba (
                    stroke.getColor ().r (),
                    stroke.getColor ().g (),
                    stroke.getColor ().b (),
                    stroke.getColor ().a ()
                );
                context->set_line_width (stroke.getWidth ());
                context->stroke ();
            }
            
        }

};


Rectangle rect_1 (0.5, 0.5, 0.5, 0.5, 0.5, 0.5, StrokeData (5, Color32 (0, 0, 0, 1)), FillData (Color32 (1, 0, 0, 1)));
Line someLine (Coordinate (100, 100), Coordinate (120, 50), StrokeData (5, Color32 (0, 0, 0, 1)));
Path newPath ({Coordinate (1, 1)}, StrokeData (5, Color32 (0, 0, 0, 1)));

bool mainWindow::onKeyPress (guint keyval, guint keycode, Gdk::ModifierType state) {
    return true;
}

void mainWindow::onKeyRelease (guint keyval, guint keycode, Gdk::ModifierType state) {
}

void Canvas::onDraw (const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
    windowData::updateWindowSizeData (width, height);
    //rect_1.draw (cr);
    //someLine.draw (cr);
    newPath.draw (cr);
}

void Canvas::onMouseMove (double x, double y) {
    //someLine.setEnd (Coordinate (x, y));
    queue_draw ();
}

void onMouseClick (Canvas& canvas,int clicks, double x, double y) {
    newPath.addPoint (Coordinate (x, y));
    canvas.queue_draw ();
}

void Canvas::onMouseDown (int n_press, double x, double y) {
    onMouseClick (*this, n_press, x, y);
}

void Canvas::onMouseUp (int n_press, double x, double y) {
}

int main(int argc, char** argv)
{
  auto app = Gtk::Application::create("org.gtkmm.example");
  return app->make_window_and_run<mainWindow>(argc, argv);
}